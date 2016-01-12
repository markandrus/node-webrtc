FileVideoCapturer
=================

The FileVideoCapturer functionality is relatively experimental. Currently, it
only supports video and its supported MIME types are limited. Nevertheless, it
may prove a useful tool in testing WebRTC applications.

For recording MediaStreams, see [MediaRecorder](mediarecorder.md).

Example
-------

```js
var wrtc = require('wrtc');
var frames = 'input.frames';

// Construct the FileVideoCapturer
var fileVideoCapturer = new wrtc.FileVideoCapturer(frames, {
  mimeType: 'video/vnd.webrtc.capturedframe',
  repeat: -1  // i.e., forever
});

// Capture a MediaStream from the FileVideoCapturer
var mediaStream = fileVideoCapturer.captureStream();

// Start the FileVideoCapturer
fileVideoCapturer.start();
```

Supported MIME Types
--------------------

Currently, there are two supported MIME types.

### video/vnd.webrtc.capturedframe

_This is the default._

A stream of CapturedFrame headers containing information like width and
height followed by raw I420 frame data. This format is always big endian.
