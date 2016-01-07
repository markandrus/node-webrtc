#ifndef _MEDIASTREAM_H_
#define _MEDIASTREAM_H_

#include <nan.h>
#include <v8.h>

#include "mediastreamtrack2.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "webrtc/base/scoped_ref_ptr.h"

namespace node_webrtc {

class MediaStream
: public Nan::ObjectWrap
{
 public:
  MediaStream();
  MediaStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);

  static void Init(
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory,
    v8::Handle<v8::Object> exports);
  static NAN_METHOD(New);
  static Nan::Persistent<v8::Function> constructor;

  // https://www.w3.org/TR/mediacapture-streams/#mediastream
  static NAN_PROPERTY_GETTER(GetId);
  static NAN_METHOD(GetAudioTracks);
  static NAN_METHOD(GetVideoTracks);
  static NAN_METHOD(GetTracks);
  static NAN_METHOD(GetTrackById);
  static NAN_METHOD(AddTrack);
  static NAN_METHOD(RemoveTrack);
  static NAN_METHOD(Clone);
  static NAN_PROPERTY_GETTER(GetActive);

  rtc::scoped_refptr<webrtc::MediaStreamInterface> getStream() { return this->stream; }
  void setStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);

 private:
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream;
};

}

#endif  // _MEDIASTREAM_H_
