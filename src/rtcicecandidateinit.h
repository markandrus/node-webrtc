#ifndef _RTCICECANDIDATEINIT_H_
#define _RTCICECANDIDATEINIT_H_

#include <nan.h>
#include <v8.h>
#include "talk/app/webrtc/peerconnectioninterface.h"

namespace node_webrtc {

namespace RTCIceCandidateInit {

  struct RTCIceCandidateInit {
    std::string candidate;
    Nan::Maybe<std::string> sdpMid = Nan::Nothing<std::string>();
    Nan::Maybe<uint32_t> sdpMLineIndex = Nan::Nothing<uint32_t>();
  };

  bool fromNode(
    v8::Local<v8::Value> unparsed,
    RTCIceCandidateInit& candidateInit,
    std::string* error);

  v8::Local<v8::Object> toNode(RTCIceCandidateInit* candidateInit);

}

}

#endif  // _RTCICECANDIDATEINIT_H_
