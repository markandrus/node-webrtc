#include <nan.h>
#include "rtcofferansweroptions.h"
#include "rtcofferoptions.h"

using namespace node_webrtc;
using namespace v8;

bool RTCOfferOptions::fromNode(Local<Value> unparsed, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options, std::string* error)
{
  if (!RTCOfferAnswerOptions::fromNode(unparsed, options, error)) {
    return false;
  }
  Local<Object> object = Nan::To<Object>(unparsed).ToLocalChecked();

  Local<String> key;
  Local<Value> value;

  // offerToReceiveAudio
  int32_t offerToReceiveAudio = 0;
  key = Nan::New("offerToReceiveAudio").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (value->IsNull() || value->IsUndefined()) {
      offerToReceiveAudio = -1;
    } else if (value->IsBoolean()) {
      // JSEP, Sections 5.2.3.1 and 5.2.3.2:
      //
      //   For backwards compatibility with pre-standard versions of this
      //   specification, a value of "true" is interpreted as equivalent to N=1,
      //   and "false" as N=0.
      //
      offerToReceiveAudio = value->BooleanValue() ? 1 : 0;
    } else if (value->IsInt32()) {
      int32_t i = value->Int32Value();
      if (i < 0) {
        if (error != nullptr) {
          *error = "RTCOfferOptions offerToReceiveAudio must be greater than 0";
        }
        return false;
      }
      offerToReceiveAudio = i;
    } else {
      if (error != nullptr) {
        *error = "RTCOfferOptions offerToReceiveAudio must be an integer";
      }
      return false;
    }
  }

  // offerToReceiveVideo
  int32_t offerToReceiveVideo = 0;
  key = Nan::New("offerToReceiveVideo").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (value->IsNull() || value->IsUndefined()) {
      offerToReceiveVideo = -1;
    } else if (value->IsBoolean()) {
      // JSEP, Sections 5.2.3.1 and 5.2.3.2:
      //
      //   For backwards compatibility with pre-standard versions of this
      //   specification, a value of "true" is interpreted as equivalent to N=1,
      //   and "false" as N=0.
      //
      offerToReceiveVideo = value->BooleanValue() ? 1 : 0;
    } else if (value->IsInt32()) {
      int32_t i = value->Int32Value();
      if (i < 0) {
        if (error != nullptr) {
          *error = "RTCOfferOptions offerToReceiveVideo must be greater than 0";
        }
        return false;
      }
      offerToReceiveVideo = i;
    } else {
      if (error != nullptr) {
        *error = "RTCOfferOptions offerToReceiveVideo must be an integer";
      }
      return false;
    }
  }

  // iceRestart
  bool iceRestart = false;
  key = Nan::New("iceRestart").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsBoolean()) {
      if (error != nullptr) {
        *error = "RTCOfferOptions iceRestart must be a boolean";
      }
      return false;
    }
    iceRestart = value->BooleanValue();
  }

  // useRtpMux
  bool useRtpMux = true;
  key = Nan::New("useRtpMux").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsBoolean()) {
      if (error != nullptr) {
        *error = "RTCOfferOptions useRtpMux must be a boolean";
      }
      return false;
    }
    useRtpMux = value->BooleanValue();
  }

  options.offer_to_receive_audio = offerToReceiveAudio;
  options.offer_to_receive_video = offerToReceiveVideo;
  options.ice_restart = iceRestart;
  options.use_rtp_mux = useRtpMux;

  return true;
}
