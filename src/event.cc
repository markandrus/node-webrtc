#include "event.h"

using namespace node_webrtc;
using namespace v8;

Local<Event::Event> Event::create(Local<Value> target)
{
  Nan::EscapableHandleScope scope;
  Local<Event> event = Nan::New<Event>();
  Nan::Set(event, Nan::New("target").ToLocalChecked(), target);
  return scope.Escape(event);
}
