#include <stdhdrs.h>
#include <msgclient.h>
#include <sharemem.h>
#include <timers.h>

typedef struct local_tag
{
	struct {
		_32 bServer : 1;
		_32 bClient : 1;
	} flags;
   _32 MsgBaseClient;
   _32 MsgBaseServer;
   _32 waiting;
	_64 start;
	_64 end;
	_64 accum;
	_64 passes;
	_64 min;
	_64 max;
	_64 bytes;

   // buffer to send from...
   _32 data[4096];
   _32 data_in[4096];
} LOCAL;
static LOCAL l;

#ifdef __WATCOMC__
extern _64 GetCPUTicks();
#pragma aux GetCPUTicks = "rdtsc"
#define SetTick(var)   ( (var) = GetCPUTicks() )
#else
#define SetTick(var) 	asm( WIDE("rdtsc\n") : "=A"(var) );
#endif

void OpenClient( void ) /*FOLD00*/
{
   l.MsgBaseClient = LoadService( WIDE("Benchmark Service"), NULL );
}

static int CPROC ServiceFunction( _32 *params, _32 param_length /*FOLD00*/
										  , _32 *result, _32 *result_length )
{
	// echo the data we got back to the client...
// most other things will do useful functions in functions.
   //ThreadTo( ClientThread, params[-1] );
	MemCpy( result, params, param_length );
   *result_length = param_length;
   return TRUE;
}


#define NUM_FUNCTIONS (sizeof(functions)/sizeof( server_function))
SERVER_FUNCTION functions[] =
{
	ServerFunctionEntry( NULL )
, ServerFunctionEntry( NULL  )
, ServerFunctionEntry( NULL )
, ServerFunctionEntry( NULL )
, ServerFunctionEntry( ServiceFunction )
};



void OpenServer( void ) /*FOLD00*/
{
   l.MsgBaseServer = RegisterService( WIDE("Benchmark Service"), functions, NUM_FUNCTIONS );
}

void DumpStats( void ) /*FOLD00*/
{
	printf( WIDE("Accumulated ticks: %Ld\n"), l.accum );
	printf( WIDE("Packets          : %Ld\n"), l.passes );
	printf( WIDE("Bytes            : %Ld\n"), l.bytes );
	printf( WIDE("Min ticks       : %Ld\n"), l.min );
	printf( WIDE("Max ticks        : %Ld\n"), l.max );
	printf( WIDE("Avg ticks        : %Ld\n"), l.passes?(l.accum / l.passes):0);
	l.min = 0;
	l.max = 0;
	l.accum = 0;
	l.bytes = 0;
   l.passes = 0;

}
#define MSG_Test1 MSG_EventUser

void Test1Byte( void ) /*FOLD00*/
{
	_32 responce;
   _32 reslen = 4096;
	int n;
   _32 sec = time(NULL);
	while( (sec+5) > time(NULL) )
	//for( n = 0; n < 10000; n++ )
	{
		SetTick( l.start );
		if( !TransactServerMessage( l.MsgBaseClient + MSG_Test1
										  , l.data, 1
										  , &responce, l.data_in, &reslen ) )
		{
			printf( WIDE("Transact message generally failed - suppose that's a timeout.\n") );
		}
		else
		{
			if( responce & 0x80000000 )
				lprintf( WIDE("Responce was faillure from the server...") );
			else
		{
			_64 del;
			SetTick( l.end );
			l.accum += (del = l.end - l.start);
			l.passes++;
			l.bytes += reslen;
			if( (!l.min) || (del < l.min) )
				l.min = del;
			if( del > l.max )
				l.max = del;
			l.waiting = 0;
		}
		}
	}
   DumpStats();
}

void Test1000Byte( void ) /*FOLD00*/
{
	_32 responce;
   _32 reslen = 4096;
	int n;
   _32 sec = time(NULL);
	while( (sec+5) > time(NULL) )
	//for( n = 0; n < 10000; n++ )
	{
		SetTick( l.start );
		if( !TransactServerMessage( l.MsgBaseClient + MSG_Test1
										  , l.data, 1000
										  , &responce, l.data_in, &reslen ) )
		{
			printf( WIDE("Transact message generally failed - suppose that's a timeout.\n") );
		}
		else
		{
			if( responce & 0x80000000 )
				lprintf( WIDE("Responce was faillure from the server...") );
			else
			{
				_64 del;
				SetTick( l.end );
				l.accum += (del = l.end - l.start);
				l.passes++;
				l.bytes += reslen;
				if( (!l.min) || (del < l.min) )
					l.min = del;
				if( del > l.max )
					l.max = del;
				l.waiting = 0;
			}
		}
	}
   DumpStats();
}


int main( int argc, char **argv ) /*FOLD00*/
{
	if( argc < 2 )
	{
		printf( WIDE("Usage: %s [scUu]\n"), argv[0] );
		printf( WIDE("  s - server\n") );
		printf( WIDE("  c - client\n") );
		printf( WIDE(" s and c may be specified together to test single-process\n") );
      return 0;
	}
	while( argc > 1 )
	{
		char *p = argv[1];
		while( p[0] )
		{
			switch( p[0]  )
			{
			case 's':
			case 'S':
            l.flags.bServer = 1;
            break;
			case 'c':
			case 'C':
            l.flags.bClient = 1;
				break;
			}
         p++;
		}
		argv++;
      argc--;
	}
	if( l.flags.bServer )
	{
      OpenServer();
	}
	if( l.flags.bClient )
	{
      OpenClient();
		if( l.MsgBaseClient != INVALID_INDEX )
		{
		   // all tests are client based.
			Test1Byte();
		   // all tests are client based.
			Test1000Byte();
		}
	}
	else if( l.flags.bServer ) // hang out here waiting for clients...
      WakeableSleep( SLEEP_FOREVER );
   return 0;
}
