# ChangeLog #

**Tag version 3.0.2**
  * Fix linux Opengl 1.5 output
  * Fixed linux opengl keymap support (puregl2 video, puregl or puregl2 imagelib should work; puregl2 is shader based)
  * Fix GCC 64 bit issues with automaton project (brain generic variant type conflicts;duplications.   long = 64 bit in linux).
  * Handle X window closes better.

### some notes ###
  * This depricates displaylib; and requires X11 with at least basic OpenGL support (quad,triangle,texture,solid color).  Headless display is another alternative; The old Displaylib hook to SDL could be updated to be an alternative server for proxy display library.
  * native client to proxy still has issues with delayed-mouse/display motion; fixed that in puregl by fixing GetDisplay position...

**Tag version 3.0.1**
  * Fix linux build (64bit); fix lots of warnings from linux regarding mis-sized types; upgrade to %zd format for sizeof things.
  * Improved make system detecting linux packages
  * Fix linux headless output (added linux keymap)
  * Consolidated type defintions and type format specifications.
  * Added [headless display](headlessdisplay.md).
  * removed some unused/depricated interface methods in render.
  * forced function return type declaration for intershell methods; previously it was implied, and unclear what the return should be. **binary compatibility breaks**

**Release Version (dekware-3.0-alpha3) [in progress](.md)**
  * Fix deployed intershell install rule to copy set\_config.exe.
  * Fix default option database DSN in InterShell for SQL configuration.
  * 3D
    * Implement key dispatch to virtual space plugins.
  * Dekware
    * Began quick math module.  Was a simple project to reaquaint myself with the current interface methods. (sin,cos,sqrt)
    * Add interface module to Virtuality objects and Automaton brains
    * Add ability for brains to trigger scripts
    * Add ability for brains to terminate scripts
    * Fixed cards module for internal sentience changes; When a command is invoked on an object that is not itself aware, the current sentience transfers to the object performing the command for the duration of the command; accordingly if it's a macro, all commands should be as if the current awareness '/become'ed the object doing the command.
    * Added registered function handlers for ObjectMacroCreate and ObjectMacroDestroy; this allowed macros added to objects that had the virtuality interface to also be added to brains. (Issue; no way to modify/extend a macro, recreating macro automatically destroys the prior, which means it will be a different instance and all brain peices should be removed also **ob** )
    * Added default configuration data to bring up robust system.
    * Fixed automatic object extension (in the case of PSI control creation, automatic command extension was failing from incompatibility between old and new command registrations)
  * Brainboard
    * Make component menu regenerate when right clicking; should have some sort of 'updated' flag to prevent unneeded regeneration.
    * Fixed loading saved brains; SQL fixes.
  * Fixed intermitant build error with msgsvr that the msgsvr.ilk file is in use.  Two targets had the same root name; sack.msgsvr.service.exe and sack.msgsvr.service.plugin (and no standard extension) so the same sack.msgsvr.service.ilk was used; Solution is to add an extra '.' at the end of the filename; Under windows this will be OK because the final '.' is dropped going to the file system.
  * Added EditOptions.plugin to allow triggering the editoption dialog in an existing program.
  * Fixed push/end handling for SQL... and dangling statments; a SQLRecordQuery;SQLcommand;sqlrecordquery would fail no matter how it was pushed/ended because the inbetween statment would cause extra/incomplete deletions.
  * Added method to get real name from class tree; added reversibility to get from a node to the root to get the correct 'original', non aliased, path.
  * fixed 'modulepath' directive in interface configuration files.
  * Intershell
    * Added common properties for page cycler to disable.
    * Changed security hash.
    * Fix bad module name for intershell security module.
    * Add configuration for Page Cycle plugin to be able to disable cycling but leave it loaded.
  * Change border attribute reader to read/write in hex. (updated all existing border things with ID converter, and removed border= references).

**Release Version (2.5.8-rc1)**
  * fixes for building under all available compilers and options.  (android, windows(mingw,openwatcom,vs2012,vs2010), no linux build)

### outstanding issues ###
  * option editor fails to copy options (a way to make new options)
  * ball mixer with bump mapped text still fails (simple opengl output does work)
  * slider could be optimized to build an internal image of the thumb
  * implemention of json flatland worldscape server incomplete.
  * 360 view does not work in d3d2, d3d3
  * psi.command shell fails to draw correctly always.
  * win32 window focus for keyboard input and auto-raising is deficient.
  * Unicode intershell fails to keep configuration.

**Release Version (2.5.8-dev1)**
  * --(10/13/2013 - windows 8.1 to be released 10/17/2013.... always not quite what I need when I need it; still.)
  * Add directx 11 driver; support transparent windows on windows 8. (in progress)
  * Add directx 11 driver support.
  * this was more of a work in progress point; most compiler tests worked; broke some external projects.

**Release Version (2.5.7-rc4 2012-10-??)**
  * All projects build as unicode; most even function :)
  * Fixed a compile issue from last version (pngimage).


**Release candidate/version tagging was done incorrectly this pass, next time... 2.5.7-rc3 is stable**

**Release Version (2.5.7-rc3) 2013-10-09**
  * Fixup CMake scripts to support CLR (CLI/CLR) compilation; a few fixes for C files which got forced to C++.  Monolithic build will fail with CLR (freetype/zlib issues; zlib has a source standard of K&R for some reason; freetype uses a keyword 'generic'...)  Modular build does succeed... (fails to execute)
  * Do not use DllMain with Mingw/GCC; causes link errors against wrong DLLS (intershell.core pulled bag++.dll); also causes multiple starting point issues with watcom.
  * Protect against the global space going away during timer dispatch
  * updated interface configurations installed for modular build.
  * removed noisy message in filemirror while waiting for external applications to exit.
  * fixed wide character to text conversion; gave it too large of a buffer;
  * Fixed modular build failure for watcom - sqlite interface issues.
  * Compiled Watcom (modular/monolithic; unicode/ascii; release/debug) (Success)
  * Compiled MinGW (modular/monolithic; unicode/ascii; release/debug)  (Success)
  * Compiled VS10 (2010) (modular/monolithic; unicode/ascii; release/debug)  (Success)
  * Compiled VS11 (2012) (modular/monolithic; unicode/ascii; release/debug)  (Success)
  * **Fixed SQL result underflow**
  * Added libshapefile 1.3.0.


**Release Version (2.5.7-rc2) 2013-10-01**
  * Fixed some unicode issues in extracting information from fonts and caching that information.
  * Work around for watcom vsnwprintf misimplementation.
  * Fixed unicode issue building table statements with constraint clauses.
  * Fix result code return for http processing (break it for protocol upgrade hooks) (affected by returning the more generic result, if not otherwise defined internally)
  * Fix multi-surface first init. (affected by update for late loaded plugins)
  * re-enable ability to output sprites
  * fixed portable programs compiled with Watcom; deadstart DllMain hook for unloading caused multiple entry points;  Added cmake macro for exectuables.
  * tested watcom modular/monolithic release/debug, mingw modular/monolithic release/debug, vs10/11 modular/monolithic release/debug (all build)
  * added another check for interface configuration files; strips the leading part of program name off to uses remaining parts.  This allows, for example, copying editoptions.exe to editoptions.d3d.exe and editoptions.gl2.exe which will additionally look for 'd3d.interface.conf' and 'gl2.interface.conf' in addition to the full program name prefix and just 'interface.conf'.


**Release Version (2.5.7) 2013-09-29**
  * Fix/Implement OpenGL/OpenGL2(4.0?)/DX9/(DX9 Shader only?) rendering sprites of sprites.
  * Add usage output for mercurial\_version\_tagger with no arguments.
  * Finish DX9 3D support including sprites.
  * Fix startup database collision potential.
  * Add ODBC Connection pool manager
  * Use ODBC Connection pool manager in Option Database
  * Expose getting option connections by DSN name and version.
  * Improve support for long option values; implement option version 4
  * Improved option editor (removed delete and copy I think, becuase of desync with internal name tree and database)
  * Updated listbox control to support transparent colors better.
  * Updated scrollbar control to support transparent colors better.
  * Fixed event issue with physics ball demo when real provider is available.
  * Improved android support; should push a release of Dekware.
  * Fixed aligning control colors to windows colors for all render plugins.
  * Fix sprite output on opengl, opengl 2, and d3d plugins.
  * Delay logging option loading (syslog) until after interface.conf is processed to allow defaulting of those options.
  * Fix/allow late loaded 3d render module initialization.
  * Update to libpng 1.6.5
    * Update to libpng 1.6.6 (fixes no setjmp support)
  * Update to zlib 1.2.7
  * Update to freetype 2.5.0.1
  * Update to libjpeg 9
  * Intershell Updates
    * Added button to restart
    * Added button to reset SQL configuration to defaults
    * fixed set\_config to update saved SQL configuration(removed stripping .config from name)
    * Added ability to clone from elements in a macro.
  * Fixed positioning 3d displays on other monitors.

### outstanding issues ###
  * option editor fails to copy
  * ball mixer with bump mapped text still fails
  * slider could be optimized to build an internal image of the thumb
  * implemention of json flatland worldscape server incomplete.
  * compilers not excercised.
  * 360 view does not work in d3d2, d3d3 or puregl2; only puregl (vidlib issue)
  * 

### considered directions to go ###
  * automatic capture of 3d draw events to collapse into a single draw array; how do program selections change when using draw array?  program change will have to trigger a new collection.
  * implement some sort of callback handler like expat for parsed json messages inbound.
  * re-implement sprite particles on button clicks.

## first creation 2013/09/26 ##


  * include uuid 1.6.2
  * include sqlite 3.7.16.2
  * include genx  (project has had no updates; fixed some issues)
  * include expat (project has had no updates)

This is what it is.
To be filled in with current significant changes between last major fork and now.