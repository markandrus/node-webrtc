#ifndef _RTCSESSIONDESCRIPTION_H_
#define _RTCSESSIONDESCRIPTION_H_

#include <iostream>
#include <nan.h>
#include "rtcsessiondescriptioninit.h"
#include "talk/app/webrtc/peerconnectioninterface.h"

namespace node_webrtc {

class RTCSessionDescription
: public Nan::ObjectWrap
{
 public:
  RTCSessionDescription(
      webrtc::SessionDescriptionInterface* desc):
    desc(desc) {}

  ~RTCSessionDescription() { std::cout << "DESTRUCTING!" << std::endl; }

  static void Init(v8::Handle<v8::Object> exports);
  static NAN_METHOD(New);
  static Nan::Persistent<v8::Function> constructor;

  static RTCSessionDescription* create(
    RTCSessionDescriptionInit::RTCSessionDescriptionInit& descriptionInit,
    std::string *error);

  webrtc::SessionDescriptionInterface* getSessionDescriptionInterface() { return desc; }

  //
  // RTCSessionDescription implementation
  //
  static NAN_PROPERTY_GETTER(GetSdp);
  static NAN_PROPERTY_GETTER(GetType);
  static NAN_METHOD(ToJSON);

 private:
  webrtc::SessionDescriptionInterface* desc;
};

}

#endif  // _RTCSESSIONDESCRIPTION_H_
