#ifndef _MESSAGEEVENT_H_
#define _MESSAGEEVENT_H_

#include <nan.h>
#include <v8.h>

namespace node_webrtc {

namespace MessageEvent {

  typedef v8::Object MessageEvent;

  v8::Local<MessageEvent> create(v8::Local<v8::Value> data, v8::Local<v8::Value> target);

}

}

#endif  // _MESSAGEEVENT_H_
