#Quick howto make a OpenGL plugin

# Introduction #

Making a new opengl plugin is fairly simple....

There is a simple project in sack/src/games/simple\_array\_tester

Copying this project, and then updating the CMakeLists.txt..

Change the project\_name

```
   #PROJECT_NAME( simple_array_tester )
   PROJECT_NAME( your_project_name )
```

Update the list of sources...

```
ADD_LIBRARY( ${PROJECT_NAME} SHARED
${FIRST_GCC_LIBRARY_SOURCE} 
     shader.cpp  # change these sources, keep everything else.
${LAST_GCC_LIBRARY_SOURCE} 
)
```



This uses SACK callback registration, so events are defined as

see <render3d.h> other events for mouse and update.

```
static <return type> EventMacroName( "Your Instance Name(what this is)" )( /* parameters as required by macro*/ )
{
}

// you may change the text string, and the names of the parameters, but not the types.
static PTRSZVAL OnInit3d( WIDE( "Simple Shader Array" ) )( PTRANSFORM camera, RCOORD identity_depth )
{
    // return 0 to fail init, no further events will be called.
    // return non zero, and that pointer-size-value is passed to 
    // each other event...
}

static void OnFirstDraw3d( WIDE( "Simple Shader Array" ) )( PTRSZVAL psvInit )
{
}
static void OnBeginDraw3d( WIDE( "Simple Shader Array" ) )( PTRSZVAL psv,PTRANSFORM camera )
{
}

static void OnDraw3d( WIDE("Simple Shader Array") )( PTRSZVAL psvView )
{
}

```



---



This is a stripped down shader.cpp (as referenced in default CMakeLists.txt).... only the bare minimum initialization is done; everthing else is ripped out here.  This is the full  [source](http://code.google.com/p/c-system-abstraction-component-gui/source/browse/src/games/simple_array_tester/shader.cpp)

```
// don't define aliases for open/fopen et al.
#define NO_FILEOP_ALIAS
#include <stdhdrs.h>
// define the pointer to render interface to use
#define USE_RENDER_INTERFACE l.pri
// define the pointer to image interface to use
#define USE_IMAGE_INTERFACE l.pii
// this is maybe optional
#define NEED_VECTLIB_COMPARE

#include <vectlib.h>

#include <render.h>
#include <render3d.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

// define local instance.
#define YOUR_PROJECT_MAIN_SOURCE  
#include "local.h"

PRELOAD( InitMyProject )
{
   l.pri = GetDisplayInterface();
   l.pii = GetImageInterface();
}

static void OnDraw3d( WIDE("Simple Shader Array") )( PTRSZVAL psvView )
{ 
   // psv passed is the value you returned from Init.

   // render the world; assume it has already been cleared

   /************
    *   More common usage
    *   {
    *      struct my_gl_context *context = (struct my_gl_context*)psvInit;
    *      
    *      // ... issue opengl glBegin/glEnd.... etc... 
    *      // Draw your portion of the scene
    *
    *   }
    ************/
   
}

static void OnBeginDraw3d( WIDE( "Simple Shader Array" ) )( PTRSZVAL psv,PTRANSFORM camera )
{
   /************
    *   More common usage
    *   {
    *      struct my_gl_context *context = (struct my_gl_context*)psvInit;
    *      
    *      // Update time relative things; trigger physics tick?
    *
    *   }
    ************/
    // psv passed is the value you returned from Init.

    // this is a chance to move the camera; 
   // this is not the recommended place to move the camera...
   // too many people setting the camera will result in noone
   // getting what they want...
}

static void OnFirstDraw3d( WIDE( "Simple Shader Array" ) )( PTRSZVAL psvInit )
{
   /************
    *   More common usage
    *   {
    *      struct my_gl_context *context = (struct my_gl_context*)psvInit;
    *      
    *      // ... init context textures/programs/etc 
    *
    *   }
    ************/

	// and really if initshader fails, it sets up in local flags and 
	// states to make sure we just fall back to the old way.
	// so should load the classic image along with any new images.
	if (GLEW_OK != glewInit() )
	{
		// okay let's just init glew.
		return;
	}

    // Any initialization that requires an opengl context can be done here
    // things like creating programs... should be done here.

}

static PTRSZVAL OnInit3d( WIDE( "Simple Shader Array" ) )( PTRANSFORM camera, RCOORD identity_depth )
{
   // keep the camera as my data... I don't have any 
   // custom instance data in this example. 
     
   /************
    *   More common usage
    *   {
    *       struct my_gl_context *context = New( struct my_gl_context );
    *       MemSet( context, 0, sizeof( struct my_gl_context ) );
    *       context->camera = camera;
    *       context->identity_depth = identity_depth;
    *       return (PTRSZVAL)context;
    *   }
    *
    *   // and somewhere defined...
    *   struct my_gl_context {
    *      PTRANSFORM camera;
    *      RCOORD identity_depth;
    *      int program_shader_id;  // shaders are instanced per display
    *   }
    ***********/

   return (PTRSZVAL)camera;
}
```

.... example local.h....
```

#ifndef YOUR_PROJECT_MAIN_SOURCE
extern
#endif
 struct your_project_local {
   PRENDER_INTERFACE pri;
   PIMAGE_ITERFACE pii;
} your_project_local;
#define l your_project_local;
```


You get called for each display that is opened.
There may be configurations that open multiple displays for surround-o-vision.  This means that textures must be loaded into each context.  Each context which is created calls OnInit3d().  This is not within an OpenGL Context, so you may only do very simple things for initialization.  Loading GLEW at this point will fail though.

Then after all plugins are initialized, each display will call OnBeginDraw3d.  The first time a plugin ever draws, it receives OnFirstBeginDraw3d event.  This is the point that initialization that requires the GL context to be active can be done.  Loading textures...

The name used in each of the events MUST match the name of the OnInit3d method.