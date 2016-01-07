#ifndef _RTCICECANDIDATE_H_
#define _RTCICECANDIDATE_H_

#include <nan.h>
#include <v8.h>
#include "rtcicecandidateinit.h"
#include "talk/app/webrtc/peerconnectioninterface.h"

namespace node_webrtc {

class RTCIceCandidate
: public Nan::ObjectWrap
{
 public:
  RTCIceCandidate(webrtc::IceCandidateInterface* cand):
    cand(cand) {}

  static void Init(v8::Handle<v8::Object> exports);
  static NAN_METHOD(New);
  static Nan::Persistent<v8::Function> constructor;

  static RTCIceCandidate* create(
    RTCIceCandidateInit::RTCIceCandidateInit& candidateInit,
    std::string* error);

  webrtc::IceCandidateInterface* getIceCandidateInterface() { return cand; }

  //
  // RTCIceCandidate implementation
  //
  static NAN_PROPERTY_GETTER(GetCandidate);
  static NAN_PROPERTY_GETTER(GetSdpMid);
  static NAN_PROPERTY_GETTER(GetSdpMLineIndex);

  /* static NAN_PROPERTY_GETTER(GetFoundation);
  static NAN_PROPERTY_GETTER(GetPriority);
  static NAN_PROPERTY_GETTER(GetIp);
  static NAN_PROPERTY_GETTER(GetProtocol);
  static NAN_PROPERTY_GETTER(GetPort);
  static NAN_PROPERTY_GETTER(GetType);
  static NAN_PROPERTY_GETTER(GetTcpType);
  static NAN_PROPERTY_GETTER(GetRelatedAddress);
  static NAN_PROPERTY_GETTER(GetRelatedPort); */

 private:
  webrtc::IceCandidateInterface* cand;
};

}

#endif  // _RTCICECANDIDATE_H_
