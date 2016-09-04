#ifndef SRC_IDL_WEBRTC_RTCOFFERANSWEROPTIONS_H_
#define SRC_IDL_WEBRTC_RTCOFFERANSWEROPTIONS_H_

#include "webrtc/api/peerconnectioninterface.h"

#include "src/idl/convert.h"
#include "src/idl/either.h"
#include "src/idl/rtcansweroptions.h"
#include "src/idl/rtcofferoptions.h"

namespace node_webrtc {

template <>
struct Convert<webrtc::PeerConnectionInterface::RTCOfferAnswerOptions, RTCOfferOptions> {
  static Either<std::string, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions> From(RTCOfferOptions);
  static RTCOfferOptions To(webrtc::PeerConnectionInterface::RTCOfferAnswerOptions);
};

}  // namespace node_webrtc

#endif  // SRC_IDL_WEBRTC_RTCOFFERANSWEROPTIONS_H_
