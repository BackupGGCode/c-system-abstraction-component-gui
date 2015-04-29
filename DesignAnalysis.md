# Introduction #

A walkthrough of a practical guide to object oriented design;  As if there is any other method.

# Glossary of substitutions #

  * structure; any object, class, struct, enum, list, generic, sort of thing that is a thing.
    1. function() can be a structure since it has data (local on stack) and code; which javascript (ab)uses.
    1. class
    1. dekware object; can also have a thread attached to it.
    1. common, related values grouped together to operate as an entity.

# Details #

  * Identify useful structures
    * group logical parameters into groups that relate
    * determine root structures which will require static declaration.
  * Layout structures and determine structure extensions and interfaces
    * This refers to how structures inherit or relate to other structures.
  * Implement structure and make sure structure builds; this will clear up obvious relation errors.
  * Determine interfaces to operate on structures.
    * determine methods that are abstractable.
      * An abstractable method is usually a internal event function that results as a callback to a implementation of this class.
    * Define interfaces
  * Implement functions


blah; See I knew it could be boiled down to something simple.

http://en.wikipedia.org/wiki/Object-oriented_design

http://en.wikipedia.org/wiki/Object-oriented_analysis

And none of that has anything to do with actually doing the work.

And more work that can be done to get approval for ideas before implementing them http://en.wikipedia.org/wiki/Object-oriented_modeling

# Object Relationships #
  * is-a
> > classic; object heirarchy; object is based on an existing object and has all of the parent's object plus some extra methods and data.
  * has-a
> > classic; object has a reference to an instance of another object in it.
  * is-attached-to
> > An object may be circularly linked with another object such that this object 'has' another object, but that other object is also aware of the object it is attached to. Being a top level relation any reference to any of the attachments also refers to all other attachments.
  * contains (has-a)
> > This is a reimplementation of has-a but is reflective in that contents also know their container.
  * is-contained-by
> > Reflected property of has-a.
  * implements
> > A class can be referred to by another common interface; Interfaces are method-only structures that contain no data.

## The Diamond Inheritance problem ##
http://en.wikipedia.org/wiki/Diamond_inheritance#The_diamond_problem
```
class object;
class animal:object;
class plant:object;
class lifeform: animal,plant;
```
This is a problem that boils down to 'just because you can do something doesn't mean you should'.  This will cause inherent problems that are only addressable by addition of intense algorithms to process references.

```
class object;
interface lifeform;
class plant:object,lifeform;
class animal:object,lifeform;
```
Better solution... Since what you want is the merge of the methods since the data should be protected from external access.

## Object Abstraction ##
Wherever possible details of a structure should be kept private to the methods that operate on the structure.  This allows compatibility as the interface is unlikely to change, as the functionality of an object is extended or replaced internally.

Primary design decision against C++ as a choice
  1. objects had to be exposed in headers to external code because it was external code that was responsible for allocating the size of the thing (on the stack or as an instance with 'new' operator).  This means that any change to a class requires the application to restart.  If I have to make factory methods or classes; then I might as well just use a C style 'object **o = CreateObject()' sort of syntax instead of 'object**o = new object()'.  And even C# exposes everything which is good from an open operating standpoint.

## Apples and Oranges; they're fruit! ##
```

struct entity;
void method( struct entity *_this )
{
   ... _this->entity_property ...
}

class entity {
   void method() 
   { 
      ...  this.entity_property ... 
   }
}

```

C++ is superior because.... it has inerhitance...
C has structure extension... and implements the same ideaology.

```
class object;
class entity: object;

#define New(n)  ((n*)malloc(sizeof(n)))

struct object
{
 ...
   void (*f)( void *user_data );
   void *user_data;
 ...
};
struct entity_one
{
  struct object *o;
   ... 
   ...
};
struct entity_two
{
  struct object *o;
   ... 
   ...
};

void MethodOne( void*userdata )
{
   struct entity_one *_this = (struct entity_one *)userdata;
   /* ... */
}
CreateEntityOne()
{
   struct entity_one *entity = New( struct entity_one );
   entity->o = New( struct object );
   entity->o->f = MethodOne;
   entity->o->userdata = (void*)entity;
}

void MethodTwo( void*userdata )
{
   struct entity_two *_this = (struct entity_two *)userdata;
   /* ... */
}

CreateEntityTwo()
{
   struct entity_one *entity = New( struct entity_two );
   entity->o = New( struct object );
   entity->o->f = MethodTwo;
   entity->o->userdata = (void*)entity;
}


void InvokeMethod( struct object *o )
{
   o->f( o->userdata );
}

```

The above code creates two different entities that both extend 'object', and using the method 'f' in 'object' will result in custom results; this is like virtual; but it's also like interface depending on how the initializers are configured.

## Proper use of global space ##
C and C++ both allow declarations in global space that is shared between all things; but other modules cannot know that that exists.  C# solves this by not allowing property declarations outside of a class.

```
/* file global.h */
#ifndef MAIN_SOURCE
extern  // not the declaration of the instance....
#endif
struct module_name_global
{
   // declare global variables here... 
   // this allows later extension to create an instance of global
   // and a find and replace of 'g.' to 'g->' can become an operator
   // on multiple instances of the global data.
   PLIST clients;
} module_name_global;

struct module_name_global
{
   PLIST clients;
} *module_name_global_instance;


#define g module_name_global

```

The global structure can be trunctated to the variable 'g' for common usage, and brevity.  This conflicts with routines like MakeColor(r,g,b) which would have the 'g' substituted badly.

```
/*** file code.c ****/
#define MAIN_SOURCE /* defined in ONE source file; maybe global.c*/
#include "global.h"

int f( void )
{
   INDEX idx;
   PCLIENT client;
   LIST_FORALL( g.clients, idx, PCLIENT, client )
   {
      /* send information to connected clients */
   }
}

```

Example of usage of global...
If global.c is used it would be a simple `#define MAIN_SOURCE` and `#include "global.h"` and no other content.


```
/*** file module.c ***/

static struct module_name_local
{
    /* local data for this module, that's not used by other code */
} l;

```

The same idea can be used to give each source file a 'l' (local) structure to compliment the 'g' structure.

Even in C#, a `static struct ApplicationCommon` ; space is useful to maintain all of the common definitions of data for forms and other operation parameters.

# Dekware Objects #
Methods are associated with an object.

Methods are available to all objects within and attached to the object with the method contained.

Data is associated with an object.

A object may aquire a sentience, which is a thread, which is a command input stream, output stream, and a macro state processor.