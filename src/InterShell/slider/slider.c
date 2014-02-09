#define DEFINE_DEFAULT_IMAGE_INTERFACE
#define USES_INTERSHELL_INTERFACE
#define DEFINES_INTERSHELL_INTERFACE

#include "resource.h"

#include <controls.h>

#include "../intershell_export.h"
#include "../intershell_registry.h"

//---------------------------------------------------------------------------

enum {
	CHECKBOX_HORIZONTAL = 42000
	, CHECKBOX_DRAGGING
	, EDIT_BACKGROUND_IMAGE
	, EDIT_MIN 
	, EDIT_MAX 
	, EDIT_CURRENT 

};



typedef struct slider_info_tag
{
	struct {
		BIT_FIELD bHorizontal : 1; // vertical if not horizontal
		BIT_FIELD bDragging	: 1; // clicked and held...
	} flags;

	CDATA backcolor, color;

	SFTFont *font;

	_32 min, max, current;	

	/* this control may be destroyed and recreated based on other options */
	PSI_CONTROL control;
	 TEXTSTR image_name;
	Image image;

} SLIDER_INFO, *PSLIDER_INFO;


//---------------------------------------------------------------------------
static PTRSZVAL CPROC ReloadSliderColor( PTRSZVAL psv, arg_list args );
static PTRSZVAL CPROC ReloadSliderBackColor( PTRSZVAL psv, arg_list args );
static PTRSZVAL CPROC SetSliderHorizontal( PTRSZVAL psv, arg_list args );
static PTRSZVAL CPROC SetSliderDragging( PTRSZVAL psv, arg_list args );
static PTRSZVAL CPROC SetSliderBackgroundImage( PTRSZVAL psv, arg_list args );
static PTRSZVAL CPROC SetSliderMinValue( PTRSZVAL psv, arg_list args );
static PTRSZVAL CPROC SetSliderMaxValue( PTRSZVAL psv, arg_list args );
static PTRSZVAL CPROC SetSliderCurrentValue( PTRSZVAL psv, arg_list args );
static void CPROC OnSliderUpdateProc(PTRSZVAL, PSI_CONTROL, int val);


//---------------------------------------------------------------------------
PRELOAD( RegisterExtraSliderConfig )
{
	EasyRegisterResource( WIDE("InterShell/") _WIDE(TARGETNAME), CHECKBOX_HORIZONTAL, RADIO_BUTTON_NAME );
	EasyRegisterResource( WIDE("InterShell/") _WIDE(TARGETNAME), CHECKBOX_DRAGGING, RADIO_BUTTON_NAME );
	EasyRegisterResource( WIDE("InterShell/") _WIDE(TARGETNAME), EDIT_BACKGROUND_IMAGE, EDIT_FIELD_NAME );
	EasyRegisterResource( WIDE("InterShell/") _WIDE(TARGETNAME), EDIT_MIN, EDIT_FIELD_NAME );
	EasyRegisterResource( WIDE("InterShell/") _WIDE(TARGETNAME), EDIT_MAX, EDIT_FIELD_NAME );
	EasyRegisterResource( WIDE("InterShell/") _WIDE(TARGETNAME), EDIT_CURRENT, EDIT_FIELD_NAME );

}


static PTRSZVAL OnCreateControl(WIDE("Slider"))
/*PTRSZVAL CPROC CreateSlider*/( PSI_CONTROL frame, S_32 x, S_32 y, _32 w, _32 h )
{

	PSLIDER_INFO info = New( SLIDER_INFO );
	 
	
	MemSet( info, 0, sizeof( *info ) );

	 //Default values
	info->color = BASE_COLOR_WHITE;

	info->min = 0;
	info->max = 100;
	info->current = 10;


	info->control = MakeControl( frame
												, SLIDER_CONTROL
									  , x
									  , y
									  , w
									  , h
									  , -1
									  );


	SetSliderUpdateHandler( info->control, OnSliderUpdateProc, (PTRSZVAL)info );	//info (pointer) will be returned in OnSliderUpdateProc()
	 

	// none of these are accurate values, they are just default WHITE and nothing.
	InterShell_SetButtonColors( NULL, info->color, info->backcolor, 0, 0 );
////	SetSliderColor( info->control, info->color );
//	SetSliderBackColor( info->control, info->backcolor );
	

	// need to supply extra information about the image, location of hands and face in image
	// and the spots...
	//MakeClockAnalog( info->control );
	info->font = InterShell_GetCurrentButtonFont();
	if( info->font )
		SetCommonFont( info->control, (*info->font ) );

	//It seems that this magic line helps in showing slider
	MoveSizeCommon( info->control, x, y, w, h );

	// the result of this will be hidden...
	return (PTRSZVAL)info;
}




static PTRSZVAL OnConfigureControl( WIDE("Slider") )( PTRSZVAL psv, PSI_CONTROL parent_frame )
//PTRSZVAL CPROC ConfigureClock( PTRSZVAL psv )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	{
		PCOMMON frame = NULL; 
		int okay = 0;
		int done = 0;
		if( !frame )
		{
			frame = LoadXMLFrame( WIDE("Slider_Properties.Frame") );
			//frame = CreateFrame( WIDE("Clock Properties"), 0, 0, 420, 250, 0, NULL );
			if( frame )
			{
					 TEXTCHAR buffer[256];

				SetCommonButtonControls( frame );
				
				SetCheckState( GetControl( frame, CHECKBOX_HORIZONTAL ), info->flags.bHorizontal );
				SetCheckState( GetControl( frame, CHECKBOX_DRAGGING ), info->flags.bDragging );
				SetControlText( GetControl( frame, EDIT_BACKGROUND_IMAGE ), info->image_name );

					 snprintf(buffer, sizeof(buffer), WIDE("%d"), info->min);
				SetControlText( GetControl( frame, EDIT_MIN ), buffer );

					 snprintf(buffer, sizeof(buffer), WIDE("%d"), info->max);
				SetControlText( GetControl( frame, EDIT_MAX ), buffer );
				
					 snprintf(buffer, sizeof(buffer), WIDE("%d"), info->current);
				SetControlText( GetControl( frame, EDIT_CURRENT ), buffer );
				
				SetCommonButtons( frame, &done, &okay );
				DisplayFrameOver( frame, parent_frame );
				CommonWait( frame );

				if( okay )
				{
					GetCommonButtonControls( frame );
					info->font = InterShell_GetCurrentButtonFont();
					if( info->font )
						SetCommonFont( info->control, (*info->font ) );
					info->color = GetColorFromWell( GetControl( frame, CLR_TEXT_COLOR ) );
					info->backcolor = GetColorFromWell( GetControl( frame, CLR_BACKGROUND ) );
					{
						GetControlText( GetControl( frame, EDIT_BACKGROUND_IMAGE ), buffer, sizeof( buffer ) );
						if( info->image_name )
							Release( info->image_name );
						info->image_name = StrDup( buffer );
					}

//					SetSliderColor( info->control, info->color );
//					SetSliderkBackColor( info->control, info->backcolor );

				 info->flags.bHorizontal = GetCheckState( GetControl( frame, CHECKBOX_HORIZONTAL ) );
					 info->flags.bDragging = GetCheckState( GetControl( frame, CHECKBOX_DRAGGING ) );

					 //SetSliderHorizontal(info->control, info->flags.bHorizontal);
					 //SetSliderDragging(info->control, info->flags.bDragging);

				GetControlText( GetControl( frame, EDIT_MIN), buffer, sizeof( buffer ) );
				info->min = atoi(buffer);
				GetControlText( GetControl( frame, EDIT_MAX), buffer, sizeof( buffer ) );
				info->max = atoi(buffer);
				GetControlText( GetControl( frame, EDIT_CURRENT), buffer, sizeof( buffer ) );
				info->current = atoi(buffer);

					 //SetSliderMinValue( info->control, info->min);
					 //SetSliderMaxValue( info->control, info->max);
					 //SetSliderCurrentValue( info->control, info->current);

				}
				DestroyFrame( &frame );
			}
		}
	}
	return psv;
}


static void OnSaveControl( WIDE( "Slider" ) )( FILE *file,PTRSZVAL psv )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	fprintf( file, WIDE("Slider color=$%02X%02X%02X%02X\n")
			 , AlphaVal( info->color )
			 , RedVal( info->color )
			 , GreenVal( info->color )
			 , BlueVal( info->color )
			 );
	fprintf( file, WIDE("Slider back color=$%02X%02X%02X%02X\n")
			 , AlphaVal( info->backcolor )
			 , RedVal( info->backcolor )
			 , GreenVal( info->backcolor )
			 , BlueVal( info->backcolor )
			 );
	fprintf( file, WIDE("Slider background image=%s\n" ), info->image_name?info->image_name:WIDE("") );
	fprintf( file, WIDE("Slider is horizontal?%s\n"), info->flags.bHorizontal?WIDE("Yes"):WIDE("No") );
	fprintf( file, WIDE("Slider is draggable?%s\n"), info->flags.bDragging?WIDE("Yes"):WIDE("No") );
	fprintf( file, WIDE("Slider min value=%d\n"), info->min);
	fprintf( file, WIDE("Slider max value=%d\n"), info->max);
	fprintf( file, WIDE("Slider current value=%d\n"), info->current);


	InterShell_SaveCommonButtonParameters( file );

}


static PTRSZVAL CPROC ReloadSliderColor( PTRSZVAL psv, arg_list args )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	PARAM( args, CDATA, color );
	info->color = color;
	return psv;
}

static PTRSZVAL CPROC ReloadSliderBackColor( PTRSZVAL psv, arg_list args )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	PARAM( args, CDATA, color );
	info->backcolor = color;
	return psv;
}

static PTRSZVAL CPROC SetSliderHorizontal( PTRSZVAL psv, arg_list args )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	PARAM( args, LOGICAL, bHorizontal );
	info->flags.bHorizontal = bHorizontal;
	return psv;
}

static PTRSZVAL CPROC SetSliderDragging( PTRSZVAL psv, arg_list args )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	PARAM( args, LOGICAL, bDragging );
	info->flags.bDragging = bDragging;
	return psv;
}


static PTRSZVAL CPROC SetSliderBackgroundImage( PTRSZVAL psv, arg_list args )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	PARAM( args, CTEXTSTR, name );
	info->image_name = StrDup( name );

	return psv;
}

static PTRSZVAL CPROC SetSliderMinValue( PTRSZVAL psv, arg_list args )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	PARAM( args, S_32, min );
	info->min = min;
	return psv;
}

static PTRSZVAL CPROC SetSliderMaxValue( PTRSZVAL psv, arg_list args )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	PARAM( args, S_32, max );
	info->max = max;
	return psv;
}
static PTRSZVAL CPROC SetSliderCurrentValue( PTRSZVAL psv, arg_list args )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	PARAM( args, S_32, current );
	info->current = current;
	return psv;
}

static void OnLoadControl( WIDE( "Slider" ) )( PCONFIG_HANDLER pch, PTRSZVAL psv )
{
	AddConfigurationMethod( pch, WIDE("Slider color=%c"), ReloadSliderColor );
	AddConfigurationMethod( pch, WIDE("Slider back color=%c"), ReloadSliderBackColor );
	AddConfigurationMethod( pch, WIDE("Slider is horizontal?%b"), SetSliderHorizontal );
	AddConfigurationMethod( pch, WIDE("Slider is draggable?%b"), SetSliderDragging );
	AddConfigurationMethod( pch, WIDE("Slider background image=%m" ), SetSliderBackgroundImage );
	AddConfigurationMethod( pch, WIDE("Slider min value=%i"), SetSliderMinValue );
	AddConfigurationMethod( pch, WIDE("Slider max value=%i"), SetSliderMaxValue );
	AddConfigurationMethod( pch, WIDE("Slider current value=%i"), SetSliderCurrentValue );

}

static void OnFixupControl( WIDE("Slider") )(  PTRSZVAL psv )
{
	_32 opt;
	PSLIDER_INFO info = (PSLIDER_INFO)psv;

	if( info )
	{
//		SetSliderColor( info->control, info->color );
//		SetSliderBackColor( info->control, info->backcolor );
		 opt = info->flags.bHorizontal ? SLIDER_HORIZ : SLIDER_VERT;
		SetSliderValues( info->control, info->min, info->current, info->max );
		SetSliderOptions( info->control, opt );
		InterShell_SetButtonColors( NULL, info->color, info->backcolor, 0, 0 );
	}
}

static PSI_CONTROL OnGetControl( WIDE("Slider") )( PTRSZVAL psv )
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;
	return info->control;
}


static LOGICAL OnQueryShowControl( WIDE("Slider") )( PTRSZVAL psv )
{
	_32 opt;
	PSLIDER_INFO info = (PSLIDER_INFO)psv;

	if( info ) 
	{	
		if(info->min == 0 && info->max == 0 && info->current == 0)
		{
			info->min = 0;
			info->max = 100;
			info->current = 10;
		}

		 opt = info->flags.bHorizontal ? SLIDER_HORIZ : SLIDER_VERT;
		SetSliderValues( info->control, info->min, info->current, info->max );
		SetSliderOptions( info->control, opt );

	}

	return TRUE;
}

static void CPROC OnSliderUpdateProc(PTRSZVAL psv, PSI_CONTROL pc, int val)
//Update current value
{
	PSLIDER_INFO info = (PSLIDER_INFO)psv;

//	lprintf( "OnSliderUpdateProc: info=%X  val=%d", info, val);

	if(!info)
		return;
	
	info->current = val;
}

//---------------------------------------------------------------------------

#if ( __WATCOMC__ < 1291 )
PUBLIC( void, ExportThis )( void )
{
}
#endif
