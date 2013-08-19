#include <stdhdrs.h>
#include <sharemem.h>
#include <timers.h>

#define SIZE 128
_32 threads;

PTRSZVAL CPROC tester( PTHREAD thread )
{
	PTRSZVAL psv = GetThreadParam( thread );
	POINTER p;
	int n;
	int m;
	for( n = 0; n < 10000000; n++ )
	{
		p = Allocate( SIZE );
		memset( p, psv, SIZE );
		//Relinquish();
		for( m = 0; m < SIZE; m++ )
			if( ((_8*)p)[m] != psv )
				printf( "FAIL %d %d", m, psv );
		Release( p );
	}
	threads--;
}




int main( void )
{
	int n;
	for( n = 0; n < 50; n++ )
	{
      threads++;
      ThreadTo( tester, n );
	}
	while( threads )
{
printf( "thread: %d\n", threads );
WakeableSleep( 1000 );
}
}

