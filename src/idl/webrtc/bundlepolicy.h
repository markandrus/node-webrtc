#ifndef SRC_IDL_WEBRTC_BUNDLEPOLICY_H_
#define SRC_IDL_WEBRTC_BUNDLEPOLICY_H_

#include "webrtc/api/peerconnectioninterface.h"

#include "src/idl/convert.h"
#include "src/idl/either.h"
#include "src/idl/rtcbundlepolicy.h"

namespace node_webrtc {

template <>
struct Convert<webrtc::PeerConnectionInterface::BundlePolicy, RTCBundlePolicy> {
  static Either<std::string, webrtc::PeerConnectionInterface::BundlePolicy> From(RTCBundlePolicy);
  static RTCBundlePolicy To(webrtc::PeerConnectionInterface::BundlePolicy);
};

}  // namespace node_webrtc

#endif  // SRC_IDL_WEBRTC_BUNDLEPOLICY_H_
