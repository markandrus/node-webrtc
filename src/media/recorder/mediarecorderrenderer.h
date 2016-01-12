#ifndef _MEDIARECORDERRENDERER_H_
#define _MEDIARECORDERRENDERER_H_

#include "media/recorder/mediarecorder.h"

#include "talk/media/base/videoframe.h"
#include "talk/app/webrtc/mediastreaminterface.h"

namespace node_webrtc {

// MediaRecorderRenderer is a WebRTC VideoRenderer that simply queues up
// VideoFrames on its parent MediaRecorder; the MediaRecorder will ultimately
// be responsible for any conversion before returning video data to the user.
class MediaRecorderRenderer
: public webrtc::VideoRendererInterface
{
 public:
  MediaRecorderRenderer(
    node_webrtc::MediaRecorder* recorder,
    webrtc::VideoTrackInterface* track);

  // VideoRendererInterface implementation
  virtual void SetSize(int width, int height);
  virtual void RenderFrame(const cricket::VideoFrame* frame);

  virtual bool Start();
  virtual bool Stop();

 private:
  node_webrtc::MediaRecorder* recorder_;
  bool started_ = false;
  webrtc::VideoTrackInterface* track_;
};

}

#endif  // _MEDIARECORDERERRENDERER_H_
