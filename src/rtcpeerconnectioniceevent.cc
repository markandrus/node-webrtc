#include "event.h"
#include "rtcpeerconnectioniceevent.h"

using namespace node_webrtc;
using namespace v8;

Local<RTCPeerConnectionIceEvent::RTCPeerConnectionIceEvent> RTCPeerConnectionIceEvent::create(Local<Value> candidate, Local<Value> target)
{
  Nan::EscapableHandleScope scope;
  Local<RTCPeerConnectionIceEvent> event = node_webrtc::Event::create(target);
  Nan::Set(event, Nan::New("candidate").ToLocalChecked(), candidate);
  // TODO(mroberts): Pass in the URL.
  // Nan::Set(event, Nan::New("url").ToLocalChecked(), url);
  return scope.Escape(event);
}

Local<RTCPeerConnectionIceEvent::RTCPeerConnectionIceEvent> RTCPeerConnectionIceEvent::create(node_webrtc::RTCIceCandidate* candidate, Local<Value> target)
{
  Nan::EscapableHandleScope scope;
  return scope.Escape(create(candidate
      ? static_cast<Local<Value>>(candidate->handle())
      : static_cast<Local<Value>>(Nan::Null()),
    target));
}
