#ifndef SRC_IDL_WEBRTC_RTCPMUXPOLICY_H_
#define SRC_IDL_WEBRTC_RTCPMUXPOLICY_H_

#include "webrtc/api/peerconnectioninterface.h"

#include "src/idl/convert.h"
#include "src/idl/either.h"
#include "src/idl/rtcrtcpmuxpolicy.h"

namespace node_webrtc {

template <>
struct Convert<webrtc::PeerConnectionInterface::RtcpMuxPolicy, RTCRtcpMuxPolicy> {
  static Either<std::string, webrtc::PeerConnectionInterface::RtcpMuxPolicy> From(RTCRtcpMuxPolicy);
  static RTCRtcpMuxPolicy To(webrtc::PeerConnectionInterface::RtcpMuxPolicy);
};

}  // namespace node_webrtc

#endif  // SRC_IDL_WEBRTC_RTCPMUXPOLICY_H_
