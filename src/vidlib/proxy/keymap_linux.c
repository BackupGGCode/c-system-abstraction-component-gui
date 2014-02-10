
#include <stdhdrs.h>
#include <android/keycodes.h>

#include "local.h"

#include <psi/console.h>
// androi
// requires C99 to compile at this point!
RENDER_NAMESPACE


enum {
    KEY_FUNCTION_NOT_DEFINED = 0
	  , KEYDATA
     , KEY_COMMAND_SHIFT
 , KEYDATA_DEFINED // do this stroke?  
 , COMMANDKEY
 , HISTORYKEY
 , CONTROLKEY
 , SPECIALKEY
};  

typedef struct LinuxKeymapKeyDefine {
   CTEXTSTR name1;
   CTEXTSTR name2;
   int flags;
   struct {
      int bFunction;  // true if pStroke is pKeyFunc()
		CTEXTSTR pStroke; // this may be pKeyFunc()
   } op[8];
} PSIKEYDEFINE, *PPSIKEYDEFINE;

#define KEYDATA(a,b) {{ KEYDATA,  a}, {KEYDATA,b }}
#define KEYDATA8(a) {{ KEYDATA, a}, { KEYDATA, a}, { KEYDATA, a}, { KEYDATA, a}, { KEYDATA, a}, { KEYDATA, a}, { KEYDATA, a}, { KEYDATA, a} }
#define KEYSHIFT  { { KEY_COMMAND_SHIFT },{KEY_COMMAND_SHIFT},{KEY_COMMAND_SHIFT},{KEY_COMMAND_SHIFT}  \
                  ,{KEY_COMMAND_SHIFT},{KEY_COMMAND_SHIFT},{KEY_COMMAND_SHIFT},{KEY_COMMAND_SHIFT}}

//typedef int (CPROC *KeyFunc)( PCONSOLE_INFO pdp );
//typedef int (CPROC *KeyFuncUpDown)( PCONSOLE_INFO pdp, int bDown );

#define KDF_NODEFINE 0x01 // set to disallow key redefinition
//#define KDF_NUMKEY   0x04 // Key is sensitive to numlock state
#define KDF_NOKEY    0x08 // no keydef here...
#define KDF_NOREDEF  0x10
#define KDF_UPACTION 0x20 // action is called on key release

#define KEYMOD_NORMAL 0
#define KEYMOD_SHIFT  KEY_MOD_SHIFT
#define KEYMOD_CTRL   KEY_MOD_CTRL
#define KEYMOD_ALT    KEY_MOD_ALT



PSIKEYDEFINE LinuxKeyDefs[256] =
                     { [KEY_DEL]={WIDE("back"),WIDE("backspace"),0,KEYDATA("\b","\b") }
                      , [KEY_TAB]={WIDE("tab"),0,0,KEYDATA("\t","\t") }
                      , [KEY_ENTER]={WIDE("return"), WIDE("enter"),0,KEYDATA8("\n") }
                      //, [KEY_PAUSE]={WIDE("pause"),0,KDF_NODEFINE }
                      //, [KEY_ESCAPE]={WIDE("esc"), WIDE("escape"), 0, {{KEYDATA}}} // 0x1b
                      , [KEY_SPACE]={WIDE("space"), WIDE("blank"), 0, KEYDATA8(" ") } //0x20
                      , [KEY_PAGE_UP]={WIDE("prior"), WIDE("pgup"), 0 }
                      , [KEY_PAGE_DOWN]={WIDE("next"), WIDE("pgdn"), 0 }
                      //, [KEY_END]={WIDE("end"), 0, 0, {{COMMANDKEY, (PTEXT)KeyEndCmd}}}
							, [KEY_HOME]={WIDE("home"), 0, 0 }

                      , [KEY_DPAD_LEFT]={WIDE("left"), 0, 0 }

                      , [KEY_DPAD_UP]={"up" , 0, 0 }
                      , [KEY_DPAD_RIGHT]={WIDE("right"), 0, 0 }
                      , [KEY_DPAD_DOWN]={WIDE("down"), 0, 0 }
                      , [KEY_DPAD_CENTER]={WIDE("center"), 0, 0 }
                      //, {"select"}
                      //, [KEY_PRINT]={"print"}
                      //, {"execute"}
                      //, {"snapshot"}
                      , [KEY_0]={WIDE("0"), 0, 0, KEYDATA("0",")")}//0x30
                      , [KEY_1]={WIDE("1"), 0, 0, KEYDATA("1","!")}
							, [KEY_2]={WIDE("2"), 0, 0, KEYDATA("2","@")}
							, [KEY_3]={WIDE("3"), 0, 0, KEYDATA("3","#")}
                      , [KEY_4]={WIDE("4"), 0, 0, KEYDATA("4","$")}
                      , [KEY_5]={WIDE("5"), 0, 0, KEYDATA("5","%")}
                      , [KEY_6]={WIDE("6"), 0, 0, KEYDATA("6","^")}
                      , [KEY_7]={WIDE("7"), 0, 0, KEYDATA("7","&")}
                      , [KEY_8]={WIDE("8"), 0, 0, KEYDATA("8","*")}
                      , [KEY_9]={WIDE("9"), 0, 0, KEYDATA("9","(")}

/* VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
/* VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */
                      //..... to hmm 39
                      , [KEY_A] = {WIDE("a"),0,KDF_NODEFINE,KEYDATA("a","A")}// 0x41 'A'
                      , [KEY_B] = {WIDE("b"),0,KDF_NODEFINE,KEYDATA("b","B")}
                      , [KEY_C] = {WIDE("c"),0,KDF_NODEFINE,KEYDATA("c","C")}
                      , [KEY_D] = {WIDE("d"),0,KDF_NODEFINE,KEYDATA("d","D")}
                      , [KEY_E] = {WIDE("e"),0,KDF_NODEFINE,KEYDATA("e","E")}
                      , [KEY_F] = {WIDE("f"),0,KDF_NODEFINE,KEYDATA("f","F")}
                      , [KEY_G] = {WIDE("g"),0,KDF_NODEFINE,KEYDATA("g","G")}
                      , [KEY_H] = {WIDE("h"),0,KDF_NODEFINE,KEYDATA("h","H")}
                      , [KEY_I] = {WIDE("i"),0,KDF_NODEFINE,KEYDATA("i","I")}
                      , [KEY_J] = {WIDE("j"),0,KDF_NODEFINE,KEYDATA("j","J")}
                      , [KEY_K] = {WIDE("k"),0,KDF_NODEFINE,KEYDATA("k","K")}
                      , [KEY_L] = {WIDE("l"),0,KDF_NODEFINE,KEYDATA("l","L")}
                      , [KEY_M] = {WIDE("m"),0,KDF_NODEFINE,KEYDATA("m","M")}
                      , [KEY_N] = {WIDE("n"),0,KDF_NODEFINE,KEYDATA("n","N")}
                      , [KEY_O] = {WIDE("o"),0,KDF_NODEFINE,KEYDATA("o","O")}
                      , [KEY_P] = {WIDE("p"),0,KDF_NODEFINE,KEYDATA("p","P")}
                      , [KEY_Q] = {WIDE("q"),0,KDF_NODEFINE,KEYDATA("q","Q")}
                      , [KEY_R] = {WIDE("r"),0,KDF_NODEFINE,KEYDATA("r","R")}
                      , [KEY_S] = {WIDE("s"),0,KDF_NODEFINE,KEYDATA("s","S")}
                      , [KEY_T] = {WIDE("t"),0,KDF_NODEFINE,KEYDATA("t","T")}
                      , [KEY_U] = {WIDE("u"),0,KDF_NODEFINE,KEYDATA("u","U")}
                      , [KEY_V] = {WIDE("v"),0,KDF_NODEFINE,KEYDATA("v","V")}
                      , [KEY_W] = {WIDE("w"),0,KDF_NODEFINE,KEYDATA("w","W")}
                      , [KEY_X] = {WIDE("x"),0,KDF_NODEFINE,KEYDATA("x","X")}
                      , [KEY_Y] = {WIDE("y"),0,KDF_NODEFINE,KEYDATA("y","Y")}
                      , [KEY_Z] = {WIDE("z"),0,KDF_NODEFINE,KEYDATA("z","Z")}

                      //, {WIDE("numlock"),0,KDF_NODEFINE} // 0x90
                      //, {WIDE("scroll"),0,KDF_NODEFINE}

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
                      , [KEY_SHIFT_LEFT] = {WIDE("lshift"),0,KDF_NODEFINE,KEY_COMMAND_SHIFT}  // 0xa0
                      , [KEY_SHIFT_RIGHT] ={WIDE("rshift"),0,KDF_NODEFINE,KEY_COMMAND_SHIFT}
                      //, NONAMES // {WIDE("lctrl"), WIDE("lcontrol"),0,KDF_NODEFINE}
                      //, NONAMES // {WIDE("rctrl"), WIDE("rcontrol"),0,KDF_NODEFINE}
                      //, NONAMES // {WIDE("lmenu"), WIDE("lalt"),0,KDF_NODEFINE }
                      //, NONAMES // {WIDE("rmenu"), WIDE("ralt"),0,KDF_NODEFINE }
							, [KEY_SEMICOLON]={WIDE(";"), WIDE("semicolon"),0,KEYDATA(";",":")}
                      , [KEY_EQUALS]={WIDE("="), WIDE("equal"),0    ,KEYDATA("=","+")}
                      , [KEY_COMMA]={WIDE(","), WIDE("comma"),0    ,KEYDATA(",","<")}
                      , [KEY_MINUS]={WIDE("-"), WIDE("dash"),0     ,KEYDATA("-","_")}
                      , [KEY_PERIOD]={WIDE("."), WIDE("period"),0   ,KEYDATA(".",">")}
                      , [KEY_SLASH]={WIDE("/"),WIDE("slash"),0     ,KEYDATA("/","?")}
                      , [KEY_GRAVE]={WIDE("`"), WIDE("accent"),0   ,KEYDATA("`","~")}
							, [KEY_LEFT_BRACKET]={ WIDE("["), WIDE("lbracket"),0  ,KEYDATA("[","{")}
							, [KEY_BACKSLASH]={ WIDE("\\"), WIDE("backslash"),0,KEYDATA("\\","|")}
							, [KEY_RIGHT_BRACKET]={ WIDE("]"), WIDE("rbracket"),0  ,KEYDATA("]","}")}
                      , [KEY_APOSTROPHE]={ WIDE("'"), WIDE("apostrophe"),0     ,KEYDATA("`","~")}

                     // I get KEY_SHIFT_LEFT (mod 65), KEY_APOSTROPHE( 65) (up?), left_hisft(up) mod (0)
                      //, [KEY_APOSTROPHE]={ WIDE("'"), WIDE("quote"),0     ,{{KEYDATA}
                      //                        ,{KEYDATA}} }
};


static struct keymap_state
{
	struct {
		BIT_FIELD bShifted : 1;
		BIT_FIELD bShowing : 1;
	} flags;
	void (*show_keyboard)(void);
   void (*hide_keyboard)(void);
} keymap_local;

//----------------------------------------------------------------------------

TEXTCHAR  GetKeyText (int key)
{
   if( l.current_key_text )
		return l.current_key_text[0];
   return 0;
}


PTEXT SACK_Vidlib_GetKeyText( int pressed, int key_index, int *used )
{
   int used = 0;
	// check current keyboard override...
	int mod = keymap_local.flags.bShifted?1:0;
	{
		int result = 0;
		//Log1( WIDE("Keyfunc = %d"), KeyDefs[key_index].op[mod].bFunction );
		switch( LinuxKeyDefs[key_index].op[mod].bFunction )
		{
		case KEY_COMMAND_SHIFT:
         if( pressed )
				keymap_local.flags.bShifted = 1;
         else
				keymap_local.flags.bShifted = 0;
			used = 1;
         break;
		case KEYDATA:
			if( pressed )
				return LinuxKeyDefs[key_index].op[mod].pStroke;
         break;
		}
	}
   return NULL;
}


void SACK_Vidlib_SetTriggerKeyboard( void (*show)(void), void(*hide)(void))
{
	keymap_local.show_keyboard = show;
	keymap_local.hide_keyboard = hide;
}

void SACK_Vidlib_ShowInputDevice( void )
{
   keymap_local.flags.bShowing = 1;
	if( keymap_local.show_keyboard )
      keymap_local.show_keyboard();
}

void SACK_Vidlib_HideInputDevice( void )
{
   keymap_local.flags.bShowing = 0;
	if( keymap_local.hide_keyboard )
      keymap_local.hide_keyboard();
}

void SACK_Vidlib_ToggleInputDevice( void )
{
   keymap_local.flags.bShowing = !keymap_local.flags.bShowing;
	if( keymap_local.flags.bShowing )
	{
		if( keymap_local.show_keyboard )
			keymap_local.show_keyboard();
	}
   else
	{
		if( keymap_local.hide_keyboard )
			keymap_local.hide_keyboard();
	}
}

RENDER_NAMESPACE_END
