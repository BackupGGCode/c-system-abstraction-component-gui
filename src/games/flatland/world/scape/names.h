#ifndef NAMES_DEFINED
#define NAMES_DEFINED

#include <worldstrucs.h>

//PNAME GetName( PNAMESET *ppNames, char *text );
//void SetName( PNAME name, char *text );

//void DeleteName( INDEX iWorld, INDEX iName );
//WORLD_PROC( PTRSZVAL, DeleteName )( PNAME name, PNAMESET set );
//void DeleteNames( PNAMESET *ppNames );

//void GetNameText( char *text, PNAME name );

INDEX SrvrMakeName( _32 client_id, INDEX iWorld, CTEXTSTR text );

#endif
