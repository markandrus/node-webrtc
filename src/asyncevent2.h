#ifndef _ASYNCEVENT2_H_
#define _ASYNCEVENT2_H_

#include <nan.h>
#include <v8.h>

#include "rtcpeerconnection.h"

namespace node_webrtc {

class AsyncEvent2 {
 public:
  AsyncEvent2(
    PeerConnection* pc,
    PeerConnection::AsyncEventType asyncEventType,
    Nan::Callback* onSuccess,
    Nan::Callback* onFailure);

  ~AsyncEvent2();

  // NOTE(mroberts): This must be called on the main thread.
  virtual void HandleAsyncEvent() = 0;

  virtual v8::Local<v8::Promise> GetPromise();

 protected:
  // TODO(mroberts): Implement these.
  virtual void RejectWithError(const std::string& error);
  virtual void RejectWithError(std::string* error);
  virtual void Resolve(v8::Local<v8::Value> value);
  virtual void Done();
  PeerConnection* pc;
  Nan::Persistent<v8::Promise::Resolver>* resolver;

  Nan::Callback* onSuccess;
  Nan::Callback* onFailure;

 private:
  PeerConnection::AsyncEventType asyncEventType;
};

}

#endif  // _ASYNCEVENT2_H_
