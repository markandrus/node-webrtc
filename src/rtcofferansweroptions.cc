#include <nan.h>
#include "rtcofferansweroptions.h"

using namespace node_webrtc;
using namespace v8;

bool RTCOfferAnswerOptions::fromNode(Local<Value> unparsed, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options, std::string* error)
{
  Nan::HandleScope scope;

  if (!unparsed->IsObject()) {
    if (error != nullptr) {
      *error = "RTCOfferAnswerOptions must be an object";
    }
    return false;
  }
  Local<Object> object = Nan::To<Object>(unparsed).ToLocalChecked();

  Local<String> key;
  Local<Value> value;

  // voiceActivityDetection
  bool voiceActivityDetection = true;
  key = Nan::New("voiceActivityDetection").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsBoolean()) {
      if (error != nullptr) {
        *error = "RTCOfferAnswerOptions voiceActivityDetection must be a boolean";
      }
      return false;
    }
    voiceActivityDetection = value->BooleanValue();
  }

  options.voice_activity_detection = voiceActivityDetection;

  return true;
}
