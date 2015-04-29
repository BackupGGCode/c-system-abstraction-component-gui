# c-system-abstraction-component-gui
It's a core library that provides some simple container types to store references of user objects.

Built on basic types that can be allocated in application-shared memory, is a graphics system that provides rundamentary image loading (png,bmp,jpg,tga?), some basic line functions, some optimized for specific jobs like horizontal or vertical, some more generalized that allowing the developer to specify a callback for each point on the line.  Colors are 32 bit only, performing translation only when hitting a device.

Images are not much use if they cannot be shown, so a slim interface between images and windows (OpenGL and X11 on linux; D3D,OpenGL1/2,native on windows; OpenGLES2 on QNX; OpenGLES2/framebuffer on Android;WebSocket/HTML5 on any system).  There is a video rendering device that allows an application to serve its interface as a web page.  Allows processing of clicks, but functions like the mouse actually moving are lost.  Have also rendered images as surfaces in an OpenGL context.

Finally, to facilitate cross platform development, certain rundatmentary services like timer scheduler services, and thread abstraction, so other components use the same code to create a compatible threads with a consistant control interface.

But really, that's not all, there is an application core that can be extended with a pluggable architecture, with a consistant method for loading and accessing shared libraries.  Plugins provide additional controls with support for true alpha transaparency.  The application itself can be entirly transparent, and controls may seemingly hover over other surfaces.

Recently migrated to CMake, so it's much more likely that you can get a method to build it that will work on your platform.
