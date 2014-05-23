#ifndef MY_DATAPATH_DEFINED
#define MY_DATAPATH_DEFINED

#ifndef CORECON_SOURCE
#if defined( SACK_BAG_EXPORTS ) || defined( PSI_CONSOLE_SOURCE )
#define CORECON_SOURCE
#endif
#endif

#ifdef BCC16
#ifdef CORECON_SOURCE
#define CORECON_PROC(type,name) type STDPROC _export name
#else
#define CORECON_PROC(type,name) type STDPROC name
#endif
#else
#if !defined(__STATIC__) && !defined(__UNIX__)
#ifdef CORECON_SOURCE
#define CORECON_NPROC(type,name) EXPORT_METHOD type name
#define CORECON_PROC(type,name) EXPORT_METHOD type CPROC name
// for defining variables.
#define CORECON_EXPORT(type,name) EXPORT_METHOD type name
#else
#define CORECON_NPROC(type,name) IMPORT_METHOD type name
#define CORECON_PROC(type,name) IMPORT_METHOD type CPROC name
// for defining variables.
#define CORECON_EXPORT(type,name) IMPORT_METHOD type name
#endif
#else
#ifdef CORECON_SOURCE
#define CORECON_PROC(type,name) type CPROC name
#define CORECON_NPROC(type,name) type name
#define CORECON_EXPORT(type,name) type name
#else
#define CORECON_PROC(type,name) extern type CPROC name
#define CORECON_NPROC(type,name) extern type name
#define CORECON_EXPORT(type,name) extern type name
#endif
#endif
#endif

#include <stdhdrs.h>
#include <image.h>
#include <render.h>
#include <controls.h>

#include <psi/console.h>


#ifdef DEKWARE_PLUGIN
#define PLUGIN_MODULE
#include "plugin.h"

#include "datapath.h"
#include "space.h"
#endif

#include "history.h"  // history_track

PSI_CONSOLE_NAMESPACE

#if !defined( WIN32 ) && !defined( _WIN32 )
typedef struct rect_tag {
   S_32 top,left,right,bottom;
} RECT;
#endif
//----------------------------------------------------------------------------
// unused STILL - but one day status bars on output may be useful!

typedef struct statfield_tag {
   PTEXT *pLine; // line to pass to macroduplicate to get the actual string
   int nLength; // length of this field - text will not exceed this

	struct statfield_tag **me;
	struct statfield_tag *pNext; // next to the right or next to the left...
} STATFIELD, *PSTATFIELD;

typedef struct statbar_tag {
   PSTATFIELD *pLeft, *pRight;
} STATBAR, *PSTATBAR;


//----------------------------------------------------------------------------

typedef struct keybind_tag { // overrides to default definitions
   struct {
        int bMacro:1;
        int bFunction:1;
      int bStroke:1;
   } flags;
	union {
      PTEXT stroke;
   } data;
} KEYBIND, *PKEYBIND;

//----------------------------------------------------------------------------
// virtual buffer?
// video buffer?
//typedef struct vbuffer_tag {
	//int nDisplayLines;
	// nLines>0
	// ? nLines - nDisplayLines == nHistoryLines
	// : nHistoryLines = nLines
	//int nHistoryPercent; // 0 = 25, 1 = 50, 2 = 75, 3 = 100
	//int nCursorX; // current offset on current line
	//int nCursorY; // line offset from last line ... ( -1-> - lines)
   //int tabsize;   // multiple size of tabs....
//} VBUFFER, *PVBUFFER;

enum current_color_type
{
	COLOR_COMMAND
, COLOR_MARK
, COLOR_DEFAULT
, COLOR_SEGMENT
};

enum fill_color_type
{
	FILL_COMMAND_BACK
, FILL_DISPLAY_BACK
};

struct history_tracking_info
{
	// these are within the history cursor...
	// and this is the reason I need another cursor for
	// history and for display...
	// perhaps I could just always have two versions of browsing
	// browse_end, browse_current ?
	PHISTORY_REGION pHistory;
	// region history is browsed here,
	// this cursor is controlled for the top/bottom of form
	// control
	PHISTORY_BROWSER pHistoryDisplay;
	// output is performed here.
	// view is always built from tail backward.(?)
	PHISTORY_BROWSER pCurrentDisplay;
	// history, but on the outbound side?
	//
	// cursor as output has a cursor position
	// and a seperate cursor position for browsing...
	// I should probably seperate the data, but they
	// share the same width/height...
	PHISTORY_LINE_CURSOR pCursor;

	_32 pending_spaces;
   _32 pending_tabs;
   
};

//----------------------------------------------------------------------------

typedef struct myconsolestruc {
   // these would otherwise exist within the common datapath structure...
   PUSER_INPUT_BUFFER CommandInfo;

	// physical width and height, (1:1 in console modes)
	_32 nWidth;   // in pixels
	_32 nColumns; // in character count width
	_32 nHeight;  // in pixels
	_32 nLines;   // in character count rows

	CRITICALSECTION Lock;

	struct {
		BIT_FIELD bDirect:1; // alternative to direct is Line-Mode
		BIT_FIELD bLastEnqueCommand:1; // set if the last thing output was the command.
		BIT_FIELD bUpdatingEnd : 1;
		BIT_FIELD bMarking : 1;
		BIT_FIELD bMarkingBlock : 1;
		BIT_FIELD bNoDisplay : 1;
		BIT_FIELD bNoHistoryRender : 1;
		BIT_FIELD bForceNewline : 1;
		// character mode input, instead of line buffered
		// just because it's in direct mode doesn't mean it
		// has to be direct send also...  But CharMode is only
		// available if mode is also direct.
		BIT_FIELD bCharMode : 1;
		BIT_FIELD bNoLocalEcho : 1;
		BIT_FIELD bHistoryShow : 1;
		BIT_FIELD bNewLine : 1; // set if the next line is NEW else it's to be appended.
		BIT_FIELD bBuildDataWithCarriageReturn : 1;
	} flags;

   // these are working parameters during output...
	_32 pending_spaces;
   _32 pending_tabs;

	RECT rArea; // pixel size of the display (if font height/width>1)
	_32 nFontHeight;
	_32 nFontWidth;
	S_32 nXPad; // pixels/lines to padd left/right side...
	S_32 nYPad; // pixels/lines to padd top/bottom side...
	S_32 nCmdLinePad; // pixels to raise bar above cmdline

	//PHISTORY_BIOS pHistory;
	int nHistoryPercent;
	// these mark the bottom line, from these UP
	// are the regions... therefore if start = 0
	// the first line to show is above the display and
	// therefore that region has no information to show.
	int nHistoryLineStart;
	int nDisplayLineStart; // top visual line of those in 'display' (start of separator)
	int nNextCharacterBegin; // this is computed from the last position of the last renderd line ( continue for command line)
	int nCommandLineStart; // marks the top of the separator line... bottom of text

	// these are within the history cursor...
	// and this is the reason I need another cursor for
	// history and for display...
	// perhaps I could just always have two versions of browsing
	// browse_end, browse_current ?
	PHISTORY_REGION pHistory;
	// region history is browsed here,
	// this cursor is controlled for the top/bottom of form
	// control
	PHISTORY_BROWSER pHistoryDisplay;
	// output is performed here.
	// view is always built from tail backward.(?)
	PHISTORY_BROWSER pCurrentDisplay;
	// history, but on the outbound side?
	//
	// cursor as output has a cursor position
	// and a seperate cursor position for browsing...
	// I should probably seperate the data, but they
	// share the same width/height...
	PHISTORY_LINE_CURSOR pCursor;
   
	PDATALIST *CurrentLineInfo;

	KEYBIND Keyboard[256][8];
	// is actually current keymod state.
	_32 dwControlKeyState;

	int mark_location;
	// 0 = command, 1 = display, 2 = history
	//, 3 = header?, 4 = footer?
	// display list this is a mark in.
	PDATALIST *CurrentMarkInfo;
	struct {
		INDEX row, col;
	} mark_start;
	struct {
		INDEX row, col;
	} mark_end;
	// something like a specialized footer here... probably a
	// PSTATBAR MenuBar; might fit in well also...
	//PSTATBAR StatusBar; // if present - then all lines must recompute!
	// ----------------
	// Rendering Methods for console_core library external methods...

	void (CPROC *FillConsoleRect)( struct myconsolestruc *pmdp, RECT *r, enum fill_color_type );
	void (CPROC *DrawString)( struct myconsolestruc *pmdp, int x, int y, RECT *r, CTEXTSTR s, int nShown, int nShow );
	void (CPROC *SetCurrentColor )( struct myconsolestruc *pmdp, enum current_color_type, PTEXT segment );
	void (CPROC *RenderSeparator )( struct myconsolestruc *pmdp, int pos );
	void (CPROC *KeystrokePaste )( struct myconsolestruc *pmdp );
	void (CPROC *RenderCursor )( struct myconsolestruc *pmdp, RECT *r, int column );
	void (CPROC *Update )( struct myconsolestruc *pmdp, RECT *upd );
   // void CPROC
   PLIST data_processors;
	union {
		// this is what this union has if nothing else defined
		// winlogic should need no member herein....
		_32 dwInterfaceData[32];

		struct
		{
			PRENDERER renderer;
			PCOMMON frame;
			SFTFont hFont;
			Image image;
			CDATA  crCommand;
			CDATA  crCommandBackground;
			CDATA  crBackground;
			CDATA  crMark;
			CDATA  crMarkBackground;
			// current working parameters...
			CDATA crText;
         CDATA crBack;
		} psicon;
	};

} CONSOLE_INFO, *PCONSOLE_INFO;

#include "keydefs.h"

PSI_CONSOLE_NAMESPACE_END

#endif
