#ifndef _RTCDATACHANNELEVENT_H_
#define _RTCDATACHANNELEVENT_H_

#include <nan.h>
#include <v8.h>

#include "data/datachannel.h"

namespace node_webrtc {

// The RTCDataChannelEvents used in node-webrtc are lightweight and short-lived.
// Truly it should inherit from some base Event class, but we can get away with
// plain Objects.
namespace RTCDataChannelEvent {

  typedef v8::Object RTCDataChannelEvent;

  v8::Local<RTCDataChannelEvent> create(v8::Local<v8::Object> channel, v8::Local<v8::Value> target);

  v8::Local<RTCDataChannelEvent> create(node_webrtc::DataChannel* channel, v8::Local<v8::Value> target);

}

}

#endif  // _RTCDATACHANNELEVENT_H_
