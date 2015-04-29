# Introduction #

Discovery of how this works....



# Details #

Recently migrating from Source Forge.  There is currently a good deal of documents there at [Sack Sourceforge page](http://sack.sf.net) **or** and probably better [General overview](http://sack.sourceforge.net/sack.html).

Although this is shown as C++ with namespaces, it is entirely C; however, instead of adding 'extern "C" {}' around my header prototype definitions, I wrote 'namespace XXX {}'.  And make sure that everything actually compiled in C++.  But, where there's C++ there is CLR, and this also builds entirly managed, and can define 'public ref class'es that can be used to hook into C#.  The application display framework can be hosted as a control in C# applications.

To be filled in...

There is a lot I can say about this, and this is just one peice of a larger puzzle.  Internally this is really like 15 projects, each of which could have their own site.