How to build to target Android.

# Introduction #

Cover the prerequisites...
Step by step building
iteritive building for maintanence...

# Details #

First, there are a few things you need (for windows; for linux substitute generic GNUMake and GCC for MinGW, packages should already be in the path; not sure about the environment...)
> cmake 2.8+
> mingw (for gcc, make)  (potential to target visual studio alternate)
> Android SDK
> > Make sure to set ANDROID\_HOME to the path the SDK is installed in.

> Java JDK
> > Make sure to set JAVA\_HOME to the path the JDK is installed in.

> Android NDK
> > This path has to be set in /sack/android.bat to match.

> Apache ANT

Your path should contain CMake/bin, MinGW/bin, %ANDROID\_HOME%/platform-tools, %ANDROID\_HOME%/tools, and Apache ANT/bin.

To test that your path is setup, you should be able to type from a command prompt
'gcc' and get 'gcc.EXE: fatal error: no input files'
'ant' and get 'Unable to locate tools.jar....' or similar
'adb' and get the usage for the android tool
'android -u' and get the usage for android (without the -u, the SDK download manager will appear)
'cmake' should show the usage for cmake.

Once all of that works, make a directory somewhere, go there, and run ..../sack/android.bat.  The android.bat file is just a cmake command with a few defines to setup projects appropriate for building to "MinGW Makefiles" as a cmake target, with toolchain file /sack/a2\_toolchain.txt. It will probably have to be adjusted for your specific system.

ANDROID\_SDK\_ROOT is meant to be the common path that the android-SDK and android-NDK are installed in; this is not required.  It's the path leading up to the NDK install directory but not including the ndk directory; the ANDROID\_NDK\_VERSION should be just the name of the directory, so the SDK\_ROOT and NDK\_VERSION are appended.

```
set ANDROID_SDK_ROOT=c:/storage/android

set ANDROID_NDK_VERSION=android-ndk-r9
set ANDROID_DIR=%ANDROID_SDK_ROOT%/%ANDROID_NDK_VERSION%
set PWD=%~dp0
set PWD=%PWD:\=/%

cmake -G "MinGW Makefiles" %PWD%cmake_all -D__ANDROID__=1 -DCMAKE_TOOLCHAIN_FILE=%PWD%a2_toolchain.txt -DANDROID_DIR=%ANDROID_DIR% -DANDROID_NDK_VERSION=%ANDROID_NDK_VERSION%
```

**Some of the options are not getting the configuration steps yet, like ARM and LINUX isn't getting set all the time; and BUILD\_EXTRAS needs to be enabled by default this way....**


Once the cmake command is done, and it doesn't have errors, then you should get a 'debug\_solution' (assuming you haven't somewhere set release).  You should run (in the root build directory) cmake-gui (or ccmake) and make sure to check(enable) 'BUILD\_EXTRAS'

Then in debug\_solution/sack there should be a 'makeit.bat' (or makeit.sh).  Run this batch file.  A sample makeit.bat for core might look like...

```
"C:/tools/unix/cmake/bin/cmake.exe" -G "MinGW Makefiles" -T ""  "M:/sack/cmake_all/.." -DCMAKE_BUILD_TYPE=debug -DCMAKE_INSTALL_PREFIX=C:/general/build/android/debug_out/core -DBUILD_MONOLITHIC=OFF -D__NO_ODBC__=1 -D__LINUX__=1 -D__ARM__=1 -D__ANDROID__=ON -DANDROID_DIR=C:/general/android/android-ndk-r9 -DANDROID_NDK_VERSION=android-ndk-r9 -DNEED_FREETYPE=1 -DNEED_JPEG=1 -DNEED_PNG=1 -DNEED_ZLIB=1 -DBULLET_SOURCE=M:/bullet/source -D__ANDROID__=ON -DANDROID_DIR=C:/general/android/android-ndk-r9 -DANDROID_NDK_VERSION=android-ndk-r9 -DCMAKE_TOOLCHAIN_FILE=/sack/a2_toolchain.txt -DANDROID_DIR=C:/general/android/android-ndk-r9 -DANDROID_NDK_VERSION=android-ndk-r9

make.exe -i install
```

Again, run cmake-gui (ccmake) but this time in (buildroot)/debug\_solution/core and at the bottom, make sure ARM is enabled, LINUX is enabled and ANDROID is enabled

Then the build should be good.  go to the main build directory and run 'make'.

You should also end up with (buildroot)/package directory with some projects inside; for example dekware and intershell android package targets end up there.  Inside these folders is a 'makeit.bat' which can be run to copy the files from the build output ( in buildroot/debug\_out/... ) to the package ( buildroot/package/&lt;target&gt;/package ).  It generates a makefile with 3 targets;

make package
> This takes the files installed into package and makes the .apk output.  it runs 'android update project' and then 'ant debug'.  **The build type doesn't yet go through to make release.**

make uninstall
> This removes the prior package from the device (use adb devices to show devices connected).

make install
> This uses 'adb install (generated target).apk' to the connected device.

## Summary of common build steps ##
```

mkdir ~/build/android
cd ~/build/android

~/src/sack/android.bat
make

cd ~/src/sack/package/dekware
# the first time you must run the configure script
makeit 
cmake . 
make package uninstall install
# the previous can be done in 3 steps.

```

The Makeit.bat for the package looks something like...
```
cmake -G "Unix Makefiles"  -DDEKWARE_SDK_ROOT_PATH=C:/general/build/android/debug_out/dekware M:/sack/cmake_all/../android_build/dekware
```
Basically the makeit specifies the input (the SDK\_ROOT\_PATH) to copy the files from, and the cmake script to use to copy from; and assumes they will be copied to where the makeit.bat is located.

## Some things that might be useful ##

To enable logging from the device... no wait;

To connect to a device...
```
adb devices
-or-
adb connect <address: default port 5555>
```

There is a widget available on android market to enable wifi ADB, called 'Wifi ADB'.  It is a switch which changes adbd from listening on tcp to listening on USB.  If your device was on wifi, and you plugged it in, after changing away from wifi adb, you have to replug your device, then it will show on 'adb devices'

Sometimes you will have to disconnect before reconnecting.  If you walk out of wifi range, or do certain things, the adb connection will reset.

**To enable Logging**
```
adb logcat > c:/tmp/android-log.txt&
```
this command stays in the foreground, starting with an & should put it in the background.  When the device disconnects this process will exit.  YOu can keep reloading the log file, and it will be updated with the logging from the device.

**If your device is connected on USB and you want to connect with network**
```
#adb forward tcp:(host port)  tcp:(dest port)
adb forward tcp:22 tcp:22
```
The example would enable you to ssh from your computer to localhost port 22 and connect to the android device on port 22.  (SSHDroid package also supports SFTP)

## **Do NOT abort PACKING** ##
If you abort ant debug (the phase tha builds the package); there is a high chance that the files in the bin directory will be corrupt.  You should clear the package directory in (buildpath)/package/(target)/package  <-- the deepest 'package' directroy.  and rerun the makeit.bat (cmake .) in (buildpath)/package/(target)


## Signing For Release ##
$25 registration fee to google (this year)
```
keytool -genkey -v -keystore my-release-key.keystore -alias alias_name -keyalg RSA -keysize 2048 -validity 10000
```



## Native Activity Implementation ##
Forked the original native activity in android-9.
Deadstart code for android is in src/deadstart/android
  * src/deadstart/android/NativeActivity.Java
  * android\_build/MyNativeActivity.cm extends NativeActivity to provide multiple package entry points.
  * src/deadstart/android/android\_util.cpp - display/hide soft keyboard
  * src/deadstart/android/default\_android\_main.c - this is the generic 'main' of a application; it sets up handlers to forward events into renderer appropriately.
  * src/deadstart/android/native\_app\_glue.c - this is another layer of indirections; it is referenced by NativeActivity and is the first receiver of events; which then reformat and call default\_android\_main.  Maybe this just handles some things with default handlers.

default\_android\_main.c invokes deadstart and loads various 'known' entry points to hook to display.  It then passes control to "SACK\_Main" symbol which is in the lib{package}.code.so
This is done in a separate thread; so the normal `while(1) sleep();` should work. (although should be `while( !some_way_to_indicate_exit)`  )

## Android System Features ##
  * A library cannot be loaded again if it fails the first time.
> > If dlopen() is used on a library, and that library has a failed symbol resolution, dlopen() called a second time on that library will result with an error that it has already failed.
  * Libraries must be loaded in-order; there is no automatic reference of other libraries.
> > When loaded from the launcher there is no way to set "LD\_LIBRARY\_PATH" to your application installation path; so each library must be loaded in order.

#details2
# Building Details #
More about the details of how things get built
## directory structure ##
  * ./build
    * package/
      * assets/
        * Any arbitrary files you want; only one directory level
      * libs/
        * armeabi/
          * lib.so files here...
      * res/
        * drawable-xxxx/
          * icons for project...
        * values/
          * [strings.xml](https://code.google.com/p/c-system-abstraction-component-gui/source/browse/android_build/intershell/strings.xml) (required)

  * src/
    * path/to/your/java/source/**.java
  * AndroidManifest.xml (required)
  * [build.xml](https://code.google.com/p/c-system-abstraction-component-gui/source/browse/android_build/intershell/build.xml) (required; names inside to match activity...)**

[A project source folder...](https://code.google.com/p/c-system-abstraction-component-gui/source/browse/#hg%2Fandroid_build%2Fintershell)

[CMakeLists.txt](https://code.google.com/p/c-system-abstraction-component-gui/source/browse/android_build/intershell/CMakeLists.txt) for that project... Basically after all the copied files into the correct locations...
I do a configure file on a stub class that has a name that gets substituted so I can have multiple launcher icons in a single activity....


```
# cleanup old sources to make sure it gets configured

EXECUTE_PROCESS(COMMAND cmake -E remove_directory package/src )

set( application InterShell )
set( activity EditOptions )
CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/../MyNativeActivity.cm package/src/org/d3x0r/${application}/${activity}/${activity}.java )

set( activity InterShell_console )
CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/../MyNativeActivity.cm package/src/org/d3x0r/${application}/${activity}/${activity}.java )

# This is the important part; it takes the AndroidManifest.xml 
# and the build.xml and produces more android-project stuff.
# it is safe to re-run this same command after already inited
# with this command...
#   <android.bag update project --target [] --rpath .>
EXECUTE_PROCESS( 
       COMMAND android.bat update project --target "android-14" --path .
       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/package )

# Then actually build the package
#   <ant.bat> <debug/release>
# probably RelWithDebInfo target won't work... but debug and release will
EXECUTE_PROCESS( 
       COMMAND ant.bat CMAKE_BUILD_TYPE 
       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/package )

# at this point there's package... so here's rules to install/uninstall

    add_custom_target( install 
    			COMMAND echo adb install command...
                     COMMAND adb install package/bin/${Project}-${CMAKE_BUILD_TYPE}.apk
 )

    add_custom_target( uninstall 
    			COMMAND echo adb uninstall command...
                     COMMAND adb uninstall org.d3x0r.sack.${Project}
 )

# 
cmake . # make package
make uninstall install #remove old, install new

```


And then I change it to run 'make package' instead of 'cmake .'; which was much cleaner...

```
    add_custom_target( package
       COMMAND android.bat update project --target "android-14" --path .
       COMMAND ant.bat ${CMAKE_BUILD_TYPE} 
       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/package
   )
```



Java activity to overload generic class to a custom name for multiple entry points...  Package name to change as required of course....
```
package org.d3x0r.sack.@application@;

import android.app.NativeActivity;
import android.util.Log;

public class @activity@ extends NativeActivity {
// think I might have to catch landscape event here for some android revisions
  private static String TAG = "@activity@";
  public @activity@() {
    super();
  }
}
```