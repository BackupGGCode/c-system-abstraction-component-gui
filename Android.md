# Introduction #

I spent a little time (few hours) getting sack to build with adroid NDK; had only a couple issues like no semtimedop(); not sure about the proper thing to use for InterlockedExchange(); have some thumb assembly with interrupt locks, but that's always dangerous to have assembly.

But this should bring opengl sack layers to android, still have to fix the render layer, right now it's mostly bypassed by ifndef WIN32.


# Details #

toolchain file for cmake 2.8.3 in ./a2\_toolchain.txt.  This has a root path to the android NDK install; I should have grabbed the 5.1 and used that... so this will probably change.