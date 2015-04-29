# Library Asset Inventory #

This is a description of the library contents and peices.  It attempts to provide a breakdown of peices and their sizes.


# Details #


```
(Build 09/05/13, release output, modular)
       46,592 bag.video.dll
      236,032 bag.psi.dll
      246,784 bag.image.dll
      258,560 bag.psi++.dll
      406,528 bag.dll
      452,608 bag++.dll
    1,371,648 bag.externals.dll


(Build 09/05/13, debug output, modular)
        126,464 bag.video.dll
        401,920 bag.image.dll
        548,864 bag.psi.dll
        582,144 bag.psi++.dll
        934,400 bag.dll
        994,304 bag++.dll
      2,356,224 bag.externals.dll
```

Modular build splits common external libraries (zlib, png, jpeg, sqlite, freetype, expat) into bag.externals, and the core code into bag(basic aggregate group).


Monolithic build (sack\_bag) includes all of these (sack\_bag++ contains bag++, bag.psi++); some sources like sqlite are only built into the C version, and linked to from the ++ version for use.

```
(Build 09/28/2012, release output, monolithic)
        1,150,976 sack_bag++.dll
        2,797,056 sack_bag.dll
```


The breakdown of bag (it is a lot larger as object files)

```
(Build 09/05/13, release output, modular, bag)
          732 oswin.obj
        1,087 args.obj
        1,833 client_local.obj
        2,452 familytree.obj
        3,056 html5.canvas.obj
        3,190 whois.obj
        3,214 guid.obj
        3,294 sqlite_interface.obj
        3,535 sha1.obj
        3,817 listhids.obj
        3,914 fractions.obj
        3,917 net_winsock2.obj
        4,034 md5c.obj
        4,094 optionutil_v4.obj
        4,097 sqlwrap.obj
        4,899 optionutil_new.obj
        4,909 filescan.obj
        5,345 pathops.obj
        5,441 memory_operations.obj
        5,699 idle.obj
        5,784 client_client.obj
        5,796 construct.obj
        6,149 optionutil.obj
        6,229 input.obj
        6,260 listports.obj
        7,004 client_events.obj
        7,411 charProps.obj
        7,544 service_stub.obj
        7,749 client_input.obj
        8,352 client_output.obj
        8,555 client_service.obj
        9,164 sets.obj
        9,859 deadstart_core.obj
       10,590 url.obj
       11,141 udpnetwork.obj
       11,800 spawntask.obj
       12,358 ping.obj
       12,440 getoption_v4.obj
       12,601 html5.websocket.obj
       12,642 binarylist.obj
       14,205 msgqueue.obj
       14,949 getoption_new.obj
       15,149 allfiles.obj
       17,124 windowsfiles.obj
       18,556 typecode.obj
       19,138 client_common.obj
       20,214 tcpnetwork.obj
       21,683 http.obj
       21,975 sqlparse3.obj
       23,762 winfiles.obj
       25,777 sackcomm.obj
       27,054 spacetree.obj
       27,877 timers.obj
       30,652 genx.obj
       31,508 syslog.obj
       31,996 sharemem.obj
       33,016 system.obj
       36,004 vectlib.obj
       43,858 network.obj
       46,731 sqlutil.obj
       47,002 text.obj
       49,286 names.obj
       51,611 getoption.obj
       54,917 configscript.obj
       95,985 sqlstub.obj
     1,072,016 bytes

(Build 09/05/13, release output, modular, bag.image)
           439 image_client.obj
         1,887 sprite_common.obj
         2,902 line.obj
         3,031 jpgimage.obj
         3,797 pngimage.obj
         4,320 bmpimage.obj
         7,133 gifimage.obj
         7,430 interface.obj
        11,288 sprite.obj
        12,556 math.obj
        14,572 font.obj
        19,434 blotdirect.obj
        20,483 blotscaled.obj
        21,218 image.obj
        27,946 lucidaconsole2.obj
        31,059 fntrender.obj
        32,340 fntcache.obj
        66,077 alphatab.obj
        66,087 alphastab.obj
        353,999 bytes

(Build 09/05/13, release output, modular, bag.video)
         1,892 key.obj
         5,911 keydefs.obj
        10,824 opengl.obj
        85,949 vidlib.obj
        104,576 bytes

(Build 09/05/13, release output, modular, bag.psi)
           431 paste.obj
           481 calender.obj
         2,521 console_block_writer.obj
         3,170 psicon_interface.obj
         3,296 option_frame.obj
         3,711 ctlmisc.obj
         4,768 regaccess.obj
         6,080 fileopen.obj
         7,303 ctltext.obj
         8,253 xml_load.obj
         8,990 ctlslider.obj
         9,917 clock.obj
         9,997 WinLogic.obj
        10,056 scrollknob.obj
        10,590 xml_save.obj
        12,070 control_physical.obj
        12,480 ctlscroll.obj
        12,859 systray.obj
        15,556 popups.obj
        15,641 ctlsheet.obj
        15,812 analog.obj
        16,903 borders.obj
        18,946 ctledit.obj
        20,078 fntdlg.obj
        21,839 psicon.obj
        22,572 ctlbutton.obj
        22,683 ctlprop.obj
        27,059 history.obj
        27,975 ctllistbox.obj
        31,090 fntcache.obj
        35,463 mouse.obj
        37,286 palette.obj
        45,862 console_keydefs.obj
       112,873 controls.obj
        614,611 bytes



(Build 09/05/13, debug output, modular)
       11,148 sha1.obj
       11,504 familytree.obj
       14,879 md5c.obj
       17,719 sqlwrap.obj
       17,844 oswin.obj
       20,442 charProps.obj
       23,148 args.obj
       24,239 whois.obj
       24,620 sqlite_interface.obj
       24,693 client_local.obj
       25,696 guid.obj
       27,980 net_winsock2.obj
       28,186 html5.canvas.obj
       28,408 fractions.obj
       29,510 construct.obj
       30,114 idle.obj
       31,433 listhids.obj
       31,789 filescan.obj
       32,735 optionutil_v4.obj
       33,115 optionutil_new.obj
       34,033 client_events.obj
       34,303 client_client.obj
       34,333 client_input.obj
       34,882 memory_operations.obj
       35,151 pathops.obj
       35,335 binarylist.obj
       35,753 optionutil.obj
       36,618 listports.obj
       36,853 input.obj
       36,904 deadstart_core.obj
       37,285 service_stub.obj
       37,459 url.obj
       40,279 client_service.obj
       40,460 ping.obj
       42,374 sets.obj
       42,671 client_output.obj
       42,678 getoption_new.obj
       44,983 html5.websocket.obj
       45,120 spawntask.obj
       45,426 udpnetwork.obj
       47,434 msgqueue.obj
       47,805 getoption_v4.obj
       50,231 allfiles.obj
       53,361 windowsfiles.obj
       53,539 sqlparse3.obj
       56,348 client_common.obj
       62,633 genx.obj
       63,188 http.obj
       63,665 typecode.obj
       68,655 tcpnetwork.obj
       68,910 sackcomm.obj
       71,071 syslog.obj
       75,395 winfiles.obj
       76,637 timers.obj
       79,594 system.obj
       83,772 spacetree.obj
       88,553 sharemem.obj
       93,926 vectlib.obj
      103,143 sqlutil.obj
      107,565 network.obj
      109,999 text.obj
      116,194 names.obj
      128,854 getoption.obj
      130,699 configscript.obj
      186,074 sqlstub.obj

(Build 09/05/13, debug output, modular, bag.image)
     5,616 image_client.obj
    14,565 pngimage.obj
    15,617 line.obj
    19,389 math.obj
    25,376 sprite_common.obj
    29,230 jpgimage.obj
    30,413 bmpimage.obj
    30,723 interface.obj
    35,514 gifimage.obj
    42,263 font.obj
    43,803 lucidaconsole2.obj
    45,591 lucidaconsole.obj
    52,936 sprite.obj
    64,083 blotdirect.obj
    64,087 blotscaled.obj
    67,650 alphatab.obj
    67,660 alphastab.obj
    67,872 image.obj
    87,018 fntrender.obj
   108,887 fntcache.obj

(Build 09/05/13, debug output, modular, bag.video)
      27,230 key.obj
      35,611 keydefs.obj
      48,113 opengl.obj
     212,588 vidlib.obj

(Build 09/05/13, debug output, modular, bag.psi)
        2,020 paste.obj
       21,934 calender.obj
       27,508 option_frame.obj
       29,448 console_block_writer.obj
       30,505 ctlmisc.obj
       30,542 psicon_interface.obj
       31,878 regaccess.obj
       35,348 fileopen.obj
       40,023 ctltext.obj
       41,335 xml_save.obj
       41,630 ctlslider.obj
       42,013 xml_load.obj
       42,450 scrollknob.obj
       43,948 clock.obj
       47,268 systray.obj
       50,924 ctlscroll.obj
       52,826 WinLogic.obj
       54,109 control_physical.obj
       54,268 analog.obj
       57,363 popups.obj
       58,165 ctlsheet.obj
       58,791 fntdlg.obj
       61,167 console_keydefs.obj
       62,073 ctlprop.obj
       64,171 psicon.obj
       64,294 ctledit.obj
       71,435 borders.obj
       72,733 ctlbutton.obj
       84,076 palette.obj
       89,047 history.obj
       89,297 mouse.obj
       89,547 ctllistbox.obj
      105,149 fntcache.obj
      240,141 controls.obj


```