#ifndef _MEDIASTREAMTRACK_H_
#define _MEDIASTREAMTRACK_H_

#include <nan.h>
#include <v8.h>

#include "talk/app/webrtc/mediastreaminterface.h"
#include "webrtc/base/scoped_ref_ptr.h"

namespace node_webrtc {

class MediaStreamTrack
: public Nan::ObjectWrap
{
 public:
  MediaStreamTrack();
  MediaStreamTrack(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track);

  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> getTrack() { return this->track; }
  void setTrack(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track);

  static void Init(v8::Handle<v8::Object> exports);
  static NAN_METHOD(New);
  static Nan::Persistent<v8::Function> constructor;

  // https://www.w3.org/TR/mediacapture-streams/#mediastreamtrack
  static NAN_PROPERTY_GETTER(GetKind);
  static NAN_PROPERTY_GETTER(GetId);
  static NAN_PROPERTY_GETTER(GetLabel);
  static NAN_PROPERTY_GETTER(GetEnabled);
  static NAN_SETTER(SetEnabled);
  static NAN_PROPERTY_GETTER(GetMuted);
  static NAN_PROPERTY_GETTER(GetReadonly);
  static NAN_PROPERTY_GETTER(GetRemote);
  static NAN_PROPERTY_GETTER(GetReadyState);
  static NAN_METHOD(Clone);
  static NAN_METHOD(Stop);
  static NAN_METHOD(GetCapabilities);
  static NAN_METHOD(GetConstraints);
  static NAN_METHOD(GetSettings);
  static NAN_METHOD(ApplyConstraints);

 private:
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track;
};

}

#endif  // _MEDIASTREAMTRACK_H_
