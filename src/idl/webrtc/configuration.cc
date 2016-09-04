#include "src/idl/webrtc/configuration.h"

#include "src/idl/optional.h"
#include "src/idl/rtcbundlepolicy.h"
#include "src/idl/rtciceserver.h"
#include "src/idl/rtcicetransportpolicy.h"
#include "src/idl/rtcrtcpmuxpolicy.h"
#include "src/idl/webrtc/bundlepolicy.h"
#include "src/idl/webrtc/iceservers.h"
#include "src/idl/webrtc/icetransportstype.h"
#include "src/idl/webrtc/rtcpmuxpolicy.h"

using node_webrtc::Convert;
using node_webrtc::Either;
using node_webrtc::RTCBundlePolicy;
using node_webrtc::RTCConfiguration;
using node_webrtc::RTCIceServer;
using node_webrtc::RTCIceTransportPolicy;
using node_webrtc::RTCRtcpMuxPolicy;
using Configuration = webrtc::PeerConnectionInterface::RTCConfiguration;
using BundlePolicy = webrtc::PeerConnectionInterface::BundlePolicy;
using IceServers = webrtc::PeerConnectionInterface::IceServers;
using IceTransportsType = webrtc::PeerConnectionInterface::IceTransportsType;
using RtcpMuxPolicy = webrtc::PeerConnectionInterface::RtcpMuxPolicy;

Either<std::string, Configuration> Convert<Configuration, RTCConfiguration>::From(RTCConfiguration configuration) {
  Configuration config;

  Either<std::string, IceServers> servers = Convert<IceServers, std::vector<RTCIceServer>>::From(
    configuration.iceServers());
  if (!servers) {
    return Either<std::string, Configuration>::Left(servers.left());
  }
  config.servers = *servers;

  Either<std::string, IceTransportsType> type = Convert<IceTransportsType, RTCIceTransportPolicy>::From(
    configuration.iceTransportPolicy());
  if (!type) {
    return Either<std::string, Configuration>::Left(type.left());
  }
  config.type = *type;

  Either<std::string, BundlePolicy> bundlePolicy = Convert<BundlePolicy, RTCBundlePolicy>::From(
    configuration.bundlePolicy());
  if (!bundlePolicy) {
    return Either<std::string, Configuration>::Left(bundlePolicy.left());
  }
  config.bundle_policy = *bundlePolicy;

  Either<std::string, RtcpMuxPolicy> rtcpMuxPolicy = Convert<RtcpMuxPolicy, RTCRtcpMuxPolicy>::From(
    configuration.rtcpMuxPolicy());
  if (!rtcpMuxPolicy) {
    return Either<std::string, Configuration>::Left(rtcpMuxPolicy.left());
  }
  config.rtcp_mux_policy = *rtcpMuxPolicy;

  return Either<std::string, Configuration>(config);
}

RTCConfiguration Convert<Configuration, RTCConfiguration>::To(Configuration config) {
  return RTCConfiguration(
    Convert<IceServers, std::vector<RTCIceServer>>::To(config.servers),
    Optional<RTCIceTransportPolicy>(Convert<IceTransportsType, RTCIceTransportPolicy>::To(config.type)),
    Optional<RTCBundlePolicy>(Convert<BundlePolicy, RTCBundlePolicy>::To(config.bundle_policy)),
    Optional<RTCRtcpMuxPolicy>(Convert<RtcpMuxPolicy, RTCRtcpMuxPolicy>::To(config.rtcp_mux_policy)),
    Optional<std::string>(),
    Optional<unsigned short>());
}
