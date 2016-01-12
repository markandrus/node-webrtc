#ifndef _CREATESESSIONDESCRIPTIONOBSERVER_H_
#define _CREATESESSIONDESCRIPTIONOBSERVER_H_

#include "asyncevent2.h"
#include "rtcpeerconnection.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include <nan.h>

namespace node_webrtc {

class CreateSessionDescriptionObserver
: public AsyncEvent2
, public webrtc::CreateSessionDescriptionObserver
{
 public:
  CreateSessionDescriptionObserver(
      PeerConnection* pc,
      Nan::Callback* onSuccess,
      Nan::Callback* onFailure):
    AsyncEvent2(
      pc,
      PeerConnection::CREATE_SESSION_DESCRIPTION_OBSERVER,
      onSuccess,
      onFailure) {}

  //
  // AsyncEvent2 implementation
  //
  virtual void HandleAsyncEvent();

  //
  // webrtc::CreateSessionDescriptionObserver implementation
  //
  virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc);
  virtual void OnFailure(const std::string& error);

 private:
  webrtc::SessionDescriptionInterface* desc = NULL;
  std::string* error = NULL;
};

}

#endif  // _CREATESESSIONDESCRIPTIONOBSERVER_H
