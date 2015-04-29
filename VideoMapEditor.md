# Introduction #

This talks about the Video Map Editor utility, which can be used to create new site maps, and configure site maps with new parameters (maybe a server's address changes).

A site is a specific location of a video service.

An example video configuration map looks like...

```
# site file defines locations of servers
#   site <name> 
#   Hostname <name> - sets the hostname of the system, also maps in 'hosts'
#   address <ip> - defines the IP address of the system (use 'dhcp' for assigned)
#   local address <ip> - specifies the internal address of the server - control programs should use this to connect
#   expected address <ip> - defines expected IP address of the system ( most dhcp is a static map, this is what our address SHOULD be)
#   mac address <hardware> - defines the hardware address of the system (may be used to override hardware)
#   serves MySQL <yes/no> - if the sql server runs on this node... then others know where also
#   MySQL Server - if not running the server, this may indicate an external server from here - implies services for others


Uses Bingoday?No


site Node 1
Hostname video_server1
address 10.31.4.1
serves MySQL Yes
MySQL Server video_server1
local address 192.168.15.1


site Node 2 
Hostname Video_server_2
address 10.31.15.5
serves MySQL No
MySQL Server video_server_1
local address 192.168.15.2

```

video\_map\_editor.exe
Shows a dialog that lists all .map files in the current resource directory.  You can select a map to edit the video servers in.  There is also a create button that will show a dialog allowing you to enter a new name.

Editing a video map will show a dialog with a list of nodes in the system and a set of buttons at the bottom that allow creation, deletion, editing of video servers; (ok is save) (cancel is no save).

The Video map editor shows fields that say

Hostname - this is the name of the server.  When video\_link\_deploy is run, and a system is selected, this will become the name of the system.  The names of all systems are updated in the windows hosts file locally in case DNS is not setup correctly for automatic updates.

server peer IP - this is the interface that the video server's link with other video servers is on.

server local IP - this is the interace that allows access on the local network.  A peer IP is a WAN sort of address, and local is the LAN side of the site - for things like the video link controller.

MySQL Server - this is the address of the mysql server.  This should be a system name.

Expected Address - ? dhcp?

MAC Address - ? dhcp?

`[`X`]` Is MySQL Server - this is a checkbox, which indicates that this system will be the server.  Which should indicate for the other systems that this system's name should be the mysql server(?).

Control Config Name - THis is the nice name of this system which is shown on user interfaces like the video link controller.  (A user friendly version of hostname)

`[`X`]` Map uses bingoday - this is an option which allows a daily schedule to be setup ahead of time.  Otherwise it works as a single state on demand video system.  (A daily schedule can cause issues if the computer's times are not synched).

(okay, cancel, okay updates fields in the current memory, otherwise all changes are discarded)


# Video Link Deploy #
This shows a dialog that lists all maps in the resource directory.  If you select a map, and click OK then a dialog shows a list of nodes defined in the system, plus additional nodes like 'link controller' which will deploy a setup of the link control software on the current computer.





# Details #

First a dialog appears allowing you to select a site map.  A site map is a text file that describes the systems that participate in the video link.  After selecting a site map, another dialog will appear listing the sites that have been defined.  At the bottom of the dialog are buttons to create a new location, edit existing locations, or to delete a location.

Locations are defined by a name, this should reflect the site name.  They also have an address; although many systems are setup as DHCP, the expected address needs to be defined; otherwise the hosts file and routing cannot be setup.