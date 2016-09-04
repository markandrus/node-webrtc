#include "src/idl/webrtc/rtcpmuxpolicy.h"

using node_webrtc::Convert;
using node_webrtc::Either;
using node_webrtc::RTCRtcpMuxPolicy;
using RtcpMuxPolicy = webrtc::PeerConnectionInterface::RtcpMuxPolicy;
using PeerConnectionInterface = webrtc::PeerConnectionInterface;

Either<std::string, RtcpMuxPolicy> Convert<RtcpMuxPolicy, RTCRtcpMuxPolicy>::From(RTCRtcpMuxPolicy rtcpMuxPolicy) {
  switch (rtcpMuxPolicy) {
    case node_webrtc::kNegotiate:
      return Either<std::string, RtcpMuxPolicy>(PeerConnectionInterface::kRtcpMuxPolicyNegotiate);
    case node_webrtc::kRequire:
      return Either<std::string, RtcpMuxPolicy>(PeerConnectionInterface::kRtcpMuxPolicyRequire);
  }
}

RTCRtcpMuxPolicy Convert<RtcpMuxPolicy, RTCRtcpMuxPolicy>::To(RtcpMuxPolicy policy) {
  switch (policy) {
    case PeerConnectionInterface::kRtcpMuxPolicyNegotiate:
      return node_webrtc::kNegotiate;
    case PeerConnectionInterface::kRtcpMuxPolicyRequire:
      return node_webrtc::kRequire;
  }
}
