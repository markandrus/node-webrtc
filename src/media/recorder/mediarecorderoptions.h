#ifndef _MEDIARECORDEROPTIONS_H_
#define _MEDIARECORDEROPTIONS_H_

#include <nan.h>

namespace node_webrtc {

namespace MediaRecorderOptions {

  struct MediaRecorderOptions {
    Nan::Maybe<long> audio_bits_per_second = Nan::Nothing<long>();
    Nan::Maybe<long> bits_per_second = Nan::Nothing<long>();
    std::string mime_type;
    Nan::Maybe<long> video_bits_per_second = Nan::Nothing<long>();
  };

  bool from_node(
    v8::Local<v8::Value> unparsed,
    MediaRecorderOptions& options,
    std::string* error);

}

}

#endif  // _MEDIARECORDEROPTIONS_H_
