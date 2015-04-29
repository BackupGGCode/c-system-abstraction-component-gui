# Introduction #

How to use SACK....




# Details #

  1. Download package.  ( a precompiled binary.)
  1. Extract to a location.
  1. (optional) Run sack\_deploy.exe.
> > This generates a CMakePackage file that can be included to define SACK  variables for CMake.  It also writes to the registry where this is currently installed; but with the difference in 64/32 bit, this is impractical. Run 'sack\_deploy -nr' for no registry.  If cmake is a 32 bit build (only one available), then the 32 bit sack\_deploy.exe must be run to register even the 64 bit sack; because there is a separation in the registry of 32 and 64 bit accesses.


### building all with cmake ###

```
  mkdir ./sack_build_output
  cd ./sack_build_output
  cmake /path/to/sack/source/sack/cmake_all
  # or...
  cmake-gui /path/to/sack/source/sack/cmake_all
  # or... as appropriate...
  ccmake /path/to/sack/source/sack/cmake_all
```

  * pick your generator
  * pick the build type (CMAKE\_BUILD\_TYPE, debug/release/....)
  * One option which may be important, BUILD\_MONOLITHIC.  If this is set, then the library is built in a big code block, and this limits flexibility.  If this is not set, then the build is more modular, and a bunch of bag..dll are created instead of a sack\_bag.dll; however, with the modular build, alternate display interfaces may be selected.

```
  cmake --build .
```


## Visual Studio ##
Add **<installed sack root>/include/SACK** to include directories for your project; also include **<installed sack root>/include** so you can just include <sack/...> and sack can still include without.

Add sack\_bag.lib or sack\_bag++.lib to your project as appropriate.

Copy sack\_bag.dll, sack\_bag++.dll, and editoptions.exe with your project.


## Build from Source ##

Requires installed cmake in your path.
Requires a compiler of your choice installed (although only thouroughly tested VS2010, mingw, openwatcom )

### using command prompt and batch file ###
In the root of the sack source directory is a batch for windows called 'cmake\_all.bat'

In a different directory, outside of the source tree, make a directory, and go there with a command prompt (or make a shortcut to the batch file just mentioned, and change the target path).
run c:\full\path\to\sack\cmake\_all.bat  (although the drive may be omitted)

this will build a debug version, using visual studio with an 32 bit output.

cmake\_all.bat takes two parameters.
The first paramter that cmake\_all.bat takes is the environment which may be one of ( 64, vs11, vs11x64, watcom, mingw ) all else defaults as vs10-x86.  64 uses vs10 (2010) as the generator.
watcom generates watcom wmake files
mingw generates mingw makefiles

each of these is then built appropraitely.

The second parameter that cmake\_all.bat takes is 'debug' or 'release' or (err, the other two modes for cmake by default, minsizedebug?, reldebinfo?)

After this has run, options can be further refined by running 'cmake-gui .' in the build directory.

the build of all will make a directory for the compiler, a directory for (debug/release)_solution where things are compiled and a directory (debug/release)_out where the binary is installed from the builds.

in (compiler)/(debug/release)_solution/core is sack core library.  running cmake-gui here may provide some other lower level options.  This cmake build directory would be the same as running 'cmake /path/to/sack'._

the output is (compiler)/(debug/release)_out.  The output has an include folder with a SACK sub-folder; proper foreign includes should be #include <sack/stdhdrs.h>._

There is a ./lib/debug directory containing libraries that can be linked against, and the utility programs and dll's go into ./bin/debug.

Both release and debug and other builds may be built in parallel, the installed target will be the merge of all the builds, in seperate folders.

## CMake ##
in your CMakeLists.txt
```
GET_FILENAME_COMPONENT(SACK_SDK_ROOT_PATH "\[HKEY_LOCAL_MACHINE\\SOFTWARE\\Freedom Collective\\SACK;Install_Dir\]" ABSOLUTE CACHE)

PROJECT( declare_a_project )

include( ${SACK_SDK_ROOT_PATH}/CMakePackage )
LINK_DIRECTORIES( ${SACK_LIBRARY_DIR} )
include_directories( ${SACK_INCLUDE_DIR} )
INSTALL_SACK( bin )

target_link_libraries( <your project>	${SACK_LIBRARIES} )
```


Or a recommended way to include sack, pass SACK\_SDK\_ROOT\_PATH as a cmake -D on t he command line..
```
if( NOT SACK_SDK_ROOT_PATH )
GET_FILENAME_COMPONENT(SACK_SDK_ROOT_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Freedom Collective\\SACK;Install_Dir]" ABSOLUTE CACHE)
else( NOT SACK_SDK_ROOT_PATH )
set(SACK_SDK_ROOT_PATH ${SACK_SDK_ROOT_PATH} CACHE STRING "SACK SDK Root" )
endif( NOT SACK_SDK_ROOT_PATH )

include( ${SACK_SDK_ROOT_PATH}/CMakePackage )
INSTALL_SACK( bin )
include_directories( ${SACK_INCLUDE_DIR} )
LINK_DIRECTORIES( ${SACK_LIBRARY_DIR} )

```

## Bulding from sources ##
CMake GUI can be used for most cases.  Some options are semi-depricated.
| BUILD\_MONOLITHIC | determines whether sack\_bag is built or bag.dll, bag.image.dll, bag.video.dll and bag.psi.dll |
|:------------------|:-----------------------------------------------------------------------------------------------|
| CMAKE\_BUILD\_TYPE | Set for user ease... preset choices for Debug, Release, etc |
| CMAKE\_INSTALL\_PREFIX | where to install this build |
| EXTREME\_CODE\_DEBUG | Sets compile options for MINGW (-Wall -Wextra -Wconversion -Wsign-conversion -Wsign-compare -Wtype-limits)|
| NEED\_FREETYPE | A Full MinGW or Linux system should have freetype as a system library, since windows does not, this option uses a SACK-local copy of freetype.  Freetype is included and SACK may be used the same as freetype.|
| NEED\_JPEG | Jpeg Library support - when set, includes local copy |
| NEED\_PNG | PNG library support - when set, includes local copy |
| NEED\_ZLIB | Zlib library support for PNG and Freetype - when this option is set uses internal zlib |
| USE\_ASSEMBLY | Unimplemented option, this should enable using optimized assembly sources (image operations); Since modern compilers are getting better about optimizing, there isn't a WHOLE lot of benefit; does use MMX for alpha blending |
| USE\_ODBC | Enable compiling against ODBC, exports SQL support library.  This can be used to disable all SQL access support.  (See USE\_OPTIONS) |
| USE\_OPTIONS | Enables internal use of options, also includes the code to support configuration options.  Option code uses a SQL or Sqlite database to store options.  If USE\_ODBC is disabled, USE\_OPTIONS is also disabled (code compiles with flag `__NO_OPTIONS__`) |
| USE\_SQLITE | Enables use of SQLITE databases as an alternative to ODBC databases.  |
| USE\_SQLITE\_EXTERNAL | Attempts to use a system Sqlite library instead of building the one included in the SACK tree; similar to PNG, Jpeg, and Freetype) |
| USING\_OLD\_MINGW | Someday MinGW will have a full windows API support, but until then, this option allows the code to define functions which are otherwise missing (UpdateLayeredWindowIndirect for instance) |
| WIN32\_VERSION | Allows you to specify the version of Windows API You want to build against.  This should probably be 0x501 for all modern systems. |
| `__ANDROID__` | Set to build for android target platform. |
| `__ARM__` | When this is set, certain options are optimized for tiny platform... these are custom work-arounds for targeting 16 bit color linux, and windows ARM CE systems. |
| `__LINUX64__` | This option changes code compilation to work better under a 64 bit linux system.  The code will mostly work if you omit this and define LINUX only, but this improves 64 bit target compilation |
| `__LINUX__` | This should be set if you are building on a linux system.|
| `__NO_GUI__` | Do not compile any of the GUI components.  No image support, No video support, no jpeg, png, freetype.  May modify behavior of options, since without a GUI the option layer cannot prompt for configuration defaults. |
| `__WINDOWS_ARM_CE__` | a symbol to enable hacks to build this with windows CE 5 ARM target. |



# Linux Builds #
(02/12/2014)
Updated to use find\_package() in cmake; so if packages are not found, and substitutions are available internally,t hey will be additionally built.

### Ubuntu ###
you will need the following packages AT LEAST...

mesa-dev
unixodbc-dev

#android
# Android Build #

Things you will need...

  * [MinGW](http://www.mingw.org/) Download the installer; and install
  * [CMake](http://cmake.org/) Cmake as usual
  * [Android SDK](https://developer.android.com/sdk/index.ht)
  * [Android NDK](https://developer.android.com/tools/sdk/ndk/index.html)
  * [Apache-ANT](http://ant.apache.org/bindownload.cgi)

NDK I think is handled by the android sdk manager now...

Android tools need to be available in the path.  Java needs to be setup ahead of time too...

android-sdk-windows is the directory that contains 'SDK-Manager.exe'

`a2_toolchain.txt` in sack root will probably have to be edited for your appropriate tools; target platform, etc...

```
set tooldrive={I installed everything into a directory...}
set ANDROID_HOME=j:\general\android\android-sdk-windows
set JAVA_HOME=C:\Program Files\Java\jdk1.7.0_51
set path=%PATH%;%tooldrive%\tools\unix;%tooldrive%\tools\unix\mingw\bin;%tooldrive%\tools\unix\cmake\bin;;%tooldrive%\tools\unix\mingw\lib;"c:\program files\tortoisehg";"c:\program files\tortoisesvn";

: Add android sdk tools
set path=%path%;j:\general\android\android-sdk-windows\platform-tools;j:\general\android\android-sdk-windows\tools;

: Add Ant
set path=%path%;c:\tools\apache-ant-1.9.2\bin

: ADD Java
set path=%path%;"C:\Program Files (x86)\Java\jre7\bin\"
```


This is a sample batch file that should be run from the target build directory....

CMake Generator should be 'MinGW Makefiles'

```
set SACK_BASE={sack base directory that contains 'src'}
# update android ndk base... j:/general/android/android-ndk-r9
# Bullet source may be ommited; it's bypassed in android builds now

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=debug -DCMAKE_TOOLCHAIN_FILE=%SACK_BASE%/a2_toolchain.txt -D__ANDROID__=1 -DANDROID_DIR=j:/general/android/android-ndk-r9 -DANDROID_NDK_VERSION=android-ndk-r9 M:/sack/cmake_all 

make
```

This will produce debug\_out, debug\_solution and debug\_package directories for output; in debug\_package are supported android packages... core, intershell and dekware.

In a package directory you can use the following commands...
```
cmake .   # rebuild the current package
make install  #install the built package to the connected device
make uninstall  # uninstall the package

make uninstall install # uninstall then install the package
```
These commands generate 'adb install <....>' and adb uninstall commands...