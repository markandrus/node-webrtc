#include <iostream>
#include "common.h"
#include "utilities.h"

using namespace v8;
using namespace webrtc;

typedef PeerConnectionInterface::IceConnectionState IceConnectionState;
typedef PeerConnectionInterface::IceGatheringState IceGatheringState;
typedef PeerConnectionInterface::RTCOfferAnswerOptions RTCOfferAnswerOptions;
typedef PeerConnectionInterface::SignalingState SignalingState;

FakeConstraints node_webrtc::convertRTCOfferAnswerOptionsToFakeConstraints(const RTCOfferAnswerOptions &options)
{
  FakeConstraints constraints;
  constraints.SetMandatoryReceiveAudio(options.offer_to_receive_audio);
  constraints.SetMandatoryReceiveVideo(options.offer_to_receive_video);
  constraints.SetOptionalVAD(options.voice_activity_detection);
  constraints.SetMandatoryIceRestart(options.ice_restart);
  constraints.SetMandatoryUseRtpMux(options.use_rtp_mux);
  return constraints;
}

SessionDescriptionInterface* node_webrtc::parseSessionDescription(Local<Value> value, std::string* error)
{
  TRACE_CALL;

  if (value->IsNull() || value->IsUndefined()) {
    if (error) {
      *error = "Invalid RTCSessionDescription";
    }
    TRACE_END;
    return nullptr;
  }
  Local<Object> desc = Nan::To<Object>(value).ToLocalChecked();

  // Parse the RTCSessionDescription type.
  MaybeLocal<Value> maybeDescTypeProp = Nan::Get(desc,
    Nan::New("type").ToLocalChecked());
  if (maybeDescTypeProp.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCSessionDescription type";
    }
    TRACE_END;
    return nullptr;
  }

  MaybeLocal<String> maybeDescType = Nan::To<String>(maybeDescTypeProp.ToLocalChecked());
  if (maybeDescType.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCSessionDescription type";
    }
    TRACE_END;
    return nullptr;
  }

  String::Utf8Value descType(maybeDescType.ToLocalChecked());
  std::string type = *descType;

  // Parse the RTCSessionDescription sdp.
  MaybeLocal<Value> maybeDescSdpProp = Nan::Get(desc,
    Nan::New("sdp").ToLocalChecked());
  if (maybeDescSdpProp.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCSessionDescription sdp";
    }
    TRACE_END;
    return nullptr;
  }

  MaybeLocal<String> maybeDescSdp = Nan::To<String>(maybeDescSdpProp.ToLocalChecked());
  if (maybeDescSdp.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCSessionDescription sdp";
    }
    TRACE_END;
    return nullptr;
  }

  String::Utf8Value descSdp(maybeDescSdp.ToLocalChecked());
  std::string sdp = *descSdp;

  // Attempt to construct a SessionDescriptionInterface.
  SdpParseError parseError;
  SessionDescriptionInterface* sdi = CreateSessionDescription(type, sdp, &parseError);
  if (!sdi) {
    if (error) {
      *error = parseError.description;
    }
    TRACE_END;
    return nullptr;
  }

  TRACE_END;
  return sdi;
}

Local<Object> node_webrtc::convertSessionDescriptionInterfaceToJson(SessionDescriptionInterface* sdi)
{
  TRACE_CALL;
  Nan::EscapableHandleScope scope;

  Local<Object> json = Nan::New<Object>();

  std::string sdp;
  sdi->ToString(&sdp);
  Nan::Set(json, Nan::New("sdp").ToLocalChecked(), Nan::New(sdp).ToLocalChecked());

  std::string type = sdi->type();
  Nan::Set(json, Nan::New("type").ToLocalChecked(), Nan::New(type).ToLocalChecked());

  TRACE_END;
  return scope.Escape(json);
}

IceCandidateInterface* node_webrtc::parseIceCandidate(Local<Value> value, std::string* error)
{
  TRACE_CALL;

  if (!value->IsObject()) {
    if (error) {
      *error = "Invalid RTCIceCandidate";
    }
    TRACE_END;
    return nullptr;
  }
  Local<Object> cand = Nan::To<Object>(value).ToLocalChecked();

  // Parse the RTCIceCandidate sdpMLineIndex.
  MaybeLocal<Value> maybeCandSdpMLineIndexProp = Nan::Get(cand,
    Nan::New("sdpMLineIndex").ToLocalChecked());
  if (maybeCandSdpMLineIndexProp.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCIceCandidate sdpMLineIndex";
    }
    TRACE_END;
    return nullptr;
  }

  Maybe<int> maybeCandSdpMLineIndex = Nan::To<int>(maybeCandSdpMLineIndexProp.ToLocalChecked());
  if (maybeCandSdpMLineIndex.IsNothing()) {
    if (error) {
      *error = "Invalid RTCIceCandidate sdpMLineIndex";
    }
    TRACE_END;
    return nullptr;
  }

  int sdpMLineIndex = maybeCandSdpMLineIndex.FromJust();

  // Parse the RTCIceCandidate sdpMid.
  MaybeLocal<Value> maybeCandSdpMidProp = Nan::Get(cand,
    Nan::New("sdpMid").ToLocalChecked());
  if (maybeCandSdpMidProp.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCIceCandidate sdpMid";
    }
    TRACE_END;
    return nullptr;
  }

  MaybeLocal<String> maybeCandSdpMid = Nan::To<String>(maybeCandSdpMidProp.ToLocalChecked());
  if (maybeCandSdpMid.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCIceCandidate sdpMid";
    }
    TRACE_END;
    return nullptr;
  }

  String::Utf8Value candSdpMid(maybeCandSdpMid.ToLocalChecked());
  std::string sdpMid = *candSdpMid;

  // Parse the RTCIceCandidate candidate.
  MaybeLocal<Value> maybeCandCandidateProp = Nan::Get(cand,
    Nan::New("candidate").ToLocalChecked());
  if (maybeCandCandidateProp.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCIceCandidate candidate";
    }
    TRACE_END;
    return nullptr;
  }

  MaybeLocal<String> maybeCandCandidate = Nan::To<String>(maybeCandCandidateProp.ToLocalChecked());
  if (maybeCandCandidate.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCIceCandidate candidate";
    }
    TRACE_END;
    return nullptr;
  }

  String::Utf8Value candCandidate(maybeCandCandidate.ToLocalChecked());
  std::string candidate = *candCandidate;

  // Attempt to construct an IceCandidateInterface.
  SdpParseError parseError;
  IceCandidateInterface* ici = webrtc::CreateIceCandidate(sdpMid, sdpMLineIndex, candidate, &parseError);
  if (!ici) {
    if (error) {
      *error = parseError.description;
    }
    TRACE_END;
    return nullptr;
  }

  TRACE_END;
  return ici;
}

DataChannelInit* node_webrtc::parseDataChannelInit(Local<Value> value, std::string* error)
{
  TRACE_CALL;
  Nan::HandleScope scope;

  if (value->IsNull() || value->IsUndefined()) {
    TRACE_END;
    return new DataChannelInit();
  } else if (!value->IsObject()) {
    if (error) {
      *error = "Invalid RTCDataChannelInit";
    }
    TRACE_END;
    return nullptr;
  }
  Local<Object> dataChannelDict = Nan::To<Object>(value).ToLocalChecked();

  DataChannelInit* dataChannelInit = new DataChannelInit();

  if (dataChannelDict->Has(Nan::New("id").ToLocalChecked())) {
    Local<Value> value = dataChannelDict->Get(Nan::New("id").ToLocalChecked());
    if (value->IsInt32()) {
      dataChannelInit->id = value->Int32Value();
    }
  }

  if (dataChannelDict->Has(Nan::New("maxRetransmitTime").ToLocalChecked())) {
    Local<Value> value = dataChannelDict->Get(Nan::New("maxRetransmitTime").ToLocalChecked());
    if (value->IsInt32()) {
      dataChannelInit->maxRetransmitTime = value->Int32Value();
    }
  }

  if (dataChannelDict->Has(Nan::New("maxRetransmits").ToLocalChecked())) {
    Local<Value> value = dataChannelDict->Get(Nan::New("maxRetransmits").ToLocalChecked());
    if (value->IsInt32()) {
      dataChannelInit->maxRetransmits = value->Int32Value();
    }
  }

  if (dataChannelDict->Has(Nan::New("negotiated").ToLocalChecked())) {
    Local<Value> value = dataChannelDict->Get(Nan::New("negotiated").ToLocalChecked());
    if (value->IsBoolean()) {
      dataChannelInit->negotiated = value->BooleanValue();
    }
  }

  if (dataChannelDict->Has(Nan::New("ordered").ToLocalChecked())) {
    Local<Value> value = dataChannelDict->Get(Nan::New("ordered").ToLocalChecked());
    if (value->IsBoolean()) {
      dataChannelInit->ordered = value->BooleanValue();
    }
  }

  if (dataChannelDict->Has(Nan::New("protocol").ToLocalChecked())) {
    Local<Value> value = dataChannelDict->Get(Nan::New("protocol").ToLocalChecked());
    if (value->IsString()) {
      dataChannelInit->protocol = *String::Utf8Value(value->ToString());
    }
  }

  TRACE_END;
  return dataChannelInit;
}

Local<String> node_webrtc::signalingStateToString(SignalingState signalingState)
{
  Nan::EscapableHandleScope scope;
  std::string signalingStateString;
  switch (signalingState) {
    case SignalingState::kStable:
      signalingStateString = "stable";
      break;
    case SignalingState::kHaveLocalOffer:
      signalingStateString = "have-local-offer";
      break;
    case SignalingState::kHaveLocalPrAnswer:
      signalingStateString = "have-local-pranswer";
      break;
    case SignalingState::kHaveRemoteOffer:
      signalingStateString = "have-remote-offer";
      break;
    case SignalingState::kHaveRemotePrAnswer:
      signalingStateString = "have-remote-pranswer";
      break;
    case SignalingState::kClosed:
      signalingStateString = "closed";
      break;
    default:
      assert(false);  // impossible
  }
  return scope.Escape(Nan::New(signalingStateString).ToLocalChecked());
}

Local<String> node_webrtc::iceConnectionStateToString(IceConnectionState iceConnectionState)
{
  Nan::EscapableHandleScope scope;
  std::string iceConnectionStateString;
  switch (iceConnectionState) {
    case IceConnectionState::kIceConnectionNew:
      iceConnectionStateString = "new";
      break;
    case IceConnectionState::kIceConnectionChecking:
      iceConnectionStateString = "checking";
      break;
    case IceConnectionState::kIceConnectionConnected:
      iceConnectionStateString = "connected";
      break;
    case IceConnectionState::kIceConnectionCompleted:
      iceConnectionStateString = "completed";
      break;
    case IceConnectionState::kIceConnectionFailed:
      iceConnectionStateString = "failed";
      break;
    case IceConnectionState::kIceConnectionDisconnected:
      iceConnectionStateString = "disconnected";
      break;
    case IceConnectionState::kIceConnectionClosed:
      iceConnectionStateString = "closed";
      break;
    case IceConnectionState::kIceConnectionMax:
      iceConnectionStateString = "max";
      break;
    default:
      assert(false);  // impossible
  }
  return scope.Escape(Nan::New(iceConnectionStateString).ToLocalChecked());
}

Local<String> node_webrtc::iceGatheringStateToString(IceGatheringState iceGatheringState)
{
  Nan::EscapableHandleScope scope;
  std::string iceGatheringStateString;
  switch (iceGatheringState) {
    case IceGatheringState::kIceGatheringNew:
      iceGatheringStateString = "new";
      break;
    case IceGatheringState::kIceGatheringGathering:
      iceGatheringStateString = "gathering";
      break;
    case IceGatheringState::kIceGatheringComplete:
      iceGatheringStateString = "complete";
      break;
    default:
      assert(false);  // impossible
  }
  return scope.Escape(Nan::New(iceGatheringStateString).ToLocalChecked());
}

PeerConnectionInterface::RTCConfiguration* node_webrtc::parseRTCConfiguration(Local<Value> value, std::string* error)
{
  // TODO(mroberts): ...
  (void) value;
  (void) error;
  return new PeerConnectionInterface::RTCConfiguration();
}

Local<Value> rtcConfigurationToJson(PeerConnectionInterface::RTCConfiguration* configuration)
{
  Nan::EscapableHandleScope scope;
  Local<Object> json = Nan::New<Object>();

  

  return scope.Escape(json);
}
