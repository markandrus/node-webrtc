#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include "talk/app/webrtc/jsep.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/test/fakeconstraints.h"
#include <nan.h>
#include <v8.h>

namespace node_webrtc {

webrtc::FakeConstraints convertRTCOfferAnswerOptionsToFakeConstraints(const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options);

webrtc::SessionDescriptionInterface* parseSessionDescription(v8::Local<v8::Value> value, std::string* error);

v8::Local<v8::Object> convertSessionDescriptionInterfaceToJson(webrtc::SessionDescriptionInterface* sdi);

webrtc::DataChannelInit* parseDataChannelInit(v8::Local<v8::Value> value, std::string* error);

v8::Local<v8::String> signalingStateToString(webrtc::PeerConnectionInterface::SignalingState state);

v8::Local<v8::String> iceConnectionStateToString(webrtc::PeerConnectionInterface::IceConnectionState state);

v8::Local<v8::String> iceGatheringStateToString(webrtc::PeerConnectionInterface::IceGatheringState state);

webrtc::PeerConnectionInterface::RTCConfiguration* parseRTCConfiguration(v8::Local<v8::Value> value, std::string* error);

}

#endif  // _UTILITIES_H_
