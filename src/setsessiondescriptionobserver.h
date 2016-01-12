#ifndef _SETSESSIONDESCRIPTIONOBSERVER_H_
#define _SETSESSIONDESCRIPTIONOBSERVER_H_

#include <nan.h>

#include "asyncevent2.h"
#include "rtcpeerconnection.h"

namespace node_webrtc {

class SetSessionDescriptionObserver
: public AsyncEvent2
, public webrtc::SetSessionDescriptionObserver
{
 public:
  SetSessionDescriptionObserver(
      PeerConnection* pc,
      RTCSessionDescription* description,
      bool local,
      Nan::Callback* onSuccess,
      Nan::Callback* onFailure):
    AsyncEvent2(
      pc,
      PeerConnection::SET_SESSION_DESCRIPTION_OBSERVER,
      onSuccess,
      onFailure),
    description(description),
    local(local) {}

  //
  // AsyncEvent2 implementation
  //
  virtual void HandleAsyncEvent();

  //
  // webrtc::SetSessionDescriptionObserver implementation
  //
  virtual void OnSuccess();
  virtual void OnFailure(const std::string& error);

 private:
  RTCSessionDescription* description;
  bool local;
  std::string* error = NULL;
};

}

#endif  // _SETSESSIONDESCRIPTIONOBSERVER_H_
