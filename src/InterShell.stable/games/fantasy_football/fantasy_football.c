
#include <stdhdrs.h>
#define DEFINES_INTERSHELL_INTERFACE
#define USES_INTERSHELL_INTERFACE
#define USE_RENDER_INTERFACE ffl.pdi
#define USE_IMAGE_INTERFACE ffl.pii
#include <render.h>
#include <salty_generator.h>
#include <ffmpeg_interface.h>
#include "../../widgets/include/banner.h"
#include "../../intershell_registry.h"
#include "../../intershell_export.h"

RegisterControlWithBorderEx( WIDE("FF_Attract"), sizeof( struct attract_control *), BORDER_NONE, FF_Attract );
RegisterControlWithBorderEx( WIDE("FF_Grid"), sizeof( struct game_grid_control *), BORDER_NONE, FF_Grid );
RegisterControlWithBorderEx( WIDE("FF_Grid_Cell"), sizeof( struct game_cell_control *), BORDER_NONE, FF_GridCell );
RegisterControlWithBorderEx( WIDE("FF_Scoreboard"), sizeof( struct attract_control *), BORDER_NONE, FF_Scoreboard );

struct game_grid_control
{
	PSI_CONTROL pc;
	Image background;
	Image foreground;
};

struct game_cell_control
{
	LOGICAL playing;
	PSI_CONTROL pc;
	Image sticker;
	Image Prize;
	struct ffmpeg_file *file;
	//S_32 cx, cy;
	//_32 cw, ch;
	S_32 bx, by;
	_32 bw, bh;
	S_32 fx, fy;
	_32 fw, fh;
};

static struct fantasy_football_local
{
	CTEXTSTR attract;
	CTEXTSTR downs[4];
	CTEXTSTR helmets[32];
	CTEXTSTR helmet_sticker_name[32];
	struct 
	{
		struct game_grid_control control;
		struct game_cell_control cell[32];
		CTEXTSTR background;
		CTEXTSTR foreground;
		FRACTION offset_x, offset_y;
		FRACTION cell_width, cell_height;
	}grid;
	PLIST prizes;
	TEXTCHAR card_end_char;
	TEXTCHAR card_begin_char;
	_64 enable_code;

	PIMAGE_INTERFACE pii;
	PRENDER_INTERFACE pdi;

} ffl;

struct attract_control
{
	LOGICAL playing;
	PSI_CONTROL pc;
	struct ffmpeg_file *file;
};

PRELOAD( InitInterfaces )
{
	ffl.pii = GetImageInterface();
	ffl.pdi = GetDisplayInterface();
}

static void AddRules( PCONFIG_HANDLER pch );

static PTRSZVAL CPROC ProcessConfig( PTRSZVAL psv, arg_list args )
{
	PARAM( args, size_t, length );
	PARAM( args, CPOINTER, data );
	P_8 realdata;
	size_t reallength;
	PCONFIG_HANDLER pch = CreateConfigurationHandler();
	AddRules( pch );
	SRG_DecryptRawData( (P_8)data, length, &realdata, &reallength );
	ProcessConfigurationInput( pch, (CTEXTSTR)realdata, reallength, 0 );
	DestroyConfigurationEvaluator( pch );
	return psv;
}

static PTRSZVAL CPROC SetStartCharacter( PTRSZVAL psv, arg_list args )
{
	PARAM( args, char*, data );
	ffl.card_begin_char = data[0];
	return psv;
}

static PTRSZVAL CPROC SetEndCharacter( PTRSZVAL psv, arg_list args )
{
	PARAM( args, char*, data );
	ffl.card_end_char = data[0];
	return psv;
}

static PTRSZVAL CPROC SetAttract( PTRSZVAL psv, arg_list args )
{
	PARAM( args, CTEXTSTR, data );
	ffl.attract = StrDup( data );
	return psv;
}

static PTRSZVAL CPROC SetGameForeground( PTRSZVAL psv, arg_list args )
{
	PARAM( args, CTEXTSTR, data );
	ffl.grid.foreground = StrDup( data );
	return psv;
}

static PTRSZVAL CPROC SetGameBackground( PTRSZVAL psv, arg_list args )
{
	PARAM( args, CTEXTSTR, data );
	ffl.grid.background = StrDup( data );
	return psv;
}

static PTRSZVAL CPROC SetGameOffset( PTRSZVAL psv, arg_list args )
{
	PARAM( args, FRACTION, x );
	PARAM( args, FRACTION, y );
	ffl.grid.offset_x = (x);
	ffl.grid.offset_y = (y);
	return psv;
}

static PTRSZVAL CPROC SetGameSize( PTRSZVAL psv, arg_list args )
{
	PARAM( args, FRACTION, x );
	PARAM( args, FRACTION, y );
	ffl.grid.cell_width = (x);
	ffl.grid.cell_height = (y);
	return psv;
}

static PTRSZVAL CPROC SetDownIntro( PTRSZVAL psv, arg_list args )
{
	PARAM( args, S_64, down );
	PARAM( args, CTEXTSTR, data );
	ffl.downs[down-1] = StrDup( data );
	return psv;
}
static PTRSZVAL CPROC SetTeamHelmet( PTRSZVAL psv, arg_list args )
{
	PARAM( args, S_64, down );
	PARAM( args, CTEXTSTR, data );
	ffl.helmets[down-1] = StrDup( data );
	return psv;
}

static PTRSZVAL CPROC SetTeamHelmetSticker( PTRSZVAL psv, arg_list args )
{
	PARAM( args, S_64, down );
	PARAM( args, CTEXTSTR, data );
	ffl.helmet_sticker_name[down-1] = StrDup( data );
	return psv;
}

static PTRSZVAL CPROC SetSwipeEnable( PTRSZVAL psv, arg_list args )
{
	PARAM( args, S_64, data );
	ffl.enable_code = data;
	return psv;
}

static void AddRules( PCONFIG_HANDLER pch )
{
	AddConfigurationMethod( pch, WIDE("config=%B"), ProcessConfig );
	AddConfigurationMethod( pch, WIDE("Attract=%m"), SetAttract );
	AddConfigurationMethod( pch, WIDE("Down %i=%m"), SetDownIntro );
	AddConfigurationMethod( pch, WIDE("Grid Background=%m"), SetGameBackground );
	AddConfigurationMethod( pch, WIDE("Grid Foreground=%m"), SetGameForeground );
	AddConfigurationMethod( pch, WIDE("Team %i Helmet=%m"), SetTeamHelmet );
	AddConfigurationMethod( pch, WIDE("Team %i Helmet Sticker=%m"), SetTeamHelmetSticker );
	AddConfigurationMethod( pch, WIDE("Grid offset XY=%q %q"), SetGameOffset );
	AddConfigurationMethod( pch, WIDE("Grid Cell Size=%q %q"), SetGameSize );

	AddConfigurationMethod( pch, WIDE("card swipe enable=%i"), SetSwipeEnable );
	AddConfigurationMethod( pch, WIDE("card start character=%w"), SetStartCharacter );
	AddConfigurationMethod( pch, WIDE("card end character=%w"), SetEndCharacter );
}

static void ReadConfigFile( CTEXTSTR filename )
{
	PCONFIG_HANDLER pch = CreateConfigurationHandler();
	AddRules( pch );
	ProcessConfigurationFile( pch, filename, 0 );
	DestroyConfigurationHandler( pch );
}


static void OnLoadCommon( WIDE( "Fantasay Football" ) )( PCONFIG_HANDLER pch )
{
	ReadConfigFile( "fantasy_football_game.config" );
}

static void RestartAttract( PTRSZVAL psv )
{
	struct attract_control *ac = (struct attract_control *)psv;
	if( ac->file )
	{
		ffmpeg_SeekFile( ac->file, 0 );
		ffmpeg_PlayFile( ac->file );
	}
}

// helmet animation ended
static void EndGridCell( PTRSZVAL psv )
{
	struct game_cell_control *gcc = (struct game_cell_control *)psv;
	SmudgeCommon( gcc->pc ); // one more draw to make sure movie part is cleared
}

static int OnCreateCommon( WIDE( "FF_Attract" ) )( PSI_CONTROL pc )
{
	return 1;
}

static void OnHideCommon( WIDE("FF_Attract") )( PSI_CONTROL pc )
{
	ValidatedControlData( struct attract_control **, FF_Attract.TypeID, ppac, pc );
	struct attract_control *ac = (*ppac);
	ac->playing = FALSE;
	ffmpeg_PauseFile( ac->file );
}

static void OneShotDelay( PTRSZVAL psv )
{
	RestartAttract( psv );
}

static void OnRevealCommon( WIDE("FF_Attract") )( PSI_CONTROL pc )
{
	ValidatedControlData( struct attract_control **, FF_Attract.TypeID, ppac, pc );
	struct attract_control *ac = (*ppac);
	if( !ac->playing )
	{
		ac->playing = TRUE;
	//	AddTimerEx( 250, 0, OneShotDelay, (PTRSZVAL)ac );
	}
	RestartAttract( (PTRSZVAL)ac );
}


static int OnCreateCommon( WIDE( "FF_Grid" ) )( PSI_CONTROL pc )
{
	return 1;
}

static int OnDrawCommon( WIDE( "FF_Grid" ) )( PSI_CONTROL pc )
{
	Image surface = GetControlSurface( pc );
	BlotScaledImage( surface, ffl.grid.control.background );
	BlotScaledImageAlpha( surface, ffl.grid.control.foreground, ALPHA_TRANSPARENT );
	return 1;
}

static int OnCreateCommon( WIDE( "FF_Grid_Cell" ) )( PSI_CONTROL pc )
{
	return 1;
}

static void OnHideCommon( WIDE("FF_Grid_Cell") )( PSI_CONTROL pc )
{
	ValidatedControlData( struct game_cell_control **, FF_GridCell.TypeID, ppgcc, pc );
	struct game_cell_control *gcc = (*ppgcc);
	if( gcc->playing )
	{
		gcc->playing = FALSE;
		ffmpeg_PauseFile( gcc->file );
	}
}

static void DrawCellBackground( PSI_CONTROL pc )
{
	ValidatedControlData( struct game_cell_control **, FF_GridCell.TypeID, ppgcc, pc );
	Image surface = GetControlSurface( pc );
	BlotScaledImageSized( surface, ffl.grid.control.background
		, 0, 0
		, surface->width, surface->height 
		, ppgcc[0]->bx, ppgcc[0]->by
		, ppgcc[0]->bw, ppgcc[0]->bh );
}

static void DrawCellForeground( PSI_CONTROL pc )
{
	ValidatedControlData( struct game_cell_control **, FF_GridCell.TypeID, ppgcc, pc );
	Image surface = GetControlSurface( pc );
	BlotScaledImageSizedEx( surface, ffl.grid.control.foreground
		, 0, 0
		, surface->width, surface->height 
		, ppgcc[0]->fx, ppgcc[0]->fy
		, ppgcc[0]->fw, ppgcc[0]->fh, ALPHA_TRANSPARENT, BLOT_COPY );
}

static void DrawCellPrize( PSI_CONTROL pc )
{
	ValidatedControlData( struct game_cell_control **, FF_GridCell.TypeID, ppgcc, pc );
	Image surface = GetControlSurface( pc );
	BlotScaledImageSizedEx( surface, ffl.grid.control.foreground
		, 0, 0
		, surface->width, surface->height 
		, ppgcc[0]->fx, ppgcc[0]->fy
		, ppgcc[0]->fw, ppgcc[0]->fh, ALPHA_TRANSPARENT, BLOT_COPY );
}

static int OnDrawCommon( WIDE( "FF_Grid_Cell" ) )( PSI_CONTROL pc )
{
	ValidatedControlData( struct game_cell_control **, FF_GridCell.TypeID, ppgcc, pc );
	if( ppgcc[0] )
	{
		DrawCellBackground( pc );
		if( ppgcc[0]->Prize )
		{
			Image surface = GetControlSurface( pc );
			BlotScaledImageAlpha( surface, ppgcc[0]->Prize, ALPHA_TRANSPARENT );
		}
		else
		{
			Image surface = GetControlSurface( pc );
			BlotScaledImageAlpha( surface, ppgcc[0]->sticker, ALPHA_TRANSPARENT );
		}
		DrawCellForeground( pc );
	}
	return 1;
}


static int OnCreateCommon( WIDE( "FF_Scoreboard" ) )( PSI_CONTROL pc )
{
	return 1;
}

static void OnHideCommon( WIDE("FF_Scoreboard") )( PSI_CONTROL pc )
{
	ValidatedControlData( struct attract_control **, FF_Attract.TypeID, ppac, pc );
	struct attract_control *ac = (*ppac);
	ac->playing = FALSE;
	ffmpeg_PauseFile( ac->file );
}

static void NextPage( PTRSZVAL psv )
{
	struct attract_control *ac = (struct attract_control *)psv;
	ShellSetCurrentPage( GetCommonParent( ac->pc ), "next" );
}



static PTRSZVAL OnCreateControl(WIDE( "Fantasy Football/Attract" ))(PSI_CONTROL parent,S_32 x,S_32 y,_32 w,_32 h)
{
	struct attract_control *ac;
	ac = New( struct attract_control );

	ac->playing = FALSE;
	ac->pc = MakeNamedControl( parent, WIDE("FF_Attract"), x, y, w, h, 0 );
	{
		ValidatedControlData( struct attract_control **, FF_Attract.TypeID, ppac, ac->pc );
		(*ppac) = ac;
	}
	ac->file = ffmpeg_LoadFile( ffl.attract
								, NULL, 0
								, ac->pc
								, NULL, 0
								, RestartAttract, (PTRSZVAL)ac
								, NULL );
	return (PTRSZVAL)ac;
}

static PSI_CONTROL OnGetControl( WIDE("Fantasy Football/Attract") )( PTRSZVAL psv )
{
	struct attract_control *ac = (struct attract_control *)psv;
	return ac->pc;
}

static PTRSZVAL OnCreateControl(WIDE( "Fantasy Football/Game Grid" ))(PSI_CONTROL parent,S_32 x,S_32 y,_32 w,_32 h)
{
	struct game_grid_control *ac;
	ac = &ffl.grid.control;

	ac->pc = MakeNamedControl( parent, WIDE("FF_Grid"), x, y, w, h, 0 );
	ac->background = LoadImageFile( ffl.grid.background );
	ac->foreground = LoadImageFile( ffl.grid.foreground );
	{
		ValidatedControlData( struct game_grid_control **, FF_Grid.TypeID, ppac, ac->pc );
		(*ppac) = ac;
	}
	{
		int r, c;
		for( r = 0; r < 4; r++ )
			for( c = 0; c < 8; c++ )
			{
				FRACTION tmp1;
				FRACTION tmp2;
				FRACTION tmp3;
				int cx, cy, cw, ch;

				tmp1 = ffl.grid.offset_x;
				AddFractions( &tmp1, ScaleFraction( &tmp3, c, &ffl.grid.cell_width ) );
				tmp2 = ffl.grid.offset_x;
				AddFractions( &tmp2, ScaleFraction( &tmp3, c+1, &ffl.grid.cell_width ) );
				SubtractFractions( &tmp2, &tmp1 );

				ffl.grid.cell[r*8+c].bx = ScaleValue( &tmp1, ac->background->width );
				ffl.grid.cell[r*8+c].fx = ScaleValue( &tmp1, ac->foreground->width );
				cx = ScaleValue( &tmp1, w ); 
				ffl.grid.cell[r*8+c].bw = ScaleValue( &tmp2, ac->background->width );
				ffl.grid.cell[r*8+c].fw = ScaleValue( &tmp2, ac->foreground->width );
				cw = ScaleValue( &tmp2, w );

				tmp1 = ffl.grid.offset_y;
				AddFractions( &tmp1, ScaleFraction( &tmp2, r, &ffl.grid.cell_height ) );
				tmp2 = ffl.grid.offset_y;
				AddFractions( &tmp2, ScaleFraction( &tmp3, r+1, &ffl.grid.cell_height ) );
				SubtractFractions( &tmp2, &tmp1 );

				ffl.grid.cell[r*8+c].by = ScaleValue( &tmp1, ac->background->height );
				ffl.grid.cell[r*8+c].fy = ScaleValue( &tmp1, ac->foreground->height );
				cy = ScaleValue( &tmp1, h ); 
				ffl.grid.cell[r*8+c].bh = ScaleValue( &tmp2, ac->background->height );
				ffl.grid.cell[r*8+c].fh = ScaleValue( &tmp2, ac->foreground->height );
				ch = ScaleValue( &tmp2, h );
				lprintf( "cell %d is %d,%d %dx%d   %d,%d  %dx%d   %d,%d  %dx%d"
					, r * 8 + c
					, cx, cy, cw, ch
					, ffl.grid.cell[r*8+c].bx
					, ffl.grid.cell[r*8+c].by
					, ffl.grid.cell[r*8+c].bw
					, ffl.grid.cell[r*8+c].bh
					, ffl.grid.cell[r*8+c].fx
					, ffl.grid.cell[r*8+c].fy
					, ffl.grid.cell[r*8+c].fw
					, ffl.grid.cell[r*8+c].fh );
				ffl.grid.cell[r*8+c].pc = MakeNamedControl( ac->pc, WIDE("FF_Grid_Cell"), cx, cy, cw, ch, 0 );
				{
					ValidatedControlData( struct game_cell_control **, FF_GridCell.TypeID, ppgcc, ffl.grid.cell[r*8+c].pc );
					(*ppgcc) = ffl.grid.cell + (r*8+c);
				}
				ffl.grid.cell[r*8+c].sticker = LoadImageFile( ffl.helmet_sticker_name[r*8+c] );

				ffl.grid.cell[r*8+c].file = ffmpeg_LoadFile( ffl.helmets[r*8+c]
								, NULL, 0
								, ffl.grid.cell[r*8+c].pc
								, NULL, 0
								, EndGridCell, (PTRSZVAL)&ffl.grid.cell[r*8+c]
								, NULL );

			}
	}
	return (PTRSZVAL)ac;
}

static PSI_CONTROL OnGetControl( WIDE("Fantasy Football/Game Grid") )( PTRSZVAL psv )
{
	struct game_grid_control *ggc = (struct game_grid_control *)psv;
	return ggc->pc;
}


static PTRSZVAL OnCreateControl(WIDE( "Fantasy Football/1st Down" ))(PSI_CONTROL parent,S_32 x,S_32 y,_32 w,_32 h)
{
	struct attract_control *ac;
	ac = New( struct attract_control );

	ac->playing = FALSE;
	ac->pc = MakeNamedControl( parent, WIDE("FF_Attract"), x, y, w, h, 0 );
	{
		ValidatedControlData( struct attract_control **, FF_Attract.TypeID, ppac, ac->pc );
		(*ppac) = ac;
	}
	ac->file = ffmpeg_LoadFile( ffl.downs[0]
								, NULL, 0
								, ac->pc
								, NULL, 0
								, NextPage, (PTRSZVAL)ac
								, NULL );
	return (PTRSZVAL)ac;
}

static PSI_CONTROL OnGetControl( WIDE("Fantasy Football/1st Down") )( PTRSZVAL psv )
{
	struct attract_control *ac = (struct attract_control *)psv;
	return ac->pc;
}

static PTRSZVAL OnCreateControl(WIDE( "Fantasy Football/2nd Down" ))(PSI_CONTROL parent,S_32 x,S_32 y,_32 w,_32 h)
{
	struct attract_control *ac;
	ac = New( struct attract_control );

	ac->playing = FALSE;
	ac->pc = MakeNamedControl( parent, WIDE("FF_Attract"), x, y, w, h, 0 );
	{
		ValidatedControlData( struct attract_control **, FF_Attract.TypeID, ppac, ac->pc );
		(*ppac) = ac;
	}
	ac->file = ffmpeg_LoadFile( ffl.downs[1]
								, NULL, 0
								, ac->pc
								, NULL, 0
								, NextPage, (PTRSZVAL)ac
								, NULL );
	return (PTRSZVAL)ac;
}

static PSI_CONTROL OnGetControl( WIDE("Fantasy Football/2nd Down") )( PTRSZVAL psv )
{
	struct attract_control *ac = (struct attract_control *)psv;
	return ac->pc;
}

static PTRSZVAL OnCreateControl(WIDE( "Fantasy Football/3rd Down" ))(PSI_CONTROL parent,S_32 x,S_32 y,_32 w,_32 h)
{
	struct attract_control *ac;
	ac = New( struct attract_control );

	ac->playing = FALSE;
	ac->pc = MakeNamedControl( parent, WIDE("FF_Attract"), x, y, w, h, 0 );
	{
		ValidatedControlData( struct attract_control **, FF_Attract.TypeID, ppac, ac->pc );
		(*ppac) = ac;
	}
	ac->file = ffmpeg_LoadFile( ffl.downs[2]
								, NULL, 0
								, ac->pc
								, NULL, 0
								, NextPage, (PTRSZVAL)ac
								, NULL );
	return (PTRSZVAL)ac;
}

static PSI_CONTROL OnGetControl( WIDE("Fantasy Football/3rd Down") )( PTRSZVAL psv )
{
	struct attract_control *ac = (struct attract_control *)psv;
	return ac->pc;
}

static PTRSZVAL OnCreateControl(WIDE( "Fantasy Football/4th Down" ))(PSI_CONTROL parent,S_32 x,S_32 y,_32 w,_32 h)
{
	struct attract_control *ac;
	ac = New( struct attract_control );

	ac->playing = FALSE;
	ac->pc = MakeNamedControl( parent, WIDE("FF_Attract"), x, y, w, h, 0 );
	{
		ValidatedControlData( struct attract_control **, FF_Attract.TypeID, ppac, ac->pc );
		(*ppac) = ac;
	}
	ac->file = ffmpeg_LoadFile( ffl.downs[3]
								, NULL, 0
								, ac->pc
								, NULL, 0
								, NextPage, (PTRSZVAL)ac
								, NULL );
	return (PTRSZVAL)ac;
}

static PSI_CONTROL OnGetControl( WIDE("Fantasy Football/4th Down") )( PTRSZVAL psv )
{
	struct attract_control *ac = (struct attract_control *)psv;
	return ac->pc;
}

