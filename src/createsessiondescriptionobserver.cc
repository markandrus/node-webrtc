#include <iostream>
#include <v8.h>
#include "common.h"
#include "createsessiondescriptionobserver.h"
#include "peerconnection.h"
#include "rtcsessiondescription.h"
#include "utilities.h"

using namespace node_webrtc;
using namespace v8;

void CreateSessionDescriptionObserver::HandleAsyncEvent()
{
  TRACE_CALL;
  Nan::HandleScope scope;

  if (this->error) {
    this->RejectWithError(this->error);
    TRACE_END;
    return;
  }

  Local<Object> descriptionInitDict = convertSessionDescriptionInterfaceToJson(this->desc);
  Local<Value> argv[1] = { descriptionInitDict };
  Local<Function> cons = Nan::New(RTCSessionDescription::constructor);
  Local<Value> description = Nan::NewInstance(cons, 1, argv).ToLocalChecked();
  this->Resolve(description);
  TRACE_END;
  /*
  TRACE_CALL;
  Nan::HandleScope scope;

  Local<Promise::Resolver> resolver = Nan::New<Promise::Resolver>(*this->resolver);

  Local<Value> argv[1];

  if (this->error != NULL) {
    Local<Value> error = Nan::Error(Nan::New(this->error->c_str()).ToLocalChecked());
    argv[0] = error;
    resolver->GetPromise()->Catch(this->onFailure->GetFunction());
    resolver->Reject(error);
    TRACE_END;
    return;
  }

  // TODO(mroberts): Can we simply ObjectWrap instead of converting to and
  // from JSON in the constructor call?
  Local<Object> json = convertSessionDescriptionInterfaceToJson(this->desc);
  Local<Value> cargv[1] = {
    json
  };
  Local<Function> cons = Nan::New(RTCSessionDescription::constructor);
  Local<Value> desc = Nan::NewInstance(cons, 1, cargv).ToLocalChecked();
  argv[0] = desc;

  TRACE_END;
  resolver->GetPromise()->Then(this->onSuccess->GetFunction());
  resolver->Resolve(desc);
  */
}

void CreateSessionDescriptionObserver::OnFailure(const std::string& error)
{
  TRACE_CALL;
  this->error = new std::string(error);
  this->Done();
  TRACE_END;
}

void CreateSessionDescriptionObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
  TRACE_CALL;
  this->desc = desc;
  this->Done();
  /* Nan::HandleScope scope;
  Local<Object> descriptionInitDict = convertSessionDescriptionInterfaceToJson(this->desc);
  Local<Value> argv[1] = { descriptionInitDict };
  Local<Function> cons = Nan::New(RTCSessionDescription::constructor);
  Local<Value> description = Nan::NewInstance(cons, 1, argv).ToLocalChecked();
  // this->Resolve(description);
  */
  TRACE_END;
}
