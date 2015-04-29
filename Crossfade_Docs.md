# Introduction #

This is about the crossfade sample project.


# Details #

'crossfade\_vid\_playlist' is a program which can take a custom playlist, and play chains of videos.  An external program 'crossfade\_vid\_trigger' can tell which chain to play.  When a trigger is received, the list is changed immediately.


crossfade\_vid\_playlist takes command line arguments preceeded by a '-' or '@'.  The first 5 arguments for '-' are x, y, width,height, default show time, default fade time.  If a new default show time or fade time is specified the internal defaults are 500 for fade and 1000 for show.  All times are given in milliseconds.

```
  crossfade_vid_playlist -50 -50 -640 -480 -2500 -250 sample1.png sample2.png
```

The above command line would open a window at 50,50 sized 640,480 and sets the show time as 2.5 seconds per image and a quarter second to fade between images.

```
  crossfade_vid_playlist -50 -50 -640 -480 -2500 -250 @myplaylist.txt
```

Same position and size as other sample, but specifies to use a playlist file which allows definition of which chain each video is in.

playlist.lst  sample included

each line is 4 values sperated by a comma

(list),(fade in delay),(show time),(filename)


(list) is which list the video is in.  The first list (list 0) will loop continuously; until another list is triggered.

(fade in delay) this is the time it will take for new image to show fully opaque.  (uses default if 0)

(show time) this is how long this image will show before changing to the next image.  (uses default show if 0)  A video clip defaults to showing until the video ends, and then begins fading the next image in.

(filename) is the name of the image to play.


Sample file content

---


```
0,0,0,c:\\ftn3000\\etc\\images\\suncoast\\Bingo Rules (0;00;10;14).jpg
0,0,0,c:\\ftn3000\\etc\\images\\suncoast\\Bingo Rules (0;00;56;09).jpg
1,0,0,c:\\ftn3000\\etc\\images\\suncoast\\Bingo Rules (0;01;31;00).jpg
1,0,0,c:\\ftn3000\\etc\\images\\suncoast\\Cash Ball.wmv
2,0,0,c:\\ftn3000\\etc\\images\\suncoast\\Even Betor Progressive.wmv
```
the first two images are in list 0, the next two images are in list 1, and the last video is in list 2.

The two in list 0 will rotate continuously until the external crossfade\_vid\_trigger program sends it a command.


---

```
crossfade_vid_trigger 1
```

Tells crossfade\_vid\_playlist to play chain 1 immeditately.  Chain 1 will show each image or play each video in the list until it ends, and then it will go back to chain 0 rotation.


```
crossfade_vid_trigger 0
```

tells crossfade to change to list 0.  This re-starts chain 0.


Add your content here.  Format your content with:
  * Text in **bold** or _italic_
  * Headings, paragraphs, and lists
  * Automatic links to other wiki pages