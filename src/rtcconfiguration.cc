#include <nan.h>
#include "rtccertificate.h"
#include "rtcconfiguration.h"
#include "rtciceserver.h"
#include "webrtc/base/rtccertificate.h"
#include "webrtc/base/scoped_ref_ptr.h"

using namespace node_webrtc;
using namespace v8;

using BundlePolicy = webrtc::PeerConnectionInterface::BundlePolicy;
using IceServers =  webrtc::PeerConnectionInterface::IceServers;
using IceServer = webrtc::PeerConnectionInterface::IceServer;
using IceTransportsType = webrtc::PeerConnectionInterface::IceTransportsType;
using RtcpMuxPolicy = webrtc::PeerConnectionInterface::RtcpMuxPolicy;

bool RTCConfiguration::fromNode(Local<Value> unparsed, webrtc::PeerConnectionInterface::RTCConfiguration& configuration, std::string* error)
{
  Nan::HandleScope scope;

  if (!unparsed->IsObject()) {
    if (error != nullptr) {
      *error = "RTCConfiguration must be an object";
    }
    return false;
  }
  Local<Object> object = Nan::To<Object>(unparsed).ToLocalChecked();

  Local<String> key;
  Local<Value> value;
  Local<Array> array;

  // iceServers
  IceServers iceServers;
  key = Nan::New("iceServers").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsArray()) {
      if (error != nullptr) {
        *error = "RTCConfiguration iceServers must be an array of RTCIceServers";
      }
      return false;
    }
    array = value.As<Array>();
    for (uint32_t i = 0; i < array->Length(); i++) {
      Local<Value> iceServerValue = Nan::Get(array, i).ToLocalChecked();
      IceServer iceServer;
      if (!RTCIceServer::fromNode(iceServerValue, iceServer, error)) {
        return false;
      }
      iceServers.push_back(iceServer);
    }
  }

  // iceTransportPolicy
  IceTransportsType iceTransportPolicy = IceTransportsType::kAll;
  key = Nan::New("iceTransportPolicy").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "RTCIceTransportPolicy must be a string";
      }
      return false;
    }
    std::string string = *(String::Utf8Value(value));
    if (string == "public") {
      // TODO(mroberts): Not sure about this one.
      iceTransportPolicy = IceTransportsType::kNoHost;
    } else if (string == "relay") {
      iceTransportPolicy = IceTransportsType::kRelay;
    } else if (string == "all") {
      iceTransportPolicy = IceTransportsType::kAll;
    } else {
      if (error != nullptr) {
        *error = "RTCIceTransportPolicy must be one of \"public\", \"relay\", or \"all\"";
      }
      return false;
    }
  }

  // bundlePolicy
  BundlePolicy bundlePolicy = BundlePolicy::kBundlePolicyBalanced;
  key = Nan::New("bundlePolicy").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "RTCBundlePolicy must be a string";
      }
      return false;
    }
    std::string string = *(String::Utf8Value(value));
    if (string == "balanced") {
      bundlePolicy = BundlePolicy::kBundlePolicyBalanced;
    } else if (string == "max-compat") {
      bundlePolicy = BundlePolicy::kBundlePolicyMaxCompat;
    } else if (string == "max-bundle") {
      bundlePolicy = BundlePolicy::kBundlePolicyMaxBundle;
    } else {
      if (error != nullptr) {
        *error = "RTCBundlePolicy must be on of \"balanced\", \"max-compat\", or \"max-bundle\"";
      }
      return false;
    }
  }

  // rtcpMuxPolicy
  RtcpMuxPolicy rtcpMuxPolicy = RtcpMuxPolicy::kRtcpMuxPolicyRequire;
  key = Nan::New("rtcpMuxPolicy").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "RTCRtcpMuxPolicy must be a string";
      }
      return false;
    }
    std::string string = *(String::Utf8Value(value));
    if (string == "negotiate") {
      rtcpMuxPolicy = RtcpMuxPolicy::kRtcpMuxPolicyNegotiate;
    } else if (string == "require") {
      rtcpMuxPolicy = RtcpMuxPolicy::kRtcpMuxPolicyRequire;
    } else {
      if (error != nullptr) {
        *error = "RTCRtcpMuxPolicy must be on of \"negotiate\" or \"require\"";
      }
      return false;
    }
  }

  // peerIdentity
  std::string peerIdentity;
  key = Nan::New("peerIdentity").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsString()) {
      if (error != nullptr) {
        *error = "RTCConfiguration peerIdentity must be a string";
      }
      return false;
    }
    peerIdentity = *(String::Utf8Value(value));
  }

  // certificates
  std::vector<rtc::scoped_refptr<rtc::RTCCertificate>> certificates;
  key = Nan::New("certificates").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsArray()) {
      if (error != nullptr) {
        *error = "RTCConfiguration certificates must be an array of RTCCertificates";
      }
      return false;
    }
    // TODO(mroberts): ...
  }

  // iceCandidatePoolSize
  uint32_t iceCandidatePoolSize = 0;
  key = Nan::New("iceCandidatePoolSize").ToLocalChecked();
  if (Nan::Has(object, key).FromMaybe(false)) {
    value = Nan::Get(object, key).ToLocalChecked();
    if (!value->IsUint32()) {
      if (error != nullptr) {
        *error = "RTCConfiguration iceCandidatePoolSize must be an unsigned short";
      }
      return false;
    }
    iceCandidatePoolSize = Nan::To<uint32_t>(value).FromJust();
  }

  configuration.servers = iceServers;
  configuration.type = iceTransportPolicy;
  configuration.bundle_policy = bundlePolicy;
  configuration.rtcp_mux_policy = rtcpMuxPolicy;

  return true;
}

Local<RTCConfiguration::RTCConfiguration> RTCConfiguration::toNode(webrtc::PeerConnectionInterface::RTCConfiguration* configuration)
{
  Nan::EscapableHandleScope scope;
  Local<Object> object = Nan::New<Object>();

  // iceServers
  Local<Array> iceServers = Nan::New<Array>();
  uint32_t i = 0;
  for (IceServer iceServer : configuration->servers) {
    Nan::Set(iceServers, Nan::New(i++), RTCIceServer::toNode(&iceServer));
  }
  Nan::Set(object, Nan::New("iceServers").ToLocalChecked(), iceServers);

  // iceTransportPolicy
  std::string iceTransportPolicy;
  // TODO(mroberts): Not sure about these.
  switch (configuration->type) {
    case IceTransportsType::kNone:
      iceTransportPolicy = "none";
      break;
    case IceTransportsType::kRelay:
      iceTransportPolicy = "relay";
      break;
    case IceTransportsType::kNoHost:
      iceTransportPolicy = "no-host";
      break;
    case IceTransportsType::kAll:
      iceTransportPolicy= "all";
      break;
    default:
      assert(false);  // impossible
  }
  Nan::Set(object, Nan::New("iceTransportPolicy").ToLocalChecked(),
    Nan::New(iceTransportPolicy).ToLocalChecked());

  // bundlePolicy
  std::string bundlePolicy;
  switch (configuration->bundle_policy) {
    case BundlePolicy::kBundlePolicyBalanced:
      bundlePolicy = "balanced";
      break;
    case BundlePolicy::kBundlePolicyMaxBundle:
      bundlePolicy = "max-bundle";
      break;
    case BundlePolicy::kBundlePolicyMaxCompat:
      bundlePolicy = "max-compat";
      break;
    default:
      assert(false);  // impossible
  }
  Nan::Set(object, Nan::New("bundlePolicy").ToLocalChecked(),
    Nan::New(bundlePolicy).ToLocalChecked());

  // rtcpMuxPolicy
  std::string rtcpMuxPolicy;
  switch (configuration->rtcp_mux_policy) {
    case RtcpMuxPolicy::kRtcpMuxPolicyNegotiate:
      rtcpMuxPolicy= "negotiate";
      break;
    case RtcpMuxPolicy::kRtcpMuxPolicyRequire:
      rtcpMuxPolicy= "require";
      break;
    default:
      assert(false);  // impossible
  }
  Nan::Set(object, Nan::New("rtcpMuxPolicy").ToLocalChecked(),
    Nan::New(rtcpMuxPolicy).ToLocalChecked());

  // peerIdentity
  // TODO(mroberts): ...

  // certificates
  Local<Array> certificates = Nan::New<Array>();
  i = 0;
  for (rtc::scoped_refptr<rtc::RTCCertificate> certificate : configuration->certificates) {
    Nan::Set(certificates, Nan::New(i++), RTCCertificate::toNode(certificate));
  }
  Nan::Set(object, Nan::New("certificates").ToLocalChecked(), certificates);

  // iceCandidatePoolSize
  // TODO(mroberts): ...

  return scope.Escape(object);
}
