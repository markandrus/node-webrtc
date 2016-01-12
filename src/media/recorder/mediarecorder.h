#ifndef _MEDIARECORDER_H_
#define _MEDIARECORDER_H_

#include <nan.h>
#include <uv.h>
#include <v8.h>

#include "media/mediastream.h"
#include "media/recorder/mimetype.h"
#include "media/recorder/videoframeconverter.h"

#include "talk/app/webrtc/mediastreaminterface.h"
#include "webrtc/base/scoped_ref_ptr.h"

namespace node_webrtc {

class MediaRecorderRenderer;

class MediaRecorder
: public Nan::ObjectWrap
{
 public:
  enum RecordingState {
    kInactive,
    kRecording,
    kPaused
  };

  static Nan::Persistent<v8::Function> constructor;
  static void Init(v8::Handle<v8::Object> exports);
  static NAN_METHOD(New);
  static void Run(uv_async_t* handle, int status);

  static NAN_PROPERTY_GETTER(GetStream);
  static NAN_PROPERTY_GETTER(GetMimeType);
  static NAN_PROPERTY_GETTER(GetState);
  static NAN_PROPERTY_GETTER(GetIgnoreMutedMedia);
  static NAN_SETTER(SetIgnoreMutedMedia);
  static NAN_METHOD(Start);
  static NAN_METHOD(Stop);
  static NAN_METHOD(Pause);
  static NAN_METHOD(Resume);
  static NAN_METHOD(RequestData);
  static NAN_METHOD(IsTypeSupported);

  // Queue a VideoFrame to be processed and returned in the MediaRecorder's
  // ondataavailable callback.
  void QueueFrame(cricket::VideoFrame* frame);

  RecordingState state() { return this->state_; }
  bool ignore_muted_media() { return this->ignore_muted_media_; }

 private:
  MediaRecorder(node_webrtc::MediaStream* mediastream);
  MediaRecorder(node_webrtc::MediaStream* mediastream, MimeType mimetype);

  uint8_t* ConvertAndDelete(size_t& size);
  void QueueRequestDataRequest();

  uv_async_t async_;
  uv_mutex_t lock_;
  uv_loop_t* loop_;

  VideoFrameConverter* converter_;
  std::vector<cricket::VideoFrame*> frames_;
  bool ignore_muted_media_ = false;
  node_webrtc::MediaStream* media_stream_;
  MimeType mime_type_;
  std::queue<cricket::VideoFrame*> queued_frames_;
  MediaRecorderRenderer* renderer_ = nullptr;
  RecordingState state_ = kInactive;
};

}

#endif  // _MEDIARECORDER_H_
