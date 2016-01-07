#include "mediastreamconstraints.h"

using namespace node_webrtc;
using namespace v8;

bool MediaStreamConstraints::fromNode(Local<Value> unparsed, MediaStreamConstraints& _constraints, std::string* error)
{
  Nan::HandleScope scope;

  if (!unparsed->IsObject()) {
    if (error != nullptr) {
      *error = "MediaStreamConstraints must be an object";
    }
    return false;
  }
  Local<Object> object = Nan::To<Object>(unparsed).ToLocalChecked();

  Local<String> key;
  Local<Value> value;

  MediaStreamConstraints constraints;

  // audio
  key = Nan::New("audio").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsBoolean()) {
      if (error != nullptr) {
        *error = "MediaStreamConstraints audio must be boolean";
      }
      return false;
    }
    constraints.audio = Nan::Just<bool>(value->BooleanValue());
  }

  // video
  key = Nan::New("video").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsBoolean()) {
      if (error != nullptr) {
        *error = "MediaStreamConstraints video must be boolean";
      }
      return false;
    }
    constraints.video = Nan::Just<bool>(value->BooleanValue());
  }

  if (!constraints.audio.FromMaybe(false) && !constraints.video.FromMaybe(false)) {
    if (error != nullptr) {
      *error = "MediaStreamConstraints must specify at least audio or video";
    }
    return false;
  }

  _constraints = constraints;

  return true;
}
