# Introduction #

This is a description of the options for VLC\_Hook interface.

# Details #

VLC Interface is currently supporting only VLC 1.1 versions.  1.0 MAY work, but no longer compatible with 0.9 or lower.

Using EditOptions, options for where VLC is installed may be specified.  The default is to use the 'current directory/vlc'

Options for VLC are specified under

```
video.ini
    vlc
       config
          log timing=
          log verbosity=
          vlc config=
          vlc path=
```

'log timing' controls whether timing is logged to the file for when frames are locked - used to debug latency.
'log verbosity' is an option passed to VLC to control how much logging it generates, valid values for this are 0,1 and 2.
'vlc config' specifies where the vlc.cfg file is for other options which are not specified by code.  (having this point at no file is OK).
'vlc path' this is the directory that has VLC installed into it.  (c:\program files\vlc?) (c:\tools\vlc-1.1.8)  (c:\tools\vlc-1.1.4).



Add your content here.  Format your content with:
  * Text in **bold** or _italic_
  * Headings, paragraphs, and lists
  * Automatic links to other wiki pages