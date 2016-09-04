#ifndef SRC_IDL_WEBRTC_ICESERVERS_H_
#define SRC_IDL_WEBRTC_ICESERVERS_H_

#include "webrtc/api/peerconnectioninterface.h"

#include "src/idl/rtciceserver.h"

namespace node_webrtc {

template <>
struct Convert<webrtc::PeerConnectionInterface::IceServers, std::vector<RTCIceServer>> {
  static Either<std::string, webrtc::PeerConnectionInterface::IceServers> From(std::vector<RTCIceServer>);
  static std::vector<RTCIceServer> To(webrtc::PeerConnectionInterface::IceServers);
};

}  // namespace node_webrtc

#endif  // SRC_IDL_WEBRTC_ICESERVERS_H_
