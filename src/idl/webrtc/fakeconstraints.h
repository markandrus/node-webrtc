#ifndef SRC_IDL_WEBIDL_FAKECONSTRAINTS_H_
#define SRC_IDL_WEBIDL_FAKECONSTRAINTS_H_

#include "webrtc/api/test/fakeconstraints.h"

#include "src/idl/convert.h"
#include "src/idl/either.h"
#include "src/idl/rtcofferoptions.h"

namespace node_webrtc {

template <>
struct Convert<webrtc::FakeConstraints, RTCOfferOptions> {
  static Either<std::string, webrtc::FakeConstraints> From(RTCOfferOptions);
  static RTCOfferOptions To(webrtc::FakeConstraints);
};

}  // namespace node_webrtc

#endif  // SRC_IDL_WEBIDL_FAKECONSTRAINTS_H_
