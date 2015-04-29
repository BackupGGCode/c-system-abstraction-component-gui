# Introduction #

System Abstraction Component Kit


# Details #

SACK is the old name of it. It started with logging services, and feature extensions.

I keep copies of external libraries png, zlib, jpeg, and freetype, each not that big themselves (big being relative for sure), but then providing them as services for windows without carrying around additional DLLS.

The latest version of sack builds as sack-bag, and sack-bag++.  This contains all core services ready for consumption.  This can be paired down and many peices extracted fairly easily to stand alone.

This was started long before I had CVS that worked at all under windows, and a good chunk of its history has been erased, since some of this code dates back at least to 1996.  Kind of wish for bragging rights I had been more into standard headers at least with change log notes.

All in all, this extension library is very stable in its self, and the fault will inevitably lie in the user's application.  The library is fairly robust, handling NULL in most places and doing harmless things.

The library's basic design philosophy is to allow the developer to do what he wants when he wants.  Its design is also based on event notification using callbacks.  The user may override callbacks as required by the application, and none further.

There are only a few places where it can be said that it is not thread safe.  Using a SQL database connection, it is not safe without the developer handling it, to use the same connection at the same time in two threads for two different queries.

The latest evolutions of code now rely heavily on a procedure registry of sorts.  It maintains a tree of named events that can be discovered.  When a new control is created, for instance, the methods that the user registers to receive are stored in this tree in well named places, but there are macros that make this faily painless.  This ease doesn't come without some pain, however, since it leads to having to declare things slightly oddly.  Sample control at [PSI](http://sack.sourceforge.net/sack__PSI.html).