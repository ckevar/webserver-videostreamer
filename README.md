# Webserver Video streaming

server that streams raw h264 NAL units over TCP/IP based on server-sent-events. The raw h264 units can be split it using the [h264Splitter](https://github.com/ckevar/h264-splitter).
At the client side a modified version of [wfs.js](https://github.com/ChihChengYang/wfs.js) to accept text/event-streams builds the fMP4 container to play decode and play the video using the built-in decoder of HTML 5.

## Tested
Currently, the server has been tested on Linux/pop-os!. The client side has been tested on:
* Firefox 71.0
* Chrome 79 (for android devices)
* Chrome desktop is firing some shits

Also installed on Linux/pop-os!
