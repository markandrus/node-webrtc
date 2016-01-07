#ifndef _RTCCERTIFICATE_H_
#define _RTCCERTIFICATE_H_

#include <v8.h>
#include "webrtc/base/rtccertificate.h"

namespace node_webrtc {

// TODO(mroberts): This is a stop-gap solution; really, the RTCCertificate
// should be an ObjectWrap-ed class.
namespace RTCCertificate {

  typedef v8::Object RTCCertificate;

  v8::Local<RTCCertificate> toNode(rtc::RTCCertificate* certificate);

}

}

#endif  // _RTCCERTIFICATE_H_
