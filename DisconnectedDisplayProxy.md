# Introduction #

Proxy provides a video driver for headless programs.  This allows headless programs to create displays (no chance of failure) and to draw to them as if they had a physical connection.

# Details #

**./src/vidlib/proxy
  * client.c  (TCP connection client to apply a physical display to the server)
  * server.c (provides interface for host service/application)**


Server side hosts sockets at :4241(TCP) and :4240(WebSocket/JSON).

The headless process may create a display to render with.  This display will be kept internally if there are no connections.  When the server receives a connection, it syncronizes with the client, and creates existing renderer's in/with the client. The client returns a handle to its own representation of the renderer, so the future communication from the server to the client will be in terms of the client's handle.

## Typical Operations ##
### Get/Set Display Size ###
The server uses option database to define what the target screen is;  This allows a well-known interface to be able to specify to the option database values for the 'display' size.  Setting the display size has no operation.

### Operating of Certain things ###
Images pose a few challenges;
  1. LoadImage("Filename"); where does this come from?
  1. Fonts ; internally I had control of width and height, html5 canvas does not support text orientation, and only supports a pixel height, so width fitting will fail.
  1. 2 modes of operation exist now
    * RequiresDrawAll = TRUE; drawing updates are short circuited.  Things like UpdateDisplay and UpdateDisplayPortion don't work, and only drawing done during a draw callback is done.  The video service queries surfaces and plugins to see if anyone wants to draw; so this can be a very low frequency thing.
    * RequiresDrawAll = FALSE; drawing updates happen in two ways
      * Automatic window refresh; puts existing content to the display as required
      * On demand-by-region updates.

  1. for websocket mode; the drawing commands should be queued on a per-image basis, so when a new client connects, the application is not bothered with re-rendering, the current client just gets a copy of what all other clients also had.
    * Operations like BlatColor(), ClearImage() and ClearImageTo() (with an opaque color) or BlatColorAlpha() (with an opaque color), will reset the draw chain.  Drawing commands can be batched until the end of the drawing phase for that application; since most draw commands no procedural and not functions.

Local image availability is a must for functioning; so a image transfer mechanism can be employed...

Also fonts may end up being server-generated and transmitted to the clients.

When a client connects, a redraw needs to be done; but then again the drawing expressions should have already been cached and just need to be sent to that client...


## Fonts ##
Fonts ended up being tricky to implement; although I had most of the internal render to image and was able to update images to the remote to draw subimages from...

1) bmp images with 32 bit color sent are not transparent.
2) png images have to be sent; which required more work; but reduces the size.
3) render offset for the bitmap data for the output to an image was wrong and should have been based on a runtime option instead of code option.

## Shaded Images ##
For font output; I used to use shaded image output....