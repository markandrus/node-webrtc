#include "event.h"
#include "data/messageevent.h"

using namespace node_webrtc;
using namespace v8;

Local<MessageEvent::MessageEvent> MessageEvent::create(Local<Value> data, Local<Value> target)
{
  Nan::EscapableHandleScope scope;
  Local<MessageEvent> event = node_webrtc::Event::create(target);
  Nan::Set(event, Nan::New("data").ToLocalChecked(), data);
  return scope.Escape(event);
}
