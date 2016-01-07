#include "rtcicecandidateinit.h"

using namespace node_webrtc;
using namespace v8;

bool RTCIceCandidateInit::fromNode(Local<Value> unparsed, RTCIceCandidateInit& candidateInit, std::string* error)
{
  Nan::HandleScope scope;

  if (!unparsed->IsObject()) {
    if (error != nullptr) {
      *error = "RTCIceCandidateInit must be an object";
    }
    return false;
  }
  Local<Object> object = Nan::To<Object>(unparsed).ToLocalChecked();

  Local<String> key;
  Local<Value> value;

  // candidate
  std::string candidate;
  key = Nan::New("candidate").ToLocalChecked();
  if (!Nan::Has(object, key).FromMaybe(false)) {
    if (error != nullptr) {
      *error = "RTCIceCandidateInit candidate must be a string";
    }
    return false;
  }

  value = Nan::Get(object, key).ToLocalChecked();
  if (!value->IsString()) {
    if (error != nullptr) {
      *error = "RTCIceCandidateInit candidate must be a string";
    }
    return false;
  }
  candidate = *(String::Utf8Value(value));

  // sdpMid
  Nan::Maybe<std::string> sdpMid = Nan::Nothing<std::string>();
  key = Nan::New("sdpMid").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "RTCIceCandidateInit sdpMid must be a string";
      }
      return false;
    }
    sdpMid = Nan::Just<std::string>(*(String::Utf8Value(value)));
  }

  // sdpMLineIndex
  Nan::Maybe<uint32_t> sdpMLineIndex = Nan::Nothing<uint32_t>();
  key = Nan::New("sdpMLineIndex").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsUint32()) {
      if (error != nullptr) {
        *error = "RTCIceCandidateInit sdpMLineIndex must be an unsigned short";
      }
      return false;
    }
    sdpMLineIndex = Nan::Just<uint32_t>(Nan::To<uint32_t>(value).FromJust());
  }

  candidateInit.candidate = candidate;
  candidateInit.sdpMid = sdpMid;
  candidateInit.sdpMLineIndex = sdpMLineIndex;

  return true;
}

Local<Object> RTCIceCandidateInit::toNode(RTCIceCandidateInit* candidateInit)
{
  Nan::EscapableHandleScope scope;
  Local<Object> object = Nan::New<Object>();
  Nan::Set(object, Nan::New("candidate").ToLocalChecked(),
    Nan::New(candidateInit->candidate).ToLocalChecked());
  if (candidateInit->sdpMid.IsJust()) {
    Nan::Set(object, Nan::New("sdpMid").ToLocalChecked(), Nan::New(candidateInit->sdpMid.FromJust()).ToLocalChecked());
  }
  if (candidateInit->sdpMLineIndex.IsJust()) {
    Nan::Set(object, Nan::New("sdpMLineIndex").ToLocalChecked(), Nan::New(candidateInit->sdpMLineIndex.FromJust()));
  }
  return scope.Escape(object);
}
