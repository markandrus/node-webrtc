#ifndef _RTCSESSIONDESCRIPTIONINIT_H_
#define _RTCSESSIONDESCRIPTIONINIT_H_

#include <nan.h>
#include <v8.h>

namespace node_webrtc {

namespace RTCSessionDescriptionInit {

  struct RTCSessionDescriptionInit {
    std::string type;
    Nan::Maybe<std::string> sdp = Nan::Nothing<std::string>();
  };

  bool fromNode(
    v8::Local<v8::Value> unparsed,
    RTCSessionDescriptionInit& descriptionInit,
    std::string* error);

  v8::Local<v8::Object> toNode(RTCSessionDescriptionInit* descriptionInit);

}

}

#endif  // _RTCSESSIONDESCRIPTIONINIT_H_
