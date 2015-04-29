# Introduction #

This discusses what the video service deployment process is supposed to be, so if it should fail, steps can be completed manually.

The video link system has a text file describing the video link layout.  This will be an important peice to know later.

### Terms ###
DSN - Data Source Name

# Details #

video\_link\_deploy.exe
Shortcut 'Video Link Deploy'

> - Checks ODBC connection.
    1. Reads the registry under Software/ODBC/odbcinst.ini
      * Searches for the mysql odbc driver in the list of available drivers; if it is not found, Issues a message box indicating that myodbc driver might not be installed, and stops checking the DSN.
    1. Reads the registry under Software/ODBC/odbc.ini
    * Locates a DSN by the name of 'vsrvr' default.
    * if a DSN is not found, then one is created with mostly default parameters and a server name of 'not-set'.  This will probably have be set later when the video map is processed.

> - copies proxy.service.exe to proxy\_bdata.exe and mysql.proxy.service.exe
    1. extracts mysql-5.4minimall zip into local directory
    1. extracts vlc-1.1.4.7z into local directory
> - sets the path for VLC to be 

&lt;here&gt;

/vlc-1.1.4
> - Presents a dialog to select which video map to use.  Multiple sites may be configured in parallel.  If there is only one map to choose, that one will be used; and this dialog will not show.
> - reads the map file specified.
> - Present a dialog to select which site this computer should be for this map.
    1. if the site is not selected, nothing further is done
    1. if a site is selected, then this computer will be configured according to paramters specified in the map [See video map editor.](videomapeditor.md)
    * if the current system name is not the name of the system specified in the map, the computer's name is set.
    * rewrites the system's hosts file according to information in the video system map.
    * sets some options in the option.db.
    * Updates mysql service state.  If the system is configured as the server
then the mysql.proxy.service is uninstalled (if it was installed), and mysql database service is installed and started;
otherwise the mysql.proxy.service.exe is installed.  This maintains an all-directional interface that forwards to the configured mysql server.  If the system