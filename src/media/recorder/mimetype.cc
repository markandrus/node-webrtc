#include <nan.h>

#include "media/recorder/mimetype.h"

using namespace node_webrtc;
using namespace v8;

const char* node_webrtc::mime_type_to_string(MimeType mime_type)
{
  switch (mime_type) {
    case MimeType::kVideoVndWebrtcCapturedFrame:
      return "video/vnd.webrtc.capturedframe";
    case MimeType::kVideoXRawYuv:
      return "video/x-raw-yuv";
    default:
      assert(false);  // impossible
  }
}

bool node_webrtc::mime_type_from_node(Local<Value> unparsed, MimeType* mime_type)
{
  if (!unparsed->IsString()) {
    return false;
  }

  std::string string = *(String::Utf8Value(unparsed));
  if (string == "video/vnd.webrtc.capturedframe") {
    if (mime_type != nullptr) {
      *mime_type = MimeType::kVideoVndWebrtcCapturedFrame;
    }
  } else if (string == "video/x-raw-yuv") {
    if (mime_type != nullptr) {
      *mime_type = MimeType::kVideoXRawYuv;
    }
  } else {
    return false;
  }

  return true;
}
