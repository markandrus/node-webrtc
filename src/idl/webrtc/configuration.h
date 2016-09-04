#ifndef SRC_IDL_WEBRTC_CONFIGURATION_H_
#define SRC_IDL_WEBRTC_CONFIGURATION_H_

#include "webrtc/api/peerconnectioninterface.h"

#include "src/idl/convert.h"
#include "src/idl/either.h"
#include "src/idl/rtcconfiguration.h"

namespace node_webrtc {

template <>
struct Convert<webrtc::PeerConnectionInterface::RTCConfiguration, RTCConfiguration> {
  static Either<std::string, webrtc::PeerConnectionInterface::RTCConfiguration> From(RTCConfiguration);
  static RTCConfiguration To(webrtc::PeerConnectionInterface::RTCConfiguration);
};

}  // namespace node_webrtc

#endif  // SRC_IDL_WEBRTC_CONFIGURATION_H_
