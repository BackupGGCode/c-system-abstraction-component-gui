# OOP - A State of mind #

Object oriented programming is a state of mind, not a language.
But then there's Object oriented programming... some sort of high level abstraction for development of programs that's objective oriented... or have objectives... I dunno how the word got mangled... Or [Wikipedia has a horrible write-up](http://en.wikipedia.org/wiki/Object-oriented_programming).



# Objects #

So let's start with the simple; an object is a related set of data.  There are routines which do something with the data; There is a process associated with the data.  A process takes an input and generates output, and there's 'stuff' or 'information' that the process operates on.

Therefore, very little is required for a language to support objects.

# C Coding Practices #

Never declare a variable directly in global space.  By avoiding litering global namespace with variable names, you reduce the chance of collision when linkink.

Objects generally should be defined by a single source file.  Objects may have a local space where common variables that process use shall be grouped, this will be defined by a structure called 'l'.

A system of objects may interact and have common variables.  These variables shared between source files shall be defined by a structure called 'g'.

static variables in functions should be avoided, but like any tool in the toolbox may have a useful purpose like a simple thread interlock semaphore.

```
// -- common_global.h --

// if all source files include 'common_global.h' then this will provide
// the definitions from the public interface, and bias it to internal sources.
#define OBJECT_SYSTEM_SOURCE
#include <object_system_library.h>


// should redefine g as a long name, since this has potential to be a
// conflict when linking libraries.  Also during debugging, all debuggers
// have an issue if you want to explore the data 'g'.  This unfortunatly
// makes debugging a little harder, because you have to back-track to find 
// what 'g' is defined as.
#define g common_global_data_name

// one and only one source file should #define GLOBAL_SOURCE before including 
// "common_global.h"
// if not defined, then structure is 'extern' otherwise the real instance is
// allocated in the compiled code.
#ifndef GLOBAL_SOURCE
extern
#endif
       struct common_global_data
       {
           // define all global data here
           int data;  
       } g;
 
```
```
// -- common_global.c --
// a very simple C file can be maintained that does the job 
// of allocating the actual space for the global structure.
#define GLOBAL_SOURCE
#include "common_global.h"

```
```
// -- Simple_object.h --
struct system_object_simple 
{
    // object data is here
};
```
```
// -- object_system_simple_object_implementation.c
#include "common_global.h"

#define l object_system_simple_object_local_data
static struct object_system{
   // define variables that just this source needs
   // things that are common to the simple_object type itself.
} l;

void simple_object_method( PSimpleObject object )
{
    // do something with the object.
    // object->value *= 2;
}

PSimpleObject simple_object_create( void )
{
   // define a construction method/factory for making one of these objects
   
   // this is sufficient... but it is in allocated memory with unknown
   // content in the structure.
   return New( SimpleObject );
   
   PSimpleObject result;
   result = New( SimpleObject );

   // no, not really, set values for the object here..
   result->initial_values = SIMPLE_OBJECT_INITIALIZED;

   return result;
}

void simple_object_destroy( PSimpleObject object )
{
   Deallocate( PSimpleObject, object );
}

```

```
// -- object_system_library.h --
// this file defines the interface between this object system and the 
// other object systems.

typedef struct system_object_simple SimpleObject;
typedef struct system_object_simple *PSimpleObject;

#ifdef OBJECT_SYSTEM_SOURCE
#define OBJECT_SYSTEM_PROC EXPORT_METHOD
#else
#define OBJECT_SYSTEM_PROC IMPORT_METHOD

OBJECT_SYSTEM_PROC PSimpleObject simple_object_create( void );
OBJECT_SYSTEM_PROC void simple_object_destroy( PSimpleObject );
```

## Linked List(cheaper) ##
There is a construct in programming that is a linked list.  This is implemented with some data in the object that references the next object.  Although bad design in general, if the first node is the me node, the list my be treated like a double linked list.

A typical list is taught now to be a separate chain of nodes... which clutter up allocated memory, this contains the pointer within the data itself.

```
struct name {
   struct name **me;
   struct name *next;
};

struct name *list_start;
```

A routine that might add data to the list...
```
void add_node( struct name **start, struct name *node )
{
   LinkThing( start, node );

   // this is the equivalent code, the macro has checks for NULL to 
   // make its safer... if start was NULL, then dereferencing start to
   // update the head of the list would cause an exception.
   //node->next = (*start);
   //node->me = start;
   //(*start) = node;
}
```
How to remove from such a list.
```
// this removes a node in the list from anywhere in the list given just the 
// node.
void remove_node( struct name *node )
{
   UnlinkThing( node );
   // this is the equivalent code, the macro UnlinkThing() has checks to
   // protect against node being NULL and do a non-operation instead.
   //if( (*node->me) = node->next )
   //   node->next->me = node->me;
}

usage()
{
   remove_node( list_start );
}
```


```
void iterate_list( struct name *root
                 , void (*f)( PTRSZVAL, struct name *)
                 , PTRSZVAL psv )
{
    while( root )
    {  
         f( psv, root );
         root = root->next;
    } 
}

usage()
{
   iterate_list( list_start );
}

```

## C Lists part 2 ##

Linked lists are fairly efficient, they don't require other nodes to point at them.  Slab allocated lists can be efficient also, requiring slightly less memory than a separate node list, but requires additional external tracking from the object itself.  This removes the requirement that the object know it is part of a list.

This is a type defined by SACK; it has a library of common functions to operate on the list.  Some of the operations are just macros; such as the LIST\_FORALL operation.

While adding to and removing from (updating) the list is thread-safe, but reading this list is not thread-safe, and the list may be invalidated during the process, if not protected externally or by usage.

PLIST must be initialized to NULL.

```

static struct module_local
{
   PLIST list;
} l;

struct some_object
{ 
   int value;
};

void add_object( struct some_object *node )
{
   AddLink( &l.list, node );
}

void remove_object( struct some_object *node )
{
   DeleteLink( &l.list, node );
}

void iterate_list( int criteria )
{
   INDEX idx;
   struct some_object *node;
   LIST_FORALL( l.list, idx, struct some_object *, node )
   {
       if( node->value == criteria )
           break;
   }
   // if list is empty, node ends with NULL;
   // at end node == NULL;
   // if the node is not NULL, then it's value was criteria.
}


```


Add your content here.  Format your content with:
  * Text in **bold** or _italic_
  * Headings, paragraphs, and lists
  * Automatic links to other wiki pages