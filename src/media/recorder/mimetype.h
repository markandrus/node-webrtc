#ifndef _MIMETYPE_H_
#define _MIMETYPE_H_

#include <v8.h>

namespace node_webrtc {

  enum MimeType {
    kVideoVndWebrtcCapturedFrame,
    kVideoXRawYuv
  };

  const char* mime_type_to_string(MimeType mime_type);

  bool mime_type_from_node(v8::Local<v8::Value> unparsed, MimeType* mime_type);

}

#endif  // _MIMETYPE_H_
