#include "src/idl/webrtc/iceservers.h"

#include "src/idl/webrtc/iceserver.h"

using node_webrtc::Convert;
using node_webrtc::Either;
using node_webrtc::RTCIceServer;
using IceServer = webrtc::PeerConnectionInterface::IceServer;
using IceServers = webrtc::PeerConnectionInterface::IceServers;

Either<std::string, IceServers> Convert<IceServers, std::vector<RTCIceServer>>::From(std::vector<RTCIceServer> iceServers) {
  IceServers servers;
  for (auto iceServer : iceServers) {
    Either<std::string, IceServer> server = Convert<IceServer, RTCIceServer>::From(iceServer);
    if (!server) {
      return Either<std::string, IceServers>::Left(server.left());
    }
    servers.push_back(*server);
  }
  return Either<std::string, IceServers>(servers);
}

std::vector<RTCIceServer> Convert<IceServers, std::vector<RTCIceServer>>::To(IceServers servers) {
  std::vector<RTCIceServer> iceServers;
  // TODO(mroberts): This is currently unused; either implement, or refactor
  // Convert to be unidirectional.
  return iceServers;
}
