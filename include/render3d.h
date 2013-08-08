#ifndef __RENDER3D_EXTENSIONS_DEFINED__
#define __RENDER3D_EXTENSIONS_DEFINED__

#include <render.h>
#include <vectlib.h>

typedef struct render_3d_interface_tag
{
	RENDER_PROC_PTR( PTRANSFORM, GetRenderTransform)         ( PRENDERER );
	// returns TRUE if at least one of the points is inside the camera clip
	RENDER_PROC_PTR( LOGICAL, ClipPoints)         ( P_POINT points, int nPoints );
#ifdef _D3D_DRIVER
	IDirect3DDevice9 *current_device;
#endif
} RENDER3D_INTERFACE, *PRENDER3D_INTERFACE;

#ifdef _D3D_DRIVER
#define g_d3d_device  (USE_RENDER3D_INTERFACE)->current_device
#endif

// static PTRSZVAL OnInit3d( "Virtuality" )( PMatrix projection, PTRANSFORM camera, RCOORD *identity_depth, RCOORD *aspect )
// each Init is called once for each display that is opened; the application recevies the reference
// of the transform used for the camera.  This may be used to clip objects that are out of scene.
// If you return 0 as the PTRSZVAL result, you will not get any further events.
//
// - (passes identity_depth as a reference to identity depty, because if the screen size changes, the identity depth also changes)
// identity_depth this is relative to how the display was opened.  identity is where there is a 1:1 relation
// between rendered image pixels rendered and physical display pixels.
// This may be arbitrarily overridden to be closer than normal in case the physical display dpi is too dense.
// identity depth should be used for rendering icons on objects.
#define OnInit3d(name) \
	__DefineRegistryMethod(WIDE("sack/render/puregl"),Init3d,WIDE("init3d"),name,WIDE("ExtraInit3d"),PTRSZVAL,(PMatrix projection, PTRANSFORM camera, RCOORD *identity_depth, RCOORD *aspect ),__LINE__)

// static void OnClose3d( "Virtuality" )(PTRSZVAL psvInit)
// handle event when the specified display context is closed.  The value passed is the result from Init3d();
// All resources relavent to the 3d engine should be released.  (shaders)  and statuses cleared so reinitialization can occur.
#define OnClose3d(name) \
	__DefineRegistryMethod(WIDE("sack/render/puregl"),Close3d,WIDE("draw3d"),name,WIDE("ExtraClose3d"),void,(PTRSZVAL psvInit),__LINE__)

// static LOGICAL OnUpdate3d( "Virtuality" )( PTRSZVAL psvInit, PTRANSFORM eye_transform );
// called when a new frame will be rendered.  Once per frame.  All others are called per-camera per-frame.
// can update the common viewpoint here, and it will propagate, otherwise the camera ends up resetting to
// this point, unless the named transformation matrix is loaded manually.
// it is passed the origin transformation view into the universe)
// since this is potentially one for multiple Init instances, cannot pass the instance this applies to, since it is all.
// the PTRANSFORM origin has just previously been updated with Move(), so it can have speed and acceleration applied
// return true/false to indicate a desire to draw this frame.  If this and nothing else changed, the frame will be skipped.
#define OnUpdate3d(name) \
	__DefineRegistryMethod(WIDE("sack/render/puregl"),Update3d,WIDE("draw3d"),name,WIDE("Update3d"),LOGICAL,(PTRANSFORM origin ),__LINE__)

// static void OnFirstDraw3d( "Virtuality" )( PTRSZVAL psvInit );
// called the first time a camera draws itself;
// allows user code to load geometry into the camera... 
// it is passed the instance handle returned from Init3d
#define OnFirstDraw3d(name) \
	__DefineRegistryMethod(WIDE("sack/render/puregl"),FirstDraw3d,WIDE("draw3d"),name,WIDE("FirstDraw3d"),void,(PTRSZVAL psvInit ),__LINE__)


// static void OnDraw3d( "Virtuality" )( PTRSZVAL psvInit, PRAY mouse )
// this is called once for each display that is opened, and for each OnInit3d that did not return 0.
// the psvInit is the init value returned, the mouse is the mouse as it is; it will be NULL if the mouse
// is not in the current display and we are just drawing.
#define OnBeginDraw3d(name) \
	__DefineRegistryMethod(WIDE("sack/render/puregl"),BeginDraw3d,WIDE("draw3d"),name,WIDE("ExtraBeginDraw3d"),void,(PTRSZVAL psvUser,PTRANSFORM camera),__LINE__)

// static void OnDraw3d( "Virtuality" )( PTRSZVAL psvInit, PRAY mouse )
// this is called once for each display that is opened, and for each OnInit3d that did not return 0.
// the psvInit is the init value returned, the mouse is the mouse as it is; it will be NULL if the mouse
// is not in the current display and we are just drawing.
#define OnDraw3d(name) \
	__DefineRegistryMethod(WIDE("sack/render/puregl"),Draw3d,WIDE("draw3d"),name,WIDE("ExtraDraw3d"),void,(PTRSZVAL psvUser),__LINE__)

// static void OnMouse3d( "Virtuality" )( PTRSZVAL psvInit, PRAY mouse, _32 b )
// this is a real mouse event that is in a display that you returned non 0 during Init3d.
// PRAY is the line represenging the point that the user has the mouse over at the moemnt.
// mouse buttons are passed. for mouse state (may also be key state)
// return FALSE if you did not use the mouse.
// return TRUE if you did, and therefore the event is used and noonne else should make two things happen...
#define OnMouse3d(name) \
	__DefineRegistryMethod(WIDE("sack/render/puregl"),Mouse3d,WIDE("mouse3d"),name,WIDE("ExtraMouse3d"),LOGICAL,(PTRSZVAL psvUser, PRAY mouse_ray, _32 b),__LINE__)



#define GetRender3dInterface() (PRENDER3D_INTERFACE)GetInterface( WIDE("render.3d") )

#define GetRenderTransform             REND_PROC_ALIAS(GetRenderTransform)
#define ClipPoints             REND_PROC_ALIAS(ClipPoints)

#endif // __RENDER3D_EXTENSIONS_DEFINED__
