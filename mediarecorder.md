MediaRecorder
=============

The MediaRecorder functionality is relatively experimental and based on the
latest [MediaStream Recording Editor's Draft](http://w3c.github.io/mediacapture-record/MediaRecorder.html).
Currently, it only supports video and its supported MIME types are limited.
Nevertheless, it may prove a useful tool in testing WebRTC applications, and you
can always pipe the output to FFMPEG.

For creating MediaStreams from recordings, see [FileVideoCapturer](filevideocapturer.md).

Example
-------

```js
var wrtc = require('wrtc');
var out = 'out.yuv';
var timeslice = 250;

wrtc.getUserMedia({ video: true }).then(mediaStream => {

  // Construct the MediaRecorder
  var mediaRecorder = new wrtc.MediaRecorder(mediaStream, {
    mimeType: 'video/x-raw-yuv'
  });

  // Setup a callback to append frame data to a file
  mediaRecorder.ondataavailable = event => {
    fs.appendFile(out, event.data, {
      encoding: 'binary'
    }, error => {
      if (error) {
        console.error(error);
      }
    });
  };

  // Start the MediaRecorder requesting frame data
  mediaRecorder.start(timeslice);

}, error => {
  console.error(error);
});
```

Supported MIME Types
--------------------

Currently, there are two supported MIME types.

### video/vnd.webrtc.capturedframe

This format is intended to be used with [FileVideoCapturer](filevideocapturer.md).
It is a stream of CapturedFrame headers containing information like width and
height followed by raw I420 frame data. This format is always big endian.

### video/x-raw-yuv

_This is the default._

This format is just a stream of raw I420 frame data. In general, you should not
use this, as it doesn't encode any information about the frames (and if the
MediaStream you are capturing changes size, it will be very difficult to make
use of the recording).

You can playback video/x-raw-yuv recordings with VLC. For example, assuming
the recording out.yuv is 640x480 px at 30 fps:

```
vlc --rawvid-fps 30 \
    --rawvid-width 640 \
    --rawvid-height 480 \
    --rawvid-chroma I420 \
    out.yuv
```
