# Introduction #

Okay, so I have this thing, what does it do?


# Details #


## What is it that I'm seeing? ##
(can I include images?) [(click to see screenshot)](https://drive.google.com/file/d/0B812EYiKwtkkdjctejQ3OTZYRWs) not really; have to work on that.

1) There is a MDI (Multiple document interface) interface that opens with a single console.  This object is MOOSE; or Master Operator Of System Entities.  A few words appear that are maybe helpful.  This has come into existance by the script 'Macros' in bin/scripts.  There is a "bin/user.macros" file which is just an empty holding spot really.  It does do some of the sample configuration to get this demonstration up and running.
There is also a 3D view that has part of the corner of a cube (maybe, I found there are issues with aspect ratio not rendering the same).  You can toggle between views with alt-tab to bring to front.  I have very limited window control myself because I expected the operating system to do it for me; so implementation is very conservative. (IE.  It's sort of bugged and could use some help).  This view has two modes; one, the default mode, the cursor acts as a ray from the point of view into the scene, and is detected against the surfaces and mouse clicks go to render surfaces in the display; and two, if you press scroll-lock or F12 the view mode toggles and then the mouse is anchored to the center, and moving the mouse is a rotation.
  * W key - moves forward (w/ shift moves backward)
  * S key - moves down (w/ shift moves up)
  * A key - moves left
  * D key - moves right
  * Q key - rolls left
  * E key - rolls right
  * mouse rotates
  * F12 or Scroll-lock to exit rotation mode
  * Alt-F4 to exit

There are some edit features on the objects, but are currently broken... If a "thing" doesn't have a key callback, then key events should get issued as repeated state mouse events; which allows a single input-event entry point... the 3d View doesn't dispatch keys as mouse events correctly.

You can extend the functionality by typing "/plugin intershell.core" which will load into the 3d space a full screen user interface platform.  Focusing the 3D view again, you can use alt-C to enter a configuration mode.
  * First you should right click on the blank space, go to 'other properties' and select 'Edit Plugins'.  In the dialog the first thing is a listbox, the next is a text field, type in the text field "@/plugins/`*`.isp", then click Add Plugin, then click Add System, then click Okay.  [(click to see dialog with circles and arrows)](https://drive.google.com/file/d/0B812EYiKwtkkamxoR3NXREtmelk)
  * To exit edit mode, press Escape.
  * You can left-click drag to select an area on the background.  In this area (blue) you can click and create a control.  A good example would be to right-click in a area, "Create Other" -> "Quit Application".  This will mark the area for that control in green.  This area may be left-click and dragged to re-position it.  The corners may be left-click-dragged to resize the control.  Now right click in the green area gives a different popup menu allowing to edit the control specific properties, general properties, clone or destroy the control.
    * 'Edit' - A control like a button may have function specific configurations; buttons for a calculator program for instance, my have a way to configure what math function they perform... but they may still be a common button control...
    * 'Edit General' - A control has general properties; like a button, that has color, font, text (which may or may not matter, if the button's function overrides the text).
      * Text labels may however contain variables that allow for dynamic substitution without modifying the static caption...

  * Restart after exiting configuration mode;  If, for some reason, you are unable or do not leave configuration mode, the changes you made will not be saved, and it will revert to the prior state.  Changes are kept in a configuration file in `"Resources/<programname>.config"` and in the option database.

Option are by default kept in a local sqlite database.  Support for MySQL ODBC, MS Access ODBC(sqlite preferred in this case), and generic... there are features internally that handle escaping for appropriate database personalities but only a few have been tested and filled in properly; There is a tiny bit of postgresql support.

## What else can I do? ##
The Dekware console has a lot of builtin commands, and other commands will be available from objects...

/help

This first command lists all commands.  Console support infinite backscroll (limited by available memory; can overflow to page file with little issue, only what is currently viewed +1 is accessed... until it is seen)  Page-up/page-down keys are bound to scroll through history.  If the history portion of the console is too small, you can right click on the console and set the history window size as 25, 50, 75 or 100%.  at 100% you get no active display... and to see what is currently being added to the console you will have to use the End key or repeated pg-down until the end of history; at which time the display mode will not be history, and will be the full display... Otherwise you can keep history open (50% is a good setting).

[Dekware homepage](http://dekware.sourceforge.net/) More details can be found on this page.]

## That's good and all, but what can I do **SIMPLY** ##
Oh, well, you can play cards...

```
# indicates a comment line and may be omited or copied
/make deck cards
/inv 
# should now see you contain an object called 'deck'
/deck/methods
# shows what the deck of cards can do above all commands in /help
/deck/shuffle
# probably no fun to start with a brand new -off-the-shelf stacked deck of cards... so shuffle.

# probably need somewhere to deal cards to... so create some players 'a','b', 'c', and 'd'
/create a b c d

#A deck needs objects to be aware, and able to process commands 
/wake a b c d

# deal 5 cards to each player
/deck/dealto a b c d
/deck/dealto a b c d
/deck/dealto a b c d
/deck/dealto a b c d
/deck/dealto a b c d

#show the player's hand and various stats
/vvars a

/vvars b
#....

# gather up all the cards, and shuffle the deck.
/deck/shuffle

```

## Intershell.Core ##
When you reloaded, the intershell core display itself did not come back; but you can still type `/plugin intershell.core` and it will reload with your configuration.  After the first reload, and assuming all went well with adding all plugins... All the default plugins for intershell have the extension of '.isp' it has so far not been a conflict... other than some dialers.

You can edit the user.macros script (text file) in the `./bin` directory and add `/plugin intershell.core` at the end.  You can even modify other contents there.  One of the other objects that is there is the brainboard that appears.

## Objects and brains ##
`/make test "point label"`

This command will create a new 3D object.  This object has methods, and volatile variables associated with it...  (wait that's a new term, Voltaile varaible... listed with `/VVARS <object>`, if no object is specified, lists this(you) object.

There is a text label associated with each cube; for now it renders badly... this can be changed with `/test/set text "new text string"`.  The position may be set either by issuing a move command... ` /test/move 20 20 10 ` or by updating its volatile variable position ` /test/set position 20 20 15`.

The brain board also allows controlling the motion of the cube.  If you right click on the electric-blue grid, you get a menu that allows controlling properties of that brain.
  * Add Neuron - This adds a simple neuron object at the curernt cursor position
  * Add Component - This adds dynamically registered input/output variables from the object.
    * Object Motion
      * inputs
        * none
      * outputs
        * forward-backward
        * right-left
        * up-down
      * components
        * one
    * Object Rotation
      * inputs
        * none
      * outputs
        * around forward axis
        * around right axis
        * around up axis
      * components
        * one
    * If macros are created on the object then the brain gets outputs to trigger the macro;  give it a level greater than 0 to start.  The run must go back to 0 and go back above 0 to run again.

If you add a neuron, and the output for forward-backward.  Then you can left-click on the edge of the neuron and begin connecting a nerve from there to the output... move to the edge of the output. (and click again, if you released the mouse)
Connections may only be made at one spot of any neuron/input/output... a second attempt will be rejected.
A right-click while in a connecting mode will cancel the connection.
If you right click on a neuron, you will get the property dialog for the neuron... First, drag the slider all the way to the left.  Since the current input of the neuron is 0, then a negative threshold will result in an output for the neuron.  If the threshold is -100, then 0 is 100 above -100, and the output will be 100... this will go to the output and cause the object to move.

  * Save - works for neuron-only boards
  * load - works mostly (not sure about aligning loads with changing brainstem connections)

## Anything else?? ##
Well, it is a union of all points (A Nexus), so of course.  Old school MUD client.

When you start, the first things that are created is a "Room of Portals" which is the room you find yourself in when it start.  In the room of portals are portals such as "Blackmud" (others may or may not work as time has gone some connections have lost validitiy).  Blackmud does still work. `/enter blackmud` notice, this is a command to Dekware, to move yourself into a objects that is visible and external to you.  On entering the blackmud object, it gets an event that an object has entered, at which point it tells the entering object to connect to the MUD.  The user's object will then start connecting to blackmud.

There is still (And I swear I fixed this twice already) a bug with clearing screens in blackmud so sometimes extra page breaks happen... which will require using scrollback to see the character craetion stuff.

```
# when you get to blackmud, and actually enter the game there are some useful commands to issue when you start

display ansi
auto examine
auto gold
auto exits

# it's always safe to take gold, it doesn't weigh anything and is never cursed... so auto gold
# it's safe to look at things, so when they die, auto examine for content is useful
# it's nice to see colors, and this terminal does support ansi mode; the MUD itself will serve in VT100 mode and use cursor-positioning/form output sort of, which almost works also as ANSI and VT100 are somewhat merged... display ansi
# and it's nice to know which directions you can go from a room (that are obvious anyway, things like doors that are closed do not show)... auto exits
```

Have fun!