#ifndef _EVENT_H_
#define _EVENT_H_

#include <nan.h>
#include <v8.h>

namespace node_webrtc {

namespace Event {

  typedef v8::Object Event;

  v8::Local<Event> create(v8::Local<v8::Value> target);

}

}

#endif  // _EVENT_H_
