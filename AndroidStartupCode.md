# Introduction #

Cleaner; more general code for android startup

[Original Source](https://code.google.com/p/c-system-abstraction-component-gui/source/browse/src/deadstart/android/default_android_main.c)


# Details #

```
char buf[256];
                FILE *maps = fopen( "/proc/self/maps", "rt" );
                while( maps && fgets( buf, 256, maps ) )
                {
                        unsigned long start;
                        unsigned long end;
                        sscanf( buf, "%lx", &start );
                        sscanf( buf+9, "%lx", &end );
                        if( ((unsigned long)BeginNormalProcess >= start ) && ((unsigned long)BeginNormalProcess <= end ) )
                        {
                                char *mypath;
                                void *lib;
            char *myext;
                                void (*InvokeDeadstart)(void );
                                void (*MarkRootDeadstartComplete)(void );

                                fclose( maps );
            maps = NULL;

                                if( strlen( buf ) > 49 )
                                mypath = strdup( buf + 49 );
                                myext = strrchr( mypath, '.' );
                                myname = strrchr( mypath, '/' );
                                if( myname )
                                {
                                        myname[0] = 0;
                                        myname++;
                                }
                                else
                                        myname = mypath;
                                if( myext )
                                {
                                        myext[0] = 0;
                                }
                                //LOGI( "my path [%s][%s]", mypath, myname );

                                LoadLibrary( mypath, "libmylib.so" );
                        }
                }
                myname = "Reading /proc/self/maps failed";
                if( maps )
                        fclose( maps );
        }
        if( !SACK_Main )
        {
                LOGI( "(still)Failed to get SACK_Main entry point; I am [%s]", myname );
                return 0;
        }
        SACK_Main( 0, NULL );
        engine.closed = 1;
        LOGI( "Main exited... and so should we all..." );
        return NULL;
}




```


```
void *LoadLibrary( char *path, char *name )
{
        char buf[256];
   int tries = 0;
        snprintf( buf, 256, "%s/%s", path, name );
        do
        {
      void *result;
      //LOGI( "Open [%s]", buf );
                if( !( result = dlopen( buf, 0 ) ) )
                {
                        const char *recurse = dlerror();
                        LOGI( "error: %s", recurse );
                        if( strstr( recurse, "could not load needed library" ) )
                        {
                                char *namestart = strchr( recurse, '\'' );
                                char *nameend = strchr( namestart+1, '\'' );
                                char tmpname[256];
                                snprintf( tmpname, 256, "%*.*s", (nameend-namestart)-1,(nameend-namestart)-1,namestart+1 );
            LOGI( "Result was [%s]", tmpname );
                                LoadLibrary( path, tmpname );
                        }
                        else
                        {
                                LOGI( "Some Other Eror:%s", recurse );
                                break;
                        }
                }
                else
         return result;
      tries++;
        }
   while( tries < 2 );
   return NULL;
}


```