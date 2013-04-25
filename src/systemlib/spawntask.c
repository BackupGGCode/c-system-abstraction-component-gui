#define TASK_INFO_DEFINED
#include <stdhdrs.h>
#include <sack_types.h>
#include <deadstart.h>
#include <sharemem.h>
#include <idle.h>

#include <timers.h>
#include <filesys.h>

#ifdef __LINUX__
#include <sys/poll.h>
#include <sys/wait.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
extern char **environ;
#endif


#include <system.h>


//--------------------------------------------------------------------------

SACK_SYSTEM_NAMESPACE

#include "taskinfo.h"

typedef struct task_info_tag TASK_INFO;



//--------------------------------------------------------------------------

#ifdef WIN32
static int DumpErrorEx( DBG_VOIDPASS )
#define DumpError() DumpErrorEx( DBG_VOIDSRC )
{
	_lprintf(DBG_RELAY)( WIDE("Failed create process:%d"), GetLastError() );
   return 0;
}
#endif

//--------------------------------------------------------------------------
#ifdef __LINUX__
int CanRead( int handle )
{
	fd_set n;
	int rval;
	struct timeval tv;
	FD_ZERO( &n );
	FD_SET( handle, &n );
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	rval = select( handle+1, &n, NULL, NULL, &tv );
	//lprintf( "select : %d %d\n", rval, handle );
	if( rval > 0 )
		return TRUE;
	return  FALSE;
}
#endif

//--------------------------------------------------------------------------
extern PTRSZVAL CPROC WaitForTaskEnd( PTHREAD pThread );


static PTRSZVAL CPROC HandleTaskOutput(PTHREAD thread )
{
	PTASK_INFO task = (PTASK_INFO)GetThreadParam( thread );
	{
		// read input from task, montiro close and dispatch TaskEnd Notification also.
		{
			PHANDLEINFO phi = &task->hStdOut;
			PTEXT pInput = SegCreate( 4096 );
			int done, lastloop;
			Hold( task );
			done = lastloop = FALSE;
			do
			{
				_32 dwRead, dwAvail;
				if( done )
					lastloop = TRUE;
#ifdef _WIN32
				//while(  )
				{

#else
					while( CanRead( phi->handle ) )
#endif
					{
						if( task->flags.log_input )
							lprintf( WIDE( "Go to read task's stdout." ) );
#ifdef _WIN32
						if( ReadFile( phi->handle
										, GetText( pInput ), (DWORD)(GetTextSize( pInput ) - 1)
										, &dwRead, NULL ) )  //read the  pipe
						{
#else
							dwRead = read( phi->handle
											 , GetText( pInput )
											 , GetTextSize( pInput ) - 1 );
							if( !dwRead )
							{
#ifdef _DEBUG
												//lprintf( WIDE( "Ending system thread because of broke pipe! %d" ), errno );
#endif
#ifdef WIN32
								continue;
#else
												//lprintf( WIDE( "0 read = pipe failure." ) );
								break;
#endif
							}
#endif
							if( task->flags.log_input )
								lprintf( WIDE( "got read on task's stdout: %d" ), dwRead );
							if( task->flags.bSentIoTerminator )
							{
								if( dwRead > 1 )
									dwRead--;
								else
								{
									if( task->flags.log_input )
										lprintf( WIDE( "Finished, no more data, task has ended; no need for once more around" ) );
                           lastloop = 1;
									break; // we're done; task ended, and we got an io terminator on XP
								}
							}
							//lprintf( WIDE( "result %d" ), dwRead );
							GetText( pInput )[dwRead] = 0;
							pInput->data.size = dwRead;
							//LogBinary( GetText( pInput ), GetTextSize( pInput ) );
							if( task->OutputEvent )
							{
								task->OutputEvent( task->psvEnd, task, GetText( pInput ), GetTextSize( pInput ) );
							}
							pInput->data.size = 4096;
#ifdef _WIN32
						}
						else
						{
							DWORD dwError = GetLastError();
							if( ( dwError == ERROR_OPERATION_ABORTED ) && task->flags.process_ended )
							{
								if( PeekNamedPipe( phi->handle, NULL, 0, NULL, &dwAvail, NULL ) )
								{
									if( dwAvail > 0 )
									{
										lprintf( WIDE( "caught data" ) );
										// there is still data in the pipe, just that the process closed
										// and we got the sync even before getting the data.
									}
									else
										break;
								}
							}
						}
#endif
					}
#ifdef _WIN32
				}
#endif
				//allow a minor time for output to be gathered before sending
				// partial gathers...
				if( task->flags.process_ended )
#ifdef _WIN32
				{
					// Ending system thread because of process exit!
					done = TRUE;
				}
#else
				//if( !dwRead )
				//	break;
				if( task->pid == waitpid( task->pid, NULL, WNOHANG ) )
				{
					Log( WIDE( "Setting done event on system reader." ) );
					done = TRUE; // do one pass to make sure we completed read
				}
				else
				{
					//lprintf( WIDE( "process active..." ) );
					Relinquish();
				}
#endif
			}
			while( !lastloop );
#ifdef _DEBUG
			if( lastloop )
			{
				//DECLTEXT( msg, WIDE( "Ending system thread because of process exit!" ) );
				//EnqueLink( phi->pdp->&ps->Command->Output, &msg );
			}
			else
			{
				//DECLTEXT( msg, WIDE( "Guess we exited from broken pipe" ) );
				//EnqueLink( phi->pdp->&ps->Command->Output, &msg );
			}
#endif
			LineRelease( pInput );
#ifdef _WIN32
			CloseHandle( task->hReadIn );
			CloseHandle( task->hReadOut );
			CloseHandle( task->hWriteIn );
			CloseHandle( task->hWriteOut );
			//lprintf( WIDE( "Closing process handle %p" ), task->pi.hProcess );
			phi->hThread = 0;
#else
			//close( phi->handle );
			close( task->hStdIn.pair[1] );
			close( task->hStdOut.pair[0] );
         //close( task->hStdErr.pair[0] );
#define INVALID_HANDLE_VALUE -1
#endif
			if( phi->handle == task->hStdIn.handle )
				task->hStdIn.handle = INVALID_HANDLE_VALUE;
			phi->handle = INVALID_HANDLE_VALUE;

			Release( task );
			//WakeAThread( phi->pdp->common.Owner );
			return 0xdead;
		}
	}
}

//--------------------------------------------------------------------------

static int FixHandles( PTASK_INFO task )
{
#ifdef WIN32
	if( task->pi.hProcess )
		CloseHandle( task->pi.hProcess );
	task->pi.hProcess = 0;
	if( task->pi.hProcess )
		CloseHandle( task->pi.hThread );
	task->pi.hThread = 0;
#endif
   return 0; // must return 0 so expression continues
}

//--------------------------------------------------------------------------
#ifdef WIN32
extern HANDLE GetImpersonationToken( void );
#endif
// Run a program completely detached from the current process
// it runs independantly.  Program does not suspend until it completes.
// No way at all to know if the program works or fails.
SYSTEM_PROC( PTASK_INFO, LaunchPeerProgramExx )( CTEXTSTR program, CTEXTSTR path, PCTEXTSTR args
															  , int flags
															  , TaskOutput OutputHandler
															  , TaskEnd EndNotice
															  , PTRSZVAL psv
																DBG_PASS
															  )
{
	PTASK_INFO task;
	if( program && program[0] )
	{
#ifdef WIN32
		PVARTEXT pvt = VarTextCreateEx( DBG_VOIDRELAY );
		PTEXT cmdline;
		PTEXT final_cmdline;
      LOGICAL needs_quotes;
		TEXTSTR expanded_path = ExpandPath( program );
		TEXTSTR expanded_working_path = path?ExpandPath( path ):ExpandPath( WIDE(".") );
		int first = TRUE;
		//TEXTCHAR saved_path[256];
		task = (PTASK_INFO)AllocateEx( sizeof( TASK_INFO ) DBG_RELAY );
		MemSet( task, 0, sizeof( TASK_INFO ) );
		task->psvEnd = psv;
		task->EndNotice = EndNotice;
		xlprintf(LOG_DEBUG+1)( WIDE("%s[%s]"), path, expanded_working_path );
		if( StrCmp( path, WIDE(".") ) == 0 )
		{
			path = NULL;
			Release( expanded_working_path );
			expanded_working_path = NULL;
		}
		if( expanded_path && StrChr( expanded_path, ' ' ) )
			needs_quotes = TRUE;
		else if( expanded_working_path && StrChr( expanded_working_path, ' ' ) )
			needs_quotes = TRUE;
		else
			needs_quotes = FALSE;

		xlprintf(LOG_DEBUG+1)( WIDE( "quotes?%s path [%s] program [%s]"), needs_quotes?WIDE( "yes"):WIDE( "no"), expanded_working_path, expanded_path );

		if( needs_quotes )
			vtprintf( pvt, WIDE( "\"" ) );

      /*
		if( !IsAbsolutePath( expanded_path ) && expanded_working_path )
		{
			//lprintf( "needs working path too" );
			vtprintf( pvt, WIDE("%s/"), expanded_working_path );
		}
      */
		vtprintf( pvt, WIDE("%s"), expanded_path );

		if( needs_quotes )
			vtprintf( pvt, WIDE( "\"" ) );

		if( flags & LPP_OPTION_FIRST_ARG_IS_ARG )
			;
		else
		{
			if( args && args[0] )// arg[0] is passed with linux programs, and implied with windows.
				args++;
		}
		while( args && args[0] )
		{
			if( args[0][0] == 0 )
				vtprintf( pvt, WIDE( " \"\"" ) );
			else if( StrChr( args[0], ' ' ) )
				vtprintf( pvt, WIDE(" \"%s\""), args[0] );
			else
				vtprintf( pvt, WIDE(" %s"), args[0] );
			first = FALSE;
			args++;
		}
		cmdline = VarTextGet( pvt );
		vtprintf( pvt, WIDE( "cmd.exe %s" ), GetText( cmdline ) );
		final_cmdline = VarTextGet( pvt );
		VarTextDestroy( &pvt );
		MemSet( &task->si, 0, sizeof( STARTUPINFO ) );
		task->si.cb = sizeof( STARTUPINFO );
      /*
		if( path )
		{
			GetCurrentPath( saved_path, sizeof( saved_path ) );
			SetCurrentPath( path );
		}
      */
		task->OutputEvent = OutputHandler;
		if( OutputHandler )
		{
			SECURITY_ATTRIBUTES sa;

			sa.bInheritHandle = TRUE;
			sa.lpSecurityDescriptor = NULL;
			sa.nLength = sizeof( sa );

			CreatePipe( &task->hReadOut, &task->hWriteOut, &sa, 0 );
			//CreatePipe( &hReadErr, &hWriteErr, &sa, 0 );
			CreatePipe( &task->hReadIn, &task->hWriteIn, &sa, 0 );
			task->si.hStdInput = task->hReadIn;
			task->si.hStdError = task->hWriteOut;
			task->si.hStdOutput = task->hWriteOut;
			task->si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			if( !( flags & LPP_OPTION_DO_NOT_HIDE ) )
				task->si.wShowWindow = SW_HIDE;
			else
				task->si.wShowWindow = SW_SHOW;
		}
		else
		{
			task->si.dwFlags |= STARTF_USESHOWWINDOW;
			if( !( flags & LPP_OPTION_DO_NOT_HIDE ) )
				task->si.wShowWindow = SW_HIDE;
			else
				task->si.wShowWindow = SW_SHOW;
		}

		{
			HINSTANCE hShellProcess = 0;
			int success = 0;
#ifdef WIN32
			if( flags & LPP_OPTION_IMPERSONATE_EXPLORER )
			{
				HANDLE hExplorer = GetImpersonationToken();
				if( ( CreateProcessAsUser( hExplorer, NULL //program
												 , GetText( cmdline )
												 , NULL, NULL, TRUE
												 , CREATE_NEW_PROCESS_GROUP
												 , NULL
												 , expanded_working_path
												 , &task->si
												 , &task->pi ) || FixHandles(task) || DumpError() ) ||
					( CreateProcessAsUser( hExplorer, program
												, GetText( cmdline )
												, NULL, NULL, TRUE
												, CREATE_NEW_PROCESS_GROUP
												, NULL
												, expanded_working_path
												, &task->si
												, &task->pi ) || FixHandles(task) || DumpError() ) ||
					( CreateProcessAsUser( hExplorer, program
												, NULL // GetText( cmdline )
												, NULL, NULL, TRUE
												, CREATE_NEW_PROCESS_GROUP
												, NULL
												, expanded_working_path
												, &task->si
												, &task->pi ) || FixHandles(task) || DumpError() ) ||
					( CreateProcessAsUser( hExplorer, WIDE( "cmd.exe" )
												, GetText( final_cmdline )
												, NULL, NULL, TRUE
												, CREATE_NEW_PROCESS_GROUP
												, NULL
												, expanded_working_path
												, &task->si
												, &task->pi ) || FixHandles(task) || DumpError() )
				  )
				{
					success = 1;
				}
				CloseHandle( hExplorer );
			}
			else
#endif
			{
				if( ( CreateProcess( NULL //program
										 , GetText( cmdline )
										 , NULL, NULL, TRUE
										 , 0//CREATE_NEW_PROCESS_GROUP
										 , NULL
										 , expanded_working_path
										 , &task->si
										 , &task->pi ) || FixHandles(task) || DumpError() ) ||
					( CreateProcess( program
										, GetText( cmdline )
										, NULL, NULL, TRUE
										, 0//CREATE_NEW_PROCESS_GROUP
										, NULL
										, expanded_working_path
										, &task->si
										, &task->pi ) || FixHandles(task) || DumpError() ) ||
					( CreateProcess( program
										, NULL // GetText( cmdline )
										, NULL, NULL, TRUE
										, 0//CREATE_NEW_PROCESS_GROUP
										, NULL
										, expanded_working_path
										, &task->si
										, &task->pi ) || FixHandles(task) || DumpError() ) ||
					( TryShellExecute( task, expanded_working_path, program, cmdline ) ) ||
					( CreateProcess( WIDE( "cmd.exe" )
										, GetText( final_cmdline )
										, NULL, NULL, TRUE
										, 0//CREATE_NEW_PROCESS_GROUP
										, NULL
										, expanded_working_path
										, &task->si
										, &task->pi ) || FixHandles(task) || DumpError() ) ||
               0
				  )
				{
					success = 1;
				}
			}
			if( success )
			{
				//CloseHandle( task->hReadIn );
				//CloseHandle( task->hWriteOut );
				xlprintf(LOG_DEBUG+1)( WIDE("Success running %s[%s] in %s (%p): %d"), program, GetText( cmdline ), expanded_working_path, task->pi.hProcess, GetLastError() );
				if( OutputHandler )
				{
					task->hStdIn.handle 	= task->hWriteIn;
					task->hStdIn.pLine 	= NULL;
					//task->hStdIn.pdp 		= pdp;
					task->hStdIn.hThread  = 0;
					task->hStdIn.bNextNew = TRUE;

					task->hStdOut.handle 	 = task->hReadOut;
					task->hStdOut.pLine 	 = NULL;
					//task->hStdOut.pdp 		 = pdp;
					task->hStdOut.bNextNew = TRUE;
					task->hStdOut.hThread  = ThreadTo( HandleTaskOutput, (PTRSZVAL)task );
					ThreadTo( WaitForTaskEnd, (PTRSZVAL)task );
				}
				else
				{
					//task->hThread =
					ThreadTo( WaitForTaskEnd, (PTRSZVAL)task );
				}
			}
			else
			{
				xlprintf(LOG_DEBUG+1)( WIDE("Failed to run %s[%s]: %d"), program, GetText( cmdline ), GetLastError() );
				CloseHandle( task->hWriteIn );
				CloseHandle( task->hReadIn );
				CloseHandle( task->hWriteOut );
				CloseHandle( task->hReadOut );
				CloseHandle( task->pi.hProcess );
				CloseHandle( task->pi.hThread );
				Release( task );
				task = NULL;
			}
		}
		LineRelease( cmdline );
		LineRelease( final_cmdline );
		Release( expanded_working_path );
		Release( expanded_path );
      /*
      if( path )
		SetCurrentPath( saved_path );
      */
		return task;
#endif
#ifdef __LINUX__
		{
			pid_t newpid;
			TEXTCHAR saved_path[256];
         xlprintf(LOG_ALWAYS)( "Expand Path was not implemented in linux cod!" );
			task = (PTASK_INFO)Allocate( sizeof( TASK_INFO ) );
			MemSet( task, 0, sizeof( TASK_INFO ) );
			task->psvEnd = psv;
			task->EndNotice = EndNotice;
			task->OutputEvent = OutputHandler;
			{
				pipe(task->hStdIn.pair);
				task->hStdIn.handle = task->hStdIn.pair[1];

				pipe(task->hStdOut.pair);
				task->hStdOut.handle = task->hStdOut.pair[0];
			}

			// always have to thread to taskend so waitpid can clean zombies.
			ThreadTo( WaitForTaskEnd, (PTRSZVAL)task );
			if( path )
			{
				GetCurrentPath( saved_path, sizeof( saved_path ) );
				SetCurrentPath( path );
			}
			if( !( newpid = fork() ) )
			{
				// in case exec fails, we need to
				// drop any registered exit procs...
				//close( task->hStdIn.pair[1] );
				//close( task->hStdOut.pair[0] );
				//close( task->hStdErr.pair[0] );

				dup2( task->hStdIn.pair[0], 0 );
				dup2( task->hStdOut.pair[1], 1 );
				dup2( task->hStdOut.pair[1], 2 );

				DispelDeadstart();

				//usleep( 100000 );
				execve( program, (char *const*)args, environ );
				lprintf( WIDE( "Direct execute failed... trying along path..." ) );
				{
					char *tmp = StrDup( getenv( WIDE( "PATH" ) ) );
					char *tok;
					for( tok = strtok( tmp, WIDE( ":" ) ); tok; tok = strtok( NULL, WIDE( ":" ) ) )
					{
						char fullname[256];
						snprintf( fullname, sizeof( fullname ), WIDE( "%s/%s" ), tok, program );

						lprintf( WIDE( "program:[%s]" ), fullname );
						((char**)args)[0] = fullname;
						execve( fullname, (char*const*)args, environ );
					}
					Release( tmp );
				}
				close( task->hStdIn.pair[0] );
				close( task->hStdOut.pair[1] );
				//close( task->hWriteErr );
				close( 0 );
				close( 1 );
				close( 2 );
				lprintf( WIDE( "exec failed - and this is ALLL bad... %d" ), errno );
				//DebugBreak();
				// well as long as this runs before
				// the other all will be well...
				task = NULL;
				// shit - what can I do now?!
				exit(0); // just in case exec fails... need to fault this.
			}
			else
			{
				close( task->hStdIn.pair[0] );
				close( task->hStdOut.pair[1] );
			}
			ThreadTo( HandleTaskOutput, (PTRSZVAL)task );
			task->pid = newpid;
			lprintf( WIDE("Forked, and set the pid..") );
			// how can I know if the command failed?
			// well I can't - but the user's callback will be invoked
			// when the above exits.
			if( path )
			{
				// if path is NULL we didn't change the path...
				SetCurrentPath( saved_path );
			}
			return task;
		}
#endif
	}
	return FALSE;
}

SYSTEM_PROC( PTASK_INFO, LaunchPeerProgramEx )( CTEXTSTR program, CTEXTSTR path, PCTEXTSTR args
															 , TaskOutput OutputHandler
															 , TaskEnd EndNotice
															 , PTRSZVAL psv
                                                DBG_PASS
															  )
{
   return LaunchPeerProgramExx( program, path, args, LPP_OPTION_DO_NOT_HIDE, OutputHandler, EndNotice, psv DBG_RELAY );
}

//------------- System() ---------- simplest form of launch process (with otuput handler, and pprintf support )

struct task_end_notice
{
	PTHREAD thread;
	LOGICAL ended;
	PTRSZVAL psv_output;
   TaskOutput output_handler;
};

static void CPROC SystemTaskEnd( PTRSZVAL psvUser, PTASK_INFO task )
{
	struct task_end_notice *end_data = (struct task_end_notice *)psvUser;
	end_data->ended = TRUE;
	WakeThread( end_data->thread );
}

static void CPROC SystemOutputHandler( PTRSZVAL psvUser, PTASK_INFO Task, CTEXTSTR buffer, size_t len )
{
	struct task_end_notice *end_data = (struct task_end_notice *)psvUser;
	end_data->output_handler( end_data->psv_output, Task, buffer, len );
}

ATEXIT( SystemAutoShutdownTasks )
{
	INDEX idx;
	PTASK_INFO task;
	LIST_FORALL( l.system_tasks, idx, PTASK_INFO, task )
	{
      TerminateProgram( task );
	}
}

SYSTEM_PROC( PTASK_INFO, SystemEx )( CTEXTSTR command_line
															  , TaskOutput OutputHandler
															  , PTRSZVAL psv
																DBG_PASS
											)
{
	TEXTCHAR *command_line_tmp = StrDup( command_line );
	struct task_end_notice end_notice;
	PTASK_INFO result;
	int argc;
	TEXTSTR *argv;
	end_notice.ended = FALSE;
	end_notice.thread = MakeThread();
	end_notice.psv_output = psv;
	end_notice.output_handler = OutputHandler;
	ParseIntoArgs( command_line_tmp, &argc, &argv );
	Release( command_line_tmp );
	result = LaunchPeerProgramExx( argv[0], NULL, (PCTEXTSTR)argv, 0, OutputHandler?SystemOutputHandler:NULL, SystemTaskEnd, (PTRSZVAL)&end_notice DBG_RELAY );
	if( result )
	{
		AddLink( &l.system_tasks, result );
		// we'll get woken when it ends, might as well be infinite.
		while( !end_notice.ended )
		{
			if( !Idle( ) )
				WakeableSleep( 10000 );
			else
				Relinquish();
		}
		DeleteLink( &l.system_tasks, result );
	}
	{
		POINTER tmp = (POINTER)argv;
		while( argv[0] )
		{
			Release( (POINTER)argv[0] );
			argv++;
		}
		Release( tmp );
	}
   return result;
}

//----------------------- Utility to send to launched task's stdin ----------------------------

int vpprintf( PTASK_INFO task, CTEXTSTR format, va_list args )
{
	PVARTEXT pvt = VarTextCreate();
	PTEXT output;
	vvtprintf( pvt, format, args );
	output = VarTextGet( pvt );
	if(
#ifdef _WIN32
		WaitForSingleObject( task->pi.hProcess, 0 )
#else
		task->pid != waitpid( task->pid, NULL, WNOHANG )
#endif
	  )
	{
#ifdef _WIN32
		DWORD dwWritten;
#endif
      //lprintf( "Allowing write to process pipe..." );
		{
			PTEXT seg = output;
			while( seg )
			{
#ifdef _WIN32
            //LogBinary( GetText( seg )
		      //			, GetTextSize( seg ) );
   				WriteFile( task->hStdIn.handle
      					, GetText( seg )
		      			, (DWORD)GetTextSize( seg )
      					, &dwWritten
      					, NULL );
#else
				{
					struct pollfd pfd = { task->hStdIn.handle, POLLHUP|POLLERR, 0 };
					if( poll( &pfd, 1, 0 ) &&
						 pfd.revents & POLLERR )
					{
						Log( WIDE( "Pipe has no readers..." ) );
							break;
					}
					LogBinary( (_8*)GetText( seg ), GetTextSize( seg ) );
					write( task->hStdIn.handle
						 , GetText( seg )
						 , GetTextSize( seg ) );
				}
#endif
				seg = NEXTLINE(seg);
			}
		}
		LineRelease( output );
	}
	else
	{
		lprintf( WIDE("Task has ended, write  aborted.") );
	}
	VarTextDestroy( &pvt );
	return 0;
}


int pprintf( PTASK_INFO task, CTEXTSTR format, ... )
{
	va_list args;
	va_start( args, format );
	return vpprintf( task, format, args );
}


SACK_SYSTEM_NAMESPACE_END


//-------------------------------------------------------------------------

