/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <node_buffer.h>

#include <stdint.h>
#include <iostream>
#include <string>

#include "talk/app/webrtc/jsep.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/system_wrappers/include/ref_count.h"
#include "talk/app/webrtc/test/fakeconstraints.h"
#include "talk/app/webrtc/peerconnectioninterface.h"

#include "either.h"
#include "asyncevent2.h"
#include "common.h"
#include "peerconnection.h"
#include "datachannel.h"
//#include "mediastream.h"
#include "mediastream2.h"
#include "create-offer-observer.h"
#include "createsessiondescriptionobserver.h"
#include "setsessiondescriptionobserver.h"
#include "create-answer-observer.h"
#include "set-local-description-observer.h"
#include "set-remote-description-observer.h"
#include "stats-observer.h"
#include "rtcstatsresponse.h"
#include "rtcicecandidate.h"
#include "rtcicecandidateinit.h"
#include "utilities.h"
#include "rtcpeerconnectioniceevent.h"
#include "rtcdatachannelevent.h"
#include "event.h"
#include "rtcconfiguration.h"
#include "rtcansweroptions.h"
#include "rtcofferoptions.h"
#include "rtcsessiondescriptioninit.h"

using namespace node;
using namespace v8;
using namespace node_webrtc;

Nan::Persistent<Function> PeerConnection::constructor;
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> PeerConnection::_jinglePeerConnectionFactory;

//
// PeerConnection
//

PeerConnection::PeerConnection(webrtc::PeerConnectionInterface::RTCConfiguration& configuration)
: loop(uv_default_loop())
{
  _createOfferObserver = new rtc::RefCountedObject<CreateOfferObserver>( this );
  _createAnswerObserver = new rtc::RefCountedObject<CreateAnswerObserver>( this );
  _setLocalDescriptionObserver = new rtc::RefCountedObject<SetLocalDescriptionObserver>( this );
  _setRemoteDescriptionObserver = new rtc::RefCountedObject<SetRemoteDescriptionObserver>( this );

  this->configuration = configuration;

  _jinglePeerConnection = _jinglePeerConnectionFactory->CreatePeerConnection(configuration, NULL, NULL, NULL, this);

  rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
    _jinglePeerConnectionFactory->CreateAudioTrack(
      "audio", _jinglePeerConnectionFactory->CreateAudioSource(nullptr)));
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
    _jinglePeerConnectionFactory->CreateLocalMediaStream("audio");
  stream->AddTrack(audio_track);
  _jinglePeerConnection->AddStream(stream);

  uv_mutex_init(&lock);
  uv_async_init(loop, &async, reinterpret_cast<uv_async_cb>(Run));

  async.data = this;
}

PeerConnection::~PeerConnection()
{
  TRACE_CALL;
  TRACE_END;
}

void PeerConnection::QueueEvent(AsyncEventType type, void* data)
{
  TRACE_CALL;
  AsyncEvent evt;
  evt.type = type;
  evt.data = data;
  uv_mutex_lock(&lock);
  _events.push(evt);
  uv_mutex_unlock(&lock);

  uv_async_send(&async);
  TRACE_END;
}

void PeerConnection::Run(uv_async_t* handle, int status)
{
  Nan::HandleScope scope;

  PeerConnection* self = static_cast<PeerConnection*>(handle->data);
  TRACE_CALL_P((uintptr_t)self);
  Local<Object> pc = self->handle();
  bool do_shutdown = false;

  while(true)
  {
    uv_mutex_lock(&self->lock);
    bool empty = self->_events.empty();
    if(empty)
    {
      uv_mutex_unlock(&self->lock);
      break;
    }
    AsyncEvent evt = self->_events.front();
    self->_events.pop();
    uv_mutex_unlock(&self->lock);

    TRACE_U("evt.type", evt.type);
    if (PeerConnection::CREATE_SESSION_DESCRIPTION_OBSERVER & evt.type) {
      CreateSessionDescriptionObserver* createSessionDescriptionObserver = static_cast<CreateSessionDescriptionObserver*>(evt.data);
      createSessionDescriptionObserver->HandleAsyncEvent();
      createSessionDescriptionObserver->Release();
    } else if(PeerConnection::SET_SESSION_DESCRIPTION_OBSERVER & evt.type) {
      SetSessionDescriptionObserver* setSessionDescriptionObserver = static_cast<SetSessionDescriptionObserver*>(evt.data);
      setSessionDescriptionObserver->HandleAsyncEvent();
      setSessionDescriptionObserver->Release();
    } else if(PeerConnection::ERROR_EVENT & evt.type)
    {
      PeerConnection::ErrorEvent* data = static_cast<PeerConnection::ErrorEvent*>(evt.data);
      Local<Function> callback = Local<Function>::Cast(pc->Get(Nan::New("onerror").ToLocalChecked()));
      Local<Value> argv[1];
      argv[0] = Nan::Error(data->msg.c_str());
      Nan::MakeCallback(pc, callback, 1, argv);
    } else if(PeerConnection::SDP_EVENT & evt.type)
    {
      /* PeerConnection::SdpEvent* data = static_cast<PeerConnection::SdpEvent*>(evt.data);
      Local<Function> callback = Local<Function>::Cast(pc->Get(Nan::New("onsuccess").ToLocalChecked()));
      Local<Value> argv[1];
      argv[0] = Nan::New(data->desc.c_str()).ToLocalChecked();
      Nan::MakeCallback(pc, callback, 1, argv); */
    } else if(PeerConnection::GET_STATS_SUCCESS & evt.type)
    {
      PeerConnection::GetStatsEvent* data = static_cast<PeerConnection::GetStatsEvent*>(evt.data);
      Nan::Callback *callback = data->callback;
      Local<Value> cargv[1];
      cargv[0] = Nan::New<External>(static_cast<void*>(&data->reports));
      Local<Value> argv[1];
      argv[0] = Nan::New(RTCStatsResponse::constructor)->NewInstance(1, cargv);
      callback->Call(1, argv);
    } else if(PeerConnection::SIGNALING_STATE_CHANGE & evt.type)
    {
      PeerConnection::StateEvent* data = static_cast<PeerConnection::StateEvent*>(evt.data);
      Local<Function> callback = Local<Function>::Cast(pc->Get(Nan::New("onsignalingstatechange").ToLocalChecked()));
      if(!callback.IsEmpty())
      {
        Local<Event::Event> event = Event::create(pc);
        Nan::MakeCallback(pc, callback, 1, (Local<Value>*) &event);
      }
      if(webrtc::PeerConnectionInterface::kClosed == data->state) {
        do_shutdown = true;
      }
    } else if(PeerConnection::ICE_CONNECTION_STATE_CHANGE & evt.type)
    {
      PeerConnection::StateEvent* data = static_cast<PeerConnection::StateEvent*>(evt.data);
      Local<Function> callback = Local<Function>::Cast(pc->Get(Nan::New("oniceconnectionstatechange").ToLocalChecked()));
      if (!callback.IsEmpty()) {
        Local<Event::Event> event = Event::create(pc);
        Nan::MakeCallback(pc, callback, 1, (Local<Value>*) &event);
      }
    } else if(PeerConnection::ICE_GATHERING_STATE_CHANGE & evt.type)
    {
      PeerConnection::StateEvent* data = static_cast<PeerConnection::StateEvent*>(evt.data);
      Local<Function> callback = Local<Function>::Cast(pc->Get(Nan::New("onicegatheringstatechange").ToLocalChecked()));
      if(!callback.IsEmpty())
      {
        Local<Event::Event> event = Event::create(pc);
        Nan::MakeCallback(pc, callback, 1, (Local<Value>*) &event);
      }
      if (self->_jinglePeerConnection->ice_gathering_state() == webrtc::PeerConnectionInterface::IceGatheringState::kIceGatheringComplete) {
        Local<Function> callback = Local<Function>::Cast(pc->Get(Nan::New("onicecandidate").ToLocalChecked()));
        if(!callback.IsEmpty())
        {
          Local<RTCPeerConnectionIceEvent::RTCPeerConnectionIceEvent> event = RTCPeerConnectionIceEvent::create(nullptr, pc);
          Nan::MakeCallback(pc, callback, 1, (Local<Value>*) &event);
        }
      }
    } else if(PeerConnection::ICE_CANDIDATE & evt.type)
    {
      PeerConnection::IceEvent* data = static_cast<PeerConnection::IceEvent*>(evt.data);
      Local<Function> callback = Local<Function>::Cast(pc->Get(Nan::New("onicecandidate").ToLocalChecked()));
      if(!callback.IsEmpty())
      {
        RTCIceCandidateInit::RTCIceCandidateInit candidateInit;
        candidateInit.candidate = data->candidate;
        candidateInit.sdpMid = Nan::Just<std::string>(data->sdpMid.c_str());
        candidateInit.sdpMLineIndex = Nan::Just<uint32_t>(data->sdpMLineIndex);
        RTCIceCandidate* candidate = RTCIceCandidate::create(candidateInit, NULL);
        Local<RTCPeerConnectionIceEvent::RTCPeerConnectionIceEvent> event =
          RTCPeerConnectionIceEvent::create(candidate, pc);
        Nan::MakeCallback(pc, callback, 1, (Local<Value>*) &event);
      }
    } else if(PeerConnection::NOTIFY_DATA_CHANNEL & evt.type)
    {
      PeerConnection::DataChannelEvent* data = static_cast<PeerConnection::DataChannelEvent*>(evt.data);
      DataChannelObserver* observer = data->observer;
      Local<Value> cargv[1];
      cargv[0] = Nan::New<External>(static_cast<void*>(observer));
      Local<Object> dc = Nan::New(DataChannel::constructor)->NewInstance(1, cargv);

      Local<Function> callback = Local<Function>::Cast(pc->Get(Nan::New("ondatachannel").ToLocalChecked()));
      Local<RTCDataChannelEvent::RTCDataChannelEvent> event =
        RTCDataChannelEvent::create(dc, pc);
      Nan::MakeCallback(pc, callback, 1, (Local<Value>*) &event);
    }
    v8::Isolate::GetCurrent()->RunMicrotasks();
  }

  if(do_shutdown) {
    uv_close((uv_handle_t*)(&self->async), NULL);
  }

  TRACE_END;
}

void PeerConnection::OnError() {
  TRACE_CALL;
  TRACE_END;
}

void PeerConnection::OnSignalingChange( webrtc::PeerConnectionInterface::SignalingState new_state ) {
  TRACE_CALL;
  StateEvent* data = new StateEvent(static_cast<uint32_t>(new_state));
  QueueEvent(PeerConnection::SIGNALING_STATE_CHANGE, static_cast<void*>(data));
  TRACE_END;
}

void PeerConnection::OnIceConnectionChange( webrtc::PeerConnectionInterface::IceConnectionState new_state ) {
  TRACE_CALL;
  StateEvent* data = new StateEvent(static_cast<uint32_t>(new_state));
  QueueEvent(PeerConnection::ICE_CONNECTION_STATE_CHANGE, static_cast<void*>(data));
  TRACE_END;
}

void PeerConnection::OnIceGatheringChange( webrtc::PeerConnectionInterface::IceGatheringState new_state ) {
  TRACE_CALL;
  StateEvent* data = new StateEvent(static_cast<uint32_t>(new_state));
  QueueEvent(PeerConnection::ICE_GATHERING_STATE_CHANGE, static_cast<void*>(data));
  TRACE_END;
}

void PeerConnection::OnIceCandidate( const webrtc::IceCandidateInterface* candidate ) {
  TRACE_CALL;
  PeerConnection::IceEvent* data = new PeerConnection::IceEvent(candidate);
  QueueEvent(PeerConnection::ICE_CANDIDATE, static_cast<void*>(data));
  TRACE_END;
}

void PeerConnection::OnDataChannel( webrtc::DataChannelInterface* jingle_data_channel ) {
  TRACE_CALL;
  DataChannelObserver* observer = new DataChannelObserver(jingle_data_channel);
  PeerConnection::DataChannelEvent* data = new PeerConnection::DataChannelEvent(observer);
  QueueEvent(PeerConnection::NOTIFY_DATA_CHANNEL, static_cast<void*>(data));
  TRACE_END;
}

void PeerConnection::OnAddStream(webrtc::MediaStreamInterface* stream) {
  TRACE_CALL;
  TRACE_END;
}

void PeerConnection::OnRemoveStream(webrtc::MediaStreamInterface* stream) {
  TRACE_CALL;
  TRACE_END;
}

void PeerConnection::OnRenegotiationNeeded() {
  TRACE_CALL;
  TRACE_END;
}

NAN_METHOD(PeerConnection::New) {
  TRACE_CALL;

  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Use the new operator to construct the PeerConnection.");
  }

  webrtc::PeerConnectionInterface::RTCConfiguration configuration;
  Local<Value> configurationValue = info[0];
  if (!configurationValue.IsEmpty() && !configurationValue->IsNull() && !configurationValue->IsUndefined()) {
    std::string error;
    if (!RTCConfiguration::fromNode(configurationValue, configuration, &error)) {
      Nan::ThrowTypeError(error.c_str());
      TRACE_END;
      return;
    }
  }

  PeerConnection* obj = new PeerConnection(configuration);
  obj->Wrap(info.This());
  obj->Ref();

  info.GetReturnValue().Set(info.This());
  TRACE_END;
}

NAN_METHOD(PeerConnection::CreateOffer) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.This());

  // Save the onSuccess/onFailure callbacks in a CreateSessionDescriptionObserver.
  Nan::Callback *onSuccess = new Nan::Callback(info[0].As<Function>());
  Nan::Callback *onFailure = new Nan::Callback(info[1].As<Function>());
  rtc::RefCountedObject<CreateSessionDescriptionObserver>* createSessionDescriptionObserver =
    new rtc::RefCountedObject<CreateSessionDescriptionObserver>(self, onSuccess, onFailure);
  createSessionDescriptionObserver->AddRef();

  // Parse any RTCOfferOptions passed in.
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
  if (info.Length() == 3) {
    std::string error;
    if (!RTCOfferOptions::fromNode(info[2], options, &error)) {
      Nan::ThrowTypeError(Nan::New(error).ToLocalChecked());
      TRACE_END;
      return;
    }
  }

  // TODO(mroberts): As of M48, PeerConnectionProxy does not yet proxy the
  // CreateOffer method which accepts RTCOfferOptions; so we need to convert
  // to FakeConstraints. Eventually we should pass the options directly.
  webrtc::FakeConstraints constraints = convertRTCOfferAnswerOptionsToFakeConstraints(options);

  self->_jinglePeerConnection->CreateOffer(createSessionDescriptionObserver,
    &constraints);

  info.GetReturnValue().Set(createSessionDescriptionObserver->GetPromise());
  TRACE_END;
}

NAN_METHOD(PeerConnection::CreateAnswer) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.This());

  // Save the onSuccess/onFailure callbacks in a CreateSessionDescriptionObserver.
  Nan::Callback *onSuccess = new Nan::Callback(info[0].As<Function>());
  Nan::Callback *onFailure = new Nan::Callback(info[1].As<Function>());
  rtc::RefCountedObject<CreateSessionDescriptionObserver>* createSessionDescriptionObserver =
    new rtc::RefCountedObject<CreateSessionDescriptionObserver>(self, onSuccess, onFailure);
  createSessionDescriptionObserver->AddRef();

  // Parse any RTCAnswerOptions passed in.
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
  if (info.Length() == 3) {
    std::string error;
    if (!RTCAnswerOptions::fromNode(info[2], options, &error)) {
      Nan::ThrowTypeError(Nan::New(error).ToLocalChecked());
      TRACE_END;
      return;
    }
  }

  // TODO(mroberts): As of M48, PeerConnectionProxy does not yet proxy the
  // CreateAnswer method which accepts RTCOfferOptions; so we need to convert
  // to FakeConstraints. Eventually we should pass the options directly.
  webrtc::FakeConstraints constraints = convertRTCOfferAnswerOptionsToFakeConstraints(options);

  self->_jinglePeerConnection->CreateAnswer(createSessionDescriptionObserver,
    &constraints);

  info.GetReturnValue().Set(createSessionDescriptionObserver->GetPromise());
  TRACE_END;
}

NAN_METHOD(PeerConnection::SetLocalDescription) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.This());

  std::string error;
  RTCSessionDescriptionInit::RTCSessionDescriptionInit descriptionInit;
  if (!RTCSessionDescriptionInit::fromNode(info[0], descriptionInit, &error)) {
    Nan::ThrowError(Nan::New(error).ToLocalChecked());
    TRACE_END;
    return;
  }

  // Parse the RTCSessionDescription.
  RTCSessionDescription* description = RTCSessionDescription::create(descriptionInit, &error);
  if (!description) {
    Nan::ThrowError(Nan::New(error).ToLocalChecked());
    TRACE_END;
    return;
  }

  // Save the onSuccess/onFailure callbacks in a SetSessionDescriptionObserver.
  Nan::Callback *onSuccess = new Nan::Callback(info[1].As<Function>());
  Nan::Callback *onFailure = new Nan::Callback(info[2].As<Function>());

  self->pendingLocalDescription = description;
  rtc::RefCountedObject<SetSessionDescriptionObserver>* setSessionDescriptionObserver =
    new rtc::RefCountedObject<SetSessionDescriptionObserver>(self, description,
      true, onSuccess, onFailure);
  setSessionDescriptionObserver->AddRef();

  self->_jinglePeerConnection->SetLocalDescription(setSessionDescriptionObserver,
    description->getSessionDescriptionInterface());

  info.GetReturnValue().Set(setSessionDescriptionObserver->GetPromise());
  TRACE_END;
}

NAN_METHOD(PeerConnection::SetRemoteDescription) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.This());

  std::string error;
  RTCSessionDescriptionInit::RTCSessionDescriptionInit descriptionInit;
  if (!RTCSessionDescriptionInit::fromNode(info[0], descriptionInit, &error)) {
    Nan::ThrowError(Nan::New(error).ToLocalChecked());
    TRACE_END;
    return;
  }

  // Parse the RTCSessionDescription.
  RTCSessionDescription* description = RTCSessionDescription::create(descriptionInit, &error);
  if (!description) {
    Nan::ThrowError(Nan::New(error).ToLocalChecked());
    TRACE_END;
    return;
  }

  // Save the onSuccess/onFailure callbacks in a SetSessionDescriptionObserver.
  Nan::Callback *onSuccess = new Nan::Callback(info[1].As<Function>());
  Nan::Callback *onFailure = new Nan::Callback(info[2].As<Function>());

  self->pendingRemoteDescription = description;
  rtc::RefCountedObject<SetSessionDescriptionObserver>* setSessionDescriptionObserver =
    new rtc::RefCountedObject<SetSessionDescriptionObserver>(self, description,
      false, onSuccess, onFailure);
  setSessionDescriptionObserver->AddRef();

  self->_jinglePeerConnection->SetRemoteDescription(setSessionDescriptionObserver,
    description->getSessionDescriptionInterface());

  info.GetReturnValue().Set(setSessionDescriptionObserver->GetPromise());
  TRACE_END;
}

NAN_METHOD(PeerConnection::AddStream)
{
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.This());
  MediaStream* mediaStream = Nan::ObjectWrap::Unwrap<MediaStream>(info[0].As<Object>());
  self->_jinglePeerConnection->AddStream(mediaStream->getStream());
  TRACE_END;
}

NAN_METHOD(PeerConnection::AddIceCandidate) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.This());

  Local<Promise::Resolver> resolver = v8::Promise::Resolver::New(Isolate::GetCurrent());
  Local<Promise> promise = resolver->GetPromise();

  Nan::Callback *onSuccess = new Nan::Callback(info[1].As<Function>());
  Nan::Callback *onFailure = new Nan::Callback(info[2].As<Function>());
  promise->Then(onSuccess->GetFunction());
  promise->Catch(onFailure->GetFunction());

  std::string error;
  RTCIceCandidateInit::RTCIceCandidateInit candidateInit;
  if (!RTCIceCandidateInit::fromNode(info[0], candidateInit, &error)) {
    Nan::ThrowError(Nan::New(error).ToLocalChecked());
    TRACE_END;
    return;
  }

  RTCIceCandidate* candidate = RTCIceCandidate::create(candidateInit, &error);
  if (!candidate) {
    Nan::ThrowError(Nan::New(error).ToLocalChecked());
    TRACE_END;
    return;
  }

  webrtc::IceCandidateInterface* ci = candidate->getIceCandidateInterface();
  if (!self->_jinglePeerConnection->AddIceCandidate(ci)) {
    PeerConnection::ErrorEvent* data = new PeerConnection::ErrorEvent(std::string("Failed to set ICE candidate."));
    self->QueueEvent(PeerConnection::ADD_ICE_CANDIDATE_ERROR, static_cast<void*>(data));
    resolver->Reject(Nan::Error(Nan::New("Failed to set ICE candidate.").ToLocalChecked()));
    TRACE_END;
    return info.GetReturnValue().Set(promise);
  }

  self->QueueEvent(PeerConnection::ADD_ICE_CANDIDATE_SUCCESS, static_cast<void*>(NULL));
  resolver->Resolve(Nan::Undefined());
  TRACE_END;
  return info.GetReturnValue().Set(promise);
}

NAN_METHOD(PeerConnection::CreateDataChannel) {
  TRACE_CALL;

  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>( info.This() );
  String::Utf8Value label(info[0]->ToString());
  Handle<Object> dataChannelDict = Handle<Object>::Cast(info[1]);

  std::string error;
  webrtc::DataChannelInit* dataChannelInit = parseDataChannelInit(info[1], &error);
  if (!dataChannelInit) {
    TRACE_END;
    return Nan::ThrowTypeError(error.c_str());
  }

  rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel_interface = self->_jinglePeerConnection->CreateDataChannel(*label, dataChannelInit);
  DataChannelObserver* observer = new DataChannelObserver(data_channel_interface);

  Local<Value> cargv[1];
  cargv[0] = Nan::New<External>(static_cast<void*>(observer));
  Local<Value> dc = Nan::New(DataChannel::constructor)->NewInstance(1, cargv);

  TRACE_END;
  info.GetReturnValue().Set(dc);
}

NAN_METHOD(PeerConnection::GetConfiguration) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.This());
  info.GetReturnValue().Set(RTCConfiguration::toNode(&self->configuration));
  TRACE_END;
}

NAN_METHOD(PeerConnection::GetStats) {
  TRACE_CALL;

  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>( info.This() );

  Nan::Callback *onSuccess = new Nan::Callback(info[0].As<Function>());
  Nan::Callback *onFailure = new Nan::Callback(info[1].As<Function>());
  rtc::scoped_refptr<StatsObserver> statsObserver =
     new rtc::RefCountedObject<StatsObserver>( self, onSuccess );

  if (!self->_jinglePeerConnection->GetStats(statsObserver, NULL,
    webrtc::PeerConnectionInterface::kStatsOutputLevelStandard))
  {
    // TODO: Include error?
    Local<Value> argv[] = {
      Nan::Null()
    };
    onFailure->Call(1, argv);
  }

  TRACE_END;
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(PeerConnection::UpdateIce) {
  TRACE_CALL;
  TRACE_END;
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(PeerConnection::Close) {
  TRACE_CALL;

  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>( info.This() );
  self->_jinglePeerConnection->Close();

  TRACE_END;
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_GETTER(PeerConnection::GetLocalDescription) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());

  // http://w3c.github.io/webrtc-pc
  //
  //   The localDescription attribute must return pendingLocalDescription if
  //   it is not null and otherwise it must return currentLocalDescription.
  //
  Local<Value> localDescription;
  if (self->pendingLocalDescription) {
    localDescription = self->pendingLocalDescription->handle();
  } else if (self->currentLocalDescription) {
    localDescription = self->currentLocalDescription->handle();
  } else {
    localDescription = Nan::Null();
  }

  info.GetReturnValue().Set(localDescription);
  TRACE_END;
}

NAN_GETTER(PeerConnection::GetRemoteDescription) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());

  // http://w3c.github.io/webrtc-pc
  //
  //   The remoteDescription attribute must return pendingRemoteDescription if
  //   it is not null and otherwise it must return currentLocalDescription.
  //
  Local<Value> remoteDescription;
  if (self->pendingRemoteDescription) {
    remoteDescription = self->pendingRemoteDescription->handle();
  } else if (self->currentRemoteDescription) {
    remoteDescription = self->currentRemoteDescription->handle();
  } else {
    remoteDescription = Nan::Null();
  }

  info.GetReturnValue().Set(remoteDescription);
  TRACE_END;
}

NAN_GETTER(PeerConnection::GetSignalingState) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  info.GetReturnValue().Set(signalingStateToString(
    self->_jinglePeerConnection->signaling_state()));
  TRACE_END;
}

NAN_GETTER(PeerConnection::GetIceConnectionState) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  info.GetReturnValue().Set(iceConnectionStateToString(
    self->_jinglePeerConnection->ice_connection_state()));
  TRACE_END;
}

NAN_GETTER(PeerConnection::GetIceGatheringState) {
  TRACE_CALL;
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  info.GetReturnValue().Set(iceGatheringStateToString(
    self->_jinglePeerConnection->ice_gathering_state()));
  TRACE_END;
}

NAN_SETTER(PeerConnection::ReadOnly) {
  INFO("PeerConnection::ReadOnly");
}

void PeerConnection::Init(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory, Handle<Object> exports ) {
  _jinglePeerConnectionFactory = peerConnectionFactory;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>( New );
  tpl->SetClassName( Nan::New( "PeerConnection").ToLocalChecked() );
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  tpl->PrototypeTemplate()->Set( Nan::New( "createOffer" ).ToLocalChecked(),
    Nan::New<FunctionTemplate>( CreateOffer )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "createAnswer" ).ToLocalChecked(),
    Nan::New<FunctionTemplate>( CreateAnswer )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "setLocalDescription").ToLocalChecked(),
    Nan::New<FunctionTemplate>( SetLocalDescription )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "setRemoteDescription").ToLocalChecked(),
    Nan::New<FunctionTemplate>( SetRemoteDescription )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "getConfiguration" ).ToLocalChecked(),
    Nan::New<FunctionTemplate>( GetConfiguration )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "getStats" ).ToLocalChecked(),
    Nan::New<FunctionTemplate>( GetStats )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "updateIce" ).ToLocalChecked(),
    Nan::New<FunctionTemplate>( UpdateIce )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "addStream" ).ToLocalChecked(),
    Nan::New<FunctionTemplate>( AddStream )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "addIceCandidate" ).ToLocalChecked(),
    Nan::New<FunctionTemplate>( AddIceCandidate )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "createDataChannel" ).ToLocalChecked(),
    Nan::New<FunctionTemplate>( CreateDataChannel )->GetFunction() );

  tpl->PrototypeTemplate()->Set( Nan::New( "close" ).ToLocalChecked(),
    Nan::New<FunctionTemplate>( Close )->GetFunction() );

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("localDescription").ToLocalChecked(), GetLocalDescription, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("remoteDescription").ToLocalChecked(), GetRemoteDescription, nullptr);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("signalingState").ToLocalChecked(), GetSignalingState, ReadOnly);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("iceConnectionState").ToLocalChecked(), GetIceConnectionState, ReadOnly);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("iceGatheringState").ToLocalChecked(), GetIceGatheringState, ReadOnly);

  constructor.Reset(tpl->GetFunction());
  exports->Set( Nan::New("PeerConnection").ToLocalChecked(), tpl->GetFunction() );
}
