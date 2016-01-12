#include "media/recorder/mediarecorderoptions.h"

using namespace node_webrtc;
using namespace v8;

bool MediaRecorderOptions::from_node(
    Local<Value> unparsed,
    MediaRecorderOptions& options,
    std::string* error)
{
  Nan::EscapableHandleScope scope;

  if (!unparsed->IsObject()) {
    if (error != nullptr) {
      *error = "MediaRecorderOptions must be an object";
    }
    return false;
  }
  Local<Object> object = Nan::To<Object>(unparsed).ToLocalChecked();

  Local<String> key;
  Local<Value> value;

  // audioBitsPerSecond
  Nan::Maybe<long> audio_bits_per_second = Nan::Nothing<long>();
  key = Nan::New("audioBitsPerSecond").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsNumber()) {
      if (error != nullptr) {
        *error = "MediaRecorderOptions audioBitsPerSecond must be a number";
      }
      return false;
    }
    audio_bits_per_second = Nan::Just<long>(value->NumberValue());
  }

  // bitsPerSecond
  Nan::Maybe<long> bits_per_second = Nan::Nothing<long>();
  key = Nan::New("bitsPerSecond").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsNumber()) {
      if (error != nullptr) {
        *error = "MediaRecorderOptions bitsPerSecond must be a number";
      }
      return false;
    }
    bits_per_second = Nan::Just<long>(value->NumberValue());
  }


  // mimeType
  std::string mime_type;
  key = Nan::New("mimeType").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "MediaRecorderOptions mimeType must be a string";
      }
      return false;
    }
    mime_type = *(String::Utf8Value(value));
  }

  // videoBitsPerSecond
  Nan::Maybe<long> video_bits_per_second = Nan::Nothing<long>();
  key = Nan::New("videoBitsPerSecond").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsNumber()) {
      if (error != nullptr) {
        *error = "MediaRecorderOptions videoBitsPerSecond must be a number";
      }
      return false;
    }
    video_bits_per_second = Nan::Just<long>(value->NumberValue());
  }

  options.audio_bits_per_second = audio_bits_per_second;
  options.bits_per_second = bits_per_second;
  options.mime_type = mime_type;
  options.video_bits_per_second = video_bits_per_second;

  return true;
}
