#ifndef SRC_IDL_WEBRTC_ICETRANSPORTSTYPE_H_
#define SRC_IDL_WEBRTC_ICETRANSPORTSTYPE_H_

#include "webrtc/api/peerconnectioninterface.h"

#include "src/idl/convert.h"
#include "src/idl/either.h"
#include "src/idl/rtcicetransportpolicy.h"

namespace node_webrtc {

template <>
struct Convert<webrtc::PeerConnectionInterface::IceTransportsType, RTCIceTransportPolicy> {
  static Either<std::string, webrtc::PeerConnectionInterface::IceTransportsType> From(RTCIceTransportPolicy);
  static RTCIceTransportPolicy To(webrtc::PeerConnectionInterface::IceTransportsType);
};

}  // namespace node_webrtc

#endif  // SRC_IDL_WEBRTC_ICETRANSPORTSTYPE_H_
