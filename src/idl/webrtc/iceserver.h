#ifndef SRC_IDL_WEBRTC_ICESERVER_H_
#define SRC_IDL_WEBRTC_ICESERVER_H_

#include "webrtc/api/peerconnectioninterface.h"

#include "src/idl/convert.h"
#include "src/idl/either.h"
#include "src/idl/rtciceserver.h"

namespace node_webrtc {

template <>
struct Convert<webrtc::PeerConnectionInterface::IceServer, RTCIceServer> {
  static Either<std::string, webrtc::PeerConnectionInterface::IceServer> From(RTCIceServer);
  static RTCIceServer To(webrtc::PeerConnectionInterface::IceServer);
};

}  // namespace node_webrtc

#endif  // SRC_IDL_WEBRTC_ICESERVER_H_
