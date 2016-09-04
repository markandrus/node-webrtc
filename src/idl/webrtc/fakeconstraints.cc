#include "src/idl/webrtc/fakeconstraints.h"

using node_webrtc::Convert;
using node_webrtc::Either;
using node_webrtc::Optional;
using node_webrtc::RTCOfferOptions;
using webrtc::FakeConstraints;

Either<std::string, FakeConstraints> Convert<FakeConstraints, RTCOfferOptions>::From(RTCOfferOptions options) {
  FakeConstraints constraints;

  constraints.SetMandatoryIceRestart(options.iceRestart());

  if (options.offerToReceiveAudio()) {
    constraints.SetMandatoryReceiveAudio(*options.offerToReceiveAudio());
  }

  if (options.offerToReceiveVideo()) {
    constraints.SetMandatoryReceiveVideo(*options.offerToReceiveVideo());
  }

  return Either<std::string, FakeConstraints>(constraints);
}

RTCOfferOptions Convert<FakeConstraints, RTCOfferOptions>::To(FakeConstraints constraints) {
  // TODO(mroberts): We should do this correctly, but the MediaConstraintsInterface
  // is a PITA to work with.
  return RTCOfferOptions(
    Optional<bool>(),
    Optional<bool>(),
    Optional<bool>());
}
