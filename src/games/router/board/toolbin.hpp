

#include "board.hpp"

#if !defined(__STATIC__) && !defined( __UNIX__ )
#ifdef TOOLBIN_SOURCE
#define TOOLBIN_PROC(type,name) EXPORT_METHOD type CPROC name
#else
#define TOOLBIN_PROC(type,name) IMPORT_METHOD type CPROC name
#endif
#else
#ifdef TOOLBIN_SOURCE
#define TOOLBIN_PROC(type,name) type CPROC name
#else
#define TOOLBIN_PROC(type,name) extern type CPROC name
#endif
#endif

typedef class TOOLBIN *PTOOLBIN;


extern "C"
{
	TOOLBIN_PROC( PTOOLBIN, CreateToolbin )( PIBOARD board );
	TOOLBIN_PROC( PSI_CONTROL, CreateToolbinControl )( PIBOARD board, PSI_CONTROL parent, S_32 x, S_32 y, _32 w, _32 h );
   TOOLBIN_PROC( PTOOLBIN, GetToolbinFromControl )( PSI_CONTROL control );

};


