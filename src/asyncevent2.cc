#include "asyncevent2.h"
#include "peerconnection.h"

using namespace node_webrtc;
using namespace v8;

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
