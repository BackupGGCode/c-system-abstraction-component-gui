Launchpad and Launch[cmd/win] is a triplet of programs that allows execution of remote processes.

# Introduction #

Launchpad runs as a server on target machines.  Launchpad has several command line parameters

-c class name to respond to.

-l enables logging

-L enables network packet receive logging

-s socket listen address (resembles send on launchcmd)

as many class names as you awnt may be added.
> If a class\_name is specified commands for specified class will be executed.
> If NO class\_name is specified all received commands will be executed.
> If no class is specified in the message, it is also launched.

Launchpad adds its machine name as a class to respond on also.

Launchcmd can send a broadcast message with no class specified; this is a command intended to run on all systems.

Launchcmd can also specify -c parameters to send to specific classes of launchpad.

Launchwin is the same client process as launchcmd, but is compiled as a windows application instead of a console application.   Using launchcmd will always cause a command prompt unless its output was redirected; using launchwin, there will be no display for the process.



# Details #