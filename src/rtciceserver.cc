#include <nan.h>
#include "rtciceserver.h"

using namespace node_webrtc;
using namespace v8;
using IceServer = webrtc::PeerConnectionInterface::IceServer;

bool RTCIceServer::fromNode(Local<Value> unparsed, IceServer& iceServer, std::string* error)
{
  Nan::HandleScope scope;

  if (!unparsed->IsObject()) {
    if (error != nullptr) {
      *error = "RTCIceServer must be an object";
    }
    return false;
  }
  Local<Object> object = Nan::To<Object>(unparsed).ToLocalChecked();

  Local<String> key;
  Nan::MaybeLocal<Value> maybeValue;
  Local<Value> value;

  // urls
  // NOTE(mroberts): Let's also include legacy support for "url".
  std::vector<std::string> urls;
  Local<String> urlsKey = Nan::New("urls").ToLocalChecked();
  Local<String> urlKey = Nan::New("url").ToLocalChecked();
  if (Nan::Has(object, urlsKey).FromMaybe(false)) {
    value = Nan::Get(object, urlsKey).ToLocalChecked();
    if (value->IsString()) {
      urls.push_back(*(String::Utf8Value(value)));
    } else if (value->IsArray()) {
      Local<Array> array = value.As<Array>();
      for (uint32_t i = 0; i < array->Length(); i++) {
        Local<Value> urlValue = Nan::Get(array, Nan::New(i)).ToLocalChecked();
        if (!urlValue->IsString()) {
          if (error != nullptr) {
            *error = "RTCIceServer urls must be a string or an array of strings";
          }
          return false;
        }
        urls.push_back(*(String::Utf8Value(value)));
      }
    } else {
      if (error != nullptr) {
        *error = "RTCIceServer urls must be a string or an array of strings";
      }
      return false;
    }
  } else if (Nan::Has(object, urlKey).FromMaybe(false)) {
    value = Nan::Get(object, urlKey).ToLocalChecked();
    if (value->IsString()) {
      urls.push_back(*(String::Utf8Value(value)));
    } else if (value->IsArray()) {
      Local<Array> array = value.As<Array>();
      for (uint32_t i = 0; i < array->Length(); i++) {
        Local<Value> urlValue = Nan::Get(array, Nan::New(i)).ToLocalChecked();
        if (!urlValue->IsString()) {
          if (error != nullptr) {
            *error = "RTCIceServer url must be a string or an array of strings";
          }
          return false;
        }
        urls.push_back(*(String::Utf8Value(value)));
      }
    } else {
      if (error != nullptr) {
        *error = "RTCIceServer url must be a string or an array of strings";
      }
      return false;
    }
  } else {
    if (error != nullptr) {
      *error = "RTCIceServer urls must be a string or an array of strings";
    }
    return false;
  }

  // username
  std::string username;
  key = Nan::New("username").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "RTCIceServer username must be a string";
      }
      return false;
    }
    username = *(String::Utf8Value(value));
  }

  // credential
  std::string credential;
  key = Nan::New("credential").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "RTCIceServer credential must be a string";
      }
      return false;
    }
    credential = *(String::Utf8Value(value));
  }

  // credentialType
  std::string credentialType = "password";
  key = Nan::New("credentialType").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "RTCIceCredentialType must be a string";
      }
      return false;
    }
    std::string string = *(String::Utf8Value(value));
    if (string != "password" && string != "token") {
      if (error != nullptr) {
        *error = "RTCIceCredentialType must be on of \"password\" or \"token\"";
      }
      return false;
    }
    credentialType = string;
  }

  iceServer.urls = urls;
  iceServer.username = username;
  // TODO(mroberts): As of M48, WebRTC only supports the "password" credential
  // type; Update this when that changes.
  iceServer.password = credential;

  return true;
}

Local<RTCIceServer::RTCIceServer> RTCIceServer::toNode(IceServer* iceServer)
{
  Nan::EscapableHandleScope scope;
  Local<Object> object = Nan::New<Object>();
  Local<Array> urls = Nan::New<Array>();
  uint32_t i = 0;
  for (std::string url : iceServer->urls) {
    Nan::Set(urls, Nan::New(i++), Nan::New(url).ToLocalChecked());
  }
  Nan::Set(object, Nan::New("urls").ToLocalChecked(), urls);
  Nan::Set(object, Nan::New("username").ToLocalChecked(), Nan::New(iceServer->username).ToLocalChecked());
  Nan::Set(object, Nan::New("credential").ToLocalChecked(), Nan::New(iceServer->password).ToLocalChecked());
  // TODO(mroberts): As of M48, WebRTC only supports the "password" credential
  // type. Update this when that changes.
  Nan::Set(object, Nan::New("credentialType").ToLocalChecked(), Nan::New("password").ToLocalChecked());
  return scope.Escape(object);
}
