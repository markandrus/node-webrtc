#include "src/idl/webrtc/iceserver.h"

#include "src/idl/optional.h"
#include "src/idl/rtcicecredentialtype.h"

using node_webrtc::Convert;
using node_webrtc::Either;
using node_webrtc::Optional;
using node_webrtc::RTCIceCredentialType;
using node_webrtc::RTCIceServer;
using IceServer = webrtc::PeerConnectionInterface::IceServer;

Either<std::string, IceServer> Convert<IceServer, RTCIceServer>::From(RTCIceServer iceServer) {
  webrtc::PeerConnectionInterface::IceServer server;
  if (iceServer.urls()) {
    server.uri = iceServer.urls().left();
  } else {
    server.urls = iceServer.urls().right();
  }
  server.username = iceServer.username().value_or("");
  server.password = iceServer.credential().value_or("");
  return Either<std::string, webrtc::PeerConnectionInterface::IceServer>(server);
}

RTCIceServer Convert<IceServer, RTCIceServer>::To(IceServer server) {
  Either<std::string, std::vector<std::string>> urls = !server.uri.empty()
    ? Either<std::string, std::vector<std::string>>::Left(server.uri)
    : Either<std::string, std::vector<std::string>>::Right(server.urls);
  return RTCIceServer(
    urls,
    Optional<std::string>(server.username),
    Optional<std::string>(server.password),
    // NOTE(mroberts): WebRTC only supports passwords right now.
    Optional<RTCIceCredentialType>(node_webrtc::kPassword));
}
