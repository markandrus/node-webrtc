#include "event.h"
#include "rtcdatachannelevent.h"

using namespace node_webrtc;
using namespace v8;

// Local<::node_webrtc::RTCDataChannelEvent::RTCDataChannelEvent> create(Local<Object> channel, Local<Value> target)
Local<RTCDataChannelEvent::RTCDataChannelEvent> RTCDataChannelEvent::create(Local<Object> channel, Local<Value> target)
{
  Nan::EscapableHandleScope scope;
  Local<::node_webrtc::RTCDataChannelEvent::RTCDataChannelEvent> event = node_webrtc::Event::create(target);
  Nan::Set(event, Nan::New("channel").ToLocalChecked(), channel);
  return scope.Escape(event);
}

Local<RTCDataChannelEvent::RTCDataChannelEvent> RTCDataChannelEvent::create(node_webrtc::DataChannel* channel, Local<Value> target)
{
  Nan::EscapableHandleScope scope;
  return scope.Escape(create(channel->handle(), target));
}
