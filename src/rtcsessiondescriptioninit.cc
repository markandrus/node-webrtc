#include "rtcsessiondescriptioninit.h"

using namespace node_webrtc;
using namespace v8;

bool RTCSessionDescriptionInit::fromNode(Local<Value> unparsed, RTCSessionDescriptionInit& descriptionInit, std::string* error)
{
  Nan::HandleScope scope;

  if (!unparsed->IsObject()) {
    if (error != nullptr) {
      *error = "RTCSessionDescriptionInit must be an object";
    }
    return false;
  }
  Local<Object> object = Nan::To<Object>(unparsed).ToLocalChecked();

  Local<String> key;
  Local<Value> value;

  // type
  std::string type;
  key = Nan::New("type").ToLocalChecked();
  if (!Nan::Has(object, key).FromMaybe(false)) {
    if (error != nullptr) {
      *error = "RTCSessionDescriptionInit type must be a string";
    }
    return false;
  }

  value = Nan::Get(object, key).ToLocalChecked();
  if (!value->IsString()) {
    if (error != nullptr) {
      *error = "RTCSessionDescriptionInit type must be a string";
    }
    return false;
  }

  std::string string = *(String::Utf8Value(value));
  if (string != "offer" && string != "pranswer" && string != "answer" && string != "rollback") {
    if (error != nullptr) {
      *error = "RTCSessionDescriptionInit type must be on of \"offer\", \"pranswer\", \"answer\", or \"rollback\"";
    }
    return false;
  }

  type = string;

  // sdp
  Nan::Maybe<std::string> sdp = Nan::Nothing<std::string>();
  key = Nan::New("sdp").ToLocalChecked();
  if (!Nan::Has(object, key).FromMaybe(false) && type != "rollback") {
    if (error != nullptr) {
      *error = "RTCSessionDescriptionInit sdp must be a string";
    }
    return false;
  } else {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "RTCSessionDescriptionInit sdp must be a string";
      }
      return false;
    }
    sdp = Nan::Just<std::string>(*(String::Utf8Value(value)));
  }

  descriptionInit.type = type;
  descriptionInit.sdp = sdp;

  return true;
}

Local<Object> RTCSessionDescriptionInit::toNode(RTCSessionDescriptionInit* descriptionInit)
{
  Nan::EscapableHandleScope scope;
  Local<Object> object = Nan::New<Object>();
  Nan::Set(object, Nan::New("type").ToLocalChecked(), Nan::New(descriptionInit->type).ToLocalChecked());
  if (descriptionInit->sdp.IsJust()) {
    Nan::Set(object, Nan::New("sdp").ToLocalChecked(), Nan::New(descriptionInit->sdp.FromJust()).ToLocalChecked());
  }
  return scope.Escape(object);
}
