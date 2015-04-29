# Introduction #

New alternative display renderer.  This Headless display serves for applications that by default do not have access to a display.  At some point later one or more clients can connect to the application and provide a display.  Commands issued by the server are buffered by the display service, so when a client connects, the application doesn't have to be notified of the connection.

A usable thing to connect for a display is an HTML5 capable browser which can run some javascript.  JSON included for messages, canvas for output, and websocket to connect.  Chrome, IE, Firefox, Opera are known to work; default android browser is known not to work.

client is in core/bin/webclient of built output


# Details #

```

Websocket host which displays connect to...
    src/vidlib/proxy/server.c  

Websocket client; connect to a server and display on a native interface... like opengl/direct3d3...
    src/vidlib/proxy/client.c  

Javascript to connect to a program and render its display.
requires: json2,jquery also in that directory...
    src/vidlib/webclient/render_client.js
```