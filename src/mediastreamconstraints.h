#ifndef _MEDIASTREAMCONSTRAINTS_H_
#define _MEDIASTREAMCONSTRAINTS_H_

#include <v8.h>
#include <nan.h>

namespace node_webrtc {

namespace MediaStreamConstraints {

  struct MediaStreamConstraints {
    Nan::Maybe<bool> audio = Nan::Nothing<bool>();
    Nan::Maybe<bool> video = Nan::Nothing<bool>();
  };

  bool fromNode(
    v8::Local<v8::Value> unparsed,
    MediaStreamConstraints& constraints,
    std::string* error);

}

}

#endif  // _MEDIASTREAMCONSTRAINTS_H_
