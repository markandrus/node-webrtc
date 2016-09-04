#include "src/idl/webrtc/icetransportstype.h"

using node_webrtc::Convert;
using node_webrtc::Either;
using node_webrtc::RTCIceTransportPolicy;
using IceTransportsType = webrtc::PeerConnectionInterface::IceTransportsType;
using PeerConnectionInterface = webrtc::PeerConnectionInterface;

Either<std::string, IceTransportsType> Convert<IceTransportsType, RTCIceTransportPolicy>::From(RTCIceTransportPolicy iceTransportPolicy) {
  switch (iceTransportPolicy) {
    case node_webrtc::kRelay:
      return Either<std::string, IceTransportsType>(PeerConnectionInterface::kRelay);
    case node_webrtc::kAll:
      return Either<std::string, IceTransportsType>(PeerConnectionInterface::kAll);
  }
}

RTCIceTransportPolicy Convert<IceTransportsType, RTCIceTransportPolicy>::To(IceTransportsType iceTransportsType) {
  switch (iceTransportsType) {
    case PeerConnectionInterface::kRelay:
      return node_webrtc::kRelay;
    case PeerConnectionInterface::kAll:
    // TODO(mroberts): These are not valid RTCIceTransportPolicy values, so
    // instead we just report these as "all". Maybe we should add these to the
    // WebIDL definition?
    case PeerConnectionInterface::kNone:
    case PeerConnectionInterface::kNoHost:
      return node_webrtc::kAll;
  }
}
