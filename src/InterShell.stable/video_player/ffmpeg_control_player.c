#include <stdhdrs.h>
#define USES_INTERSHELL_INTERFACE
#define USE_RENDER_INTERFACE l.pdi
#define USE_IMAGE_INTERFACE l.pii
#include <render.h>

#include <ffmpeg_interface.h>
#include "../widgets/include/banner.h"
#include "../intershell_registry.h"


#define VIDEO_PLAYER_MAIN_SOURCE
#include "video_player_local.h"


struct player_button
{
	PMENU_BUTTON button;
	CTEXTSTR url_name;
};

/*
static struct ffmpeg_button_local {
	struct ffmpeg_file *ffmpeg_serve;
	PLIST players;
} l;
*/

typedef struct ffmpeg_surface {
	PSI_CONTROL pc;
	struct ffmpeg_file *ffmpeg;
	LOGICAL playing; // is showing something.
} *PFFMPEG;

EasyRegisterControlWithBorder( WIDE("FFMPEG Surface"), sizeof( struct ffmpeg_surface ), BORDER_NONE );

void PlayOnPlayers( CTEXTSTR url_name )
{
	PFFMPEG ffmpeg;
	INDEX idx;
	if( !url_name )
		return;
	//LIST_FORALL( l.players, idx, PFFMPEG, ffmpeg )
	{
		if( ffmpeg->playing )
			//StopItemIn( ffmpeg->pc );
		lprintf( WIDE("PC is %p"), ffmpeg->pc );
		//PlayItemInEx( ffmpeg->pc, url_name, NULL  );
		ffmpeg->playing = TRUE;
	}
}

static void OnKeyPressEvent( WIDE("FFMPEG/button") )( PTRSZVAL psv )
{
	static int n;
	PMENU_BUTTON button = (PMENU_BUTTON)psv;
	PSI_CONTROL pc_button = InterShell_GetButtonControl( button );
	PRENDERER renderer = GetButtonAnimationLayer( pc_button );
	n++;
	if( n == 5 )
		n = 0;
}

static PTRSZVAL OnCreateMenuButton( WIDE("FFMPEG/button") )( PMENU_BUTTON button )
{

	return (PTRSZVAL)button;
}


static void OnKeyPressEvent( WIDE("FFMPEG/Player button") )( PTRSZVAL psv )
{
	struct player_button *play = (struct player_button *)psv;
	PlayOnPlayers( play->url_name );
}

static PTRSZVAL OnCreateMenuButton( WIDE("FFMPEG/Player button") )( PMENU_BUTTON button )
{
	struct player_button *play = New( struct player_button );
	play->button = button;
	play->url_name = NULL;
	InterShell_SetButtonStyle( button, WIDE("bicolor square") );
	InterShell_SetButtonText( button, WIDE("Play_Video") );
	return (PTRSZVAL)button;
}

static void OnKeyPressEvent( WIDE("FFMPEG/Stop Player button") )( PTRSZVAL psv )
{
	struct player_button *play = (struct player_button *)psv;
	PFFMPEG ffmpeg;
	INDEX idx;
	//LIST_FORALL( l.players, idx, PFFMPEG, ffmpeg )
	{
		if( ffmpeg->playing )
			StopItemIn( ffmpeg->pc );
		ffmpeg->playing = FALSE;
	}


}

static PTRSZVAL OnCreateMenuButton( WIDE("FFMPEG/Stop Player button") )( PMENU_BUTTON button )
{
	struct player_button *play = New( struct player_button );
	play->button = button;
	play->url_name = NULL;
	InterShell_SetButtonStyle( button, WIDE("bicolor square") );
	InterShell_SetButtonText( button, WIDE("Stop_Player") );
	return (PTRSZVAL)button;
}

static PTRSZVAL CPROC SetButtonMedia( PTRSZVAL psv, arg_list args )
{
	PARAM( args, CTEXTSTR, url );
	struct player_button *play = (struct player_button *)psv;
	if( play->url_name )
		Release( (POINTER)play->url_name );
	play->url_name = StrDup( url );
	return psv;
}

static void OnLoadControl( WIDE("FFMPEG/Player button") )( PCONFIG_HANDLER pch, PTRSZVAL psv )
{
	AddConfigurationMethod( pch, WIDE("Play Media:%m"), SetButtonMedia );
}

static void OnSaveControl( WIDE("FFMPEG/Player button") )( FILE *file, PTRSZVAL psv )
{
	struct player_button *play = (struct player_button *)psv;
	if( play->url_name )
		fprintf( file, WIDE("Play Media:%s\n"), play->url_name );
}



static PTRSZVAL OnCreateControl( WIDE("FFMPEG/Player") )( PSI_CONTROL parent, S_32 x, S_32 y, _32 w, _32 h )
{

	PSI_CONTROL pc = MakeNamedControl( parent, WIDE("FFMPEG Surface"), x, y, w, h, -1 );
	MyValidatedControlData( PFFMPEG, ffmpeg, pc );
	ffmpeg->pc = pc;
	AddLink( &l.players, ffmpeg );
	return (PTRSZVAL)ffmpeg;
}

static PSI_CONTROL OnGetControl( WIDE("FFMPEG/Player"))(PTRSZVAL psv )
{
	PFFMPEG ffmpeg = (PFFMPEG)psv;
	return ffmpeg->pc;
}

static void OnShowControl( WIDE("FFMPEG/Player") )(PTRSZVAL psv )
{
	PFFMPEG ffmpeg = (PFFMPEG)psv;
	//ffmpeg->ffmpeg = PlayItemInEx( ffmpeg->pc, WIDE("dshow://"), NULL );

}

static void OnHideControl( WIDE("FFMPEG/Player") )(PTRSZVAL psv )
{
	PFFMPEG ffmpeg = (PFFMPEG)psv;
	//StopItem(  ffmpeg->ffmpeg );

}

//------------------------------------------------------------------------------------
