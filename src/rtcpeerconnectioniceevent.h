#ifndef _RTCPEERCONNECTIONICEEVENT_H_
#define _RTCPEERCONNECTIONICEEVENT_H_

#include <nan.h>
#include <v8.h>
#include "rtcicecandidate.h"

namespace node_webrtc {

namespace RTCPeerConnectionIceEvent {

  typedef v8::Object RTCPeerConnectionIceEvent;

  v8::Local<RTCPeerConnectionIceEvent> create(v8::Local<v8::Value> candidate, v8::Local<v8::Value> target);

  v8::Local<RTCPeerConnectionIceEvent> create(node_webrtc::RTCIceCandidate* candidate, v8::Local<v8::Value> target);

}

}

#endif  // _RTCPEERCONNECTIONICEEVENT_H_
