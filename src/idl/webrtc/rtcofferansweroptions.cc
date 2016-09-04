#include "src/idl/webrtc/rtcofferansweroptions.h"

using node_webrtc::Convert;
using node_webrtc::Either;
using node_webrtc::RTCOfferOptions;
using RTCOfferAnswerOptions = webrtc::PeerConnectionInterface::RTCOfferAnswerOptions;

Either<std::string, RTCOfferAnswerOptions> Convert<RTCOfferAnswerOptions, RTCOfferOptions>::From(RTCOfferOptions offerOptions) {
  RTCOfferAnswerOptions options;

  if (offerOptions.iceRestart()) {
    options.ice_restart = offerOptions.iceRestart();
  }

  if (offerOptions.offerToReceiveAudio()) {
    options.offer_to_receive_audio = *(offerOptions.offerToReceiveAudio()) ? 1 : 0;
  }

  if (offerOptions.offerToReceiveVideo()) {
    options.offer_to_receive_video = *(offerOptions.offerToReceiveVideo()) ? 1 : 0;
  }

  return Either<std::string, RTCOfferAnswerOptions>(options);
}

RTCOfferOptions Convert<RTCOfferAnswerOptions, RTCOfferOptions>::To(RTCOfferAnswerOptions options) {
  Optional<bool> offerToReceiveAudio;
  if (options.offer_to_receive_audio != RTCOfferAnswerOptions::kUndefined) {
    offerToReceiveAudio = Optional<bool>(options.offer_to_receive_audio > 0);
  }

  Optional<bool> offerToReceiveVideo;
  if (options.offer_to_receive_video != RTCOfferAnswerOptions::kUndefined) {
    offerToReceiveVideo = Optional<bool>(options.offer_to_receive_video > 0);
  }

  return RTCOfferOptions(
    Optional<bool>(options.ice_restart),
    offerToReceiveAudio,
    offerToReceiveVideo);
}
