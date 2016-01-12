#include "asyncevent2.h"

using namespace node_webrtc;
using namespace v8;

AsyncEvent2::AsyncEvent2(
    PeerConnection* pc,
    PeerConnection::AsyncEventType asyncEventType,
    Nan::Callback* onSuccess,
    Nan::Callback* onFailure):
  pc(pc),
  onSuccess(onSuccess),
  onFailure(onFailure),
  asyncEventType(asyncEventType)
{
  this->resolver = new Nan::Persistent<Promise::Resolver>(
    Promise::Resolver::New(Isolate::GetCurrent()));
  Local<Promise> promise = this->GetPromise();
  if (onSuccess) {
    promise->Then(this->onSuccess->GetFunction());
  }
  if (onFailure) {
    promise->Catch(this->onFailure->GetFunction());
  }
}

AsyncEvent2::~AsyncEvent2()
{
  this->resolver->Reset();
  delete this->resolver;
}

Local<Promise> AsyncEvent2::GetPromise()
{
  Nan::EscapableHandleScope scope;
  return scope.Escape(Nan::New<Promise::Resolver>(*this->resolver)->GetPromise());
}

void AsyncEvent2::RejectWithError(const std::string& error)
{
  TRACE_CALL;
  Nan::HandleScope scope;
  Local<Value> reason = Nan::Error(Nan::New(error).ToLocalChecked());
  Nan::New<Promise::Resolver>(*this->resolver)->Reject(reason);
  TRACE_END;
}

void AsyncEvent2::RejectWithError(std::string* error)
{
  TRACE_CALL;
  Nan::HandleScope scope;
  Local<Value> reason = Nan::Error(Nan::New(error->c_str()).ToLocalChecked());
  Nan::New<Promise::Resolver>(*this->resolver)->Reject(reason);
  TRACE_END;
}

void AsyncEvent2::Resolve(Local<Value> value)
{
  TRACE_CALL;
  Nan::HandleScope scope;
  Nan::New<Promise::Resolver>(*this->resolver)->Resolve(value);
  TRACE_END;
}

void AsyncEvent2::Done()
{
  this->pc->QueueEvent(this->asyncEventType, static_cast<void*>(this));
};
