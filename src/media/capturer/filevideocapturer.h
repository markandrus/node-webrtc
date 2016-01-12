#ifndef _FILEVIDEOCAPTURER_H_
#define _FILEVIDEOCAPTURER_H_

#include <nan.h>
#include <v8.h>

#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/videosourceinterface.h"
#include "talk/media/devices/filevideocapturer.h"

#include "media/recorder/mimetype.h"

namespace node_webrtc {

// TODO(mroberts): We need some way to signal "stopped".
class FileVideoCapturer
: public Nan::ObjectWrap
{
 public:
  enum CapturingState {
    kInactive,
    kRunning
  };

  static Nan::Persistent<v8::Function> constructor;
  static void Init(
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory,
    v8::Handle<v8::Object> exports);
  static NAN_METHOD(New);
  static void Run(uv_async_t* handle, int status);

  static NAN_PROPERTY_GETTER(GetFilename);
  static NAN_PROPERTY_GETTER(GetIgnoreFrameRate);
  static NAN_SETTER(SetIgnoreFrameRate);
  static NAN_PROPERTY_GETTER(GetMimeType);
  static NAN_PROPERTY_GETTER(GetRepeat);
  static NAN_SETTER(SetRepeat);
  static NAN_PROPERTY_GETTER(GetState);
  static NAN_METHOD(CaptureStream);
  static NAN_METHOD(Start);
  static NAN_METHOD(Stop);

 private:
  FileVideoCapturer(
    std::string filename,
    cricket::FileVideoCapturer* capturer,
    MimeType mime_type);

  static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;

  uv_async_t async_;
  cricket::VideoFormat capture_format_;
  cricket::FileVideoCapturer* capturer_;
  std::string filename_;
  bool ignore_framerate_ = true;
  uv_mutex_t lock_;
  uv_loop_t* loop_;
  MimeType mime_type_;
  int repeat_ = 200;
  CapturingState state_ = CapturingState::kInactive;
  rtc::scoped_refptr<webrtc::VideoSourceInterface> video_source_;
};

}

#endif  // _FILEVIDEOCAPTURER_H_
