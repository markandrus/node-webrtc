#ifndef SRC_IDL_WEBRTC_SESSIONDESCRIPTIONINTERFACE_H_
#define SRC_IDL_WEBRTC_SESSIONDESCRIPTIONINTERFACE_H_

#include "v8.h"

#include "webrtc/api/jsep.h"

#include "src/idl/convert.h"
#include "src/idl/either.h"
#include "src/idl/optional.h"
#include "src/idl/rtcsessiondescriptioninit.h"

namespace node_webrtc {

template <>
struct Convert<const webrtc::SessionDescriptionInterface*, RTCSessionDescriptionInit> {
  static Either<std::string, const webrtc::SessionDescriptionInterface*> From(RTCSessionDescriptionInit);
  static RTCSessionDescriptionInit To(const webrtc::SessionDescriptionInterface*);
};

template <>
struct Convert<webrtc::SessionDescriptionInterface*, RTCSessionDescriptionInit> {
  static Either<std::string, webrtc::SessionDescriptionInterface*> From(RTCSessionDescriptionInit);
  static RTCSessionDescriptionInit To(webrtc::SessionDescriptionInterface*);
};

}  // namespace node_webrtc

#endif  // SRC_IDL_WEBRTC_SESSIONDESCRIPTIONINTERFACE_H_
