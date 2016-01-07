#ifndef _ASYNCEVENT2_H_
#define _ASYNCEVENT2_H_

#include <iostream>
#include <nan.h>
#include <v8.h>
#include "peerconnection.h"

namespace node_webrtc {

class AsyncEvent2 {
 public:
  AsyncEvent2(
      PeerConnection* pc,
      PeerConnection::AsyncEventType asyncEventType,
      Nan::Callback* onSuccess,
      Nan::Callback* onFailure):
    pc(pc),
    asyncEventType(asyncEventType),
    onSuccess(onSuccess),
    onFailure(onFailure)
  {
    this->resolver = new Nan::Persistent<v8::Promise::Resolver>(v8::Promise::Resolver::New(v8::Isolate::GetCurrent()));
    if (onSuccess || onFailure) {
      Local<Promise> promise = this->GetPromise();
      if (onSuccess) {
        promise->Then(this->onSuccess->GetFunction());
      }
      if (onFailure) {
        promise->Catch(this->onFailure->GetFunction());
      }
    }
  }

  ~AsyncEvent2()
  {
    std::cout << "BROK ROK E\n";
    this->resolver->Reset();
    delete this->resolver;
  }

  // NOTE(mroberts): This must be called on the main thread.
  virtual void HandleAsyncEvent() = 0;

  virtual v8::Local<v8::Promise> GetPromise() {
    Nan::EscapableHandleScope scope;
    return scope.Escape(Nan::New<v8::Promise::Resolver>(*resolver)->GetPromise());
  }

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
