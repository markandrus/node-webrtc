#include "common.h"
#include "rtcsessiondescription.h"
#include "talk/app/webrtc/jsep.h"
#include "utilities.h"

using namespace node_webrtc;
using namespace v8;
using namespace webrtc;

Nan::Persistent<Function> RTCSessionDescription::constructor;

void RTCSessionDescription::Init(Handle<Object> exports)
{
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("RTCSessionDescription").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // tpl->PrototypeTemplate()->Set(Nan::New("toJSON").ToLocalChecked(),
  //  Nan::New<FunctionTemplate>(ToJSON)->GetFunction());
  Nan::SetPrototypeMethod(tpl, "toJSON", ToJSON);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("sdp").ToLocalChecked(), GetSdp, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("type").ToLocalChecked(), GetType, nullptr);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("RTCSessionDescription").ToLocalChecked(), tpl->GetFunction());
}

NAN_PROPERTY_GETTER(RTCSessionDescription::GetSdp) {
  TRACE_CALL;

  RTCSessionDescription* self = Nan::ObjectWrap::Unwrap<RTCSessionDescription>(info.This());
  std::string sdp;
  self->desc->ToString(&sdp);

  TRACE_END;
  info.GetReturnValue().Set(Nan::New(sdp).ToLocalChecked());
}

NAN_PROPERTY_GETTER(RTCSessionDescription::GetType) {
  TRACE_CALL;

  RTCSessionDescription* self = Nan::ObjectWrap::Unwrap<RTCSessionDescription>(info.This());
  std::string type = self->desc->type();

  TRACE_END;
  info.GetReturnValue().Set(Nan::New(type).ToLocalChecked());
}

NAN_METHOD(RTCSessionDescription::New) {
  TRACE_CALL;

  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Use the new operator to construct the RTCSessionDescription.");
  }

  std::string error;
  SessionDescriptionInterface* sdi = parseSessionDescription(info[0], &error);
  if (!sdi) {
    TRACE_END;
    return Nan::ThrowError(Nan::Error(Nan::New(error).ToLocalChecked()));
  }

  RTCSessionDescription* obj = new RTCSessionDescription(sdi);
  obj->Wrap(info.This());
  obj->Ref();
  
  TRACE_END;
  info.GetReturnValue().Set(obj->handle());
}

NAN_METHOD(RTCSessionDescription::ToJSON) {
  TRACE_CALL;

  RTCSessionDescription* self = Nan::ObjectWrap::Unwrap<RTCSessionDescription>(info.This());
  Local<Object> json = convertSessionDescriptionInterfaceToJson(self->desc);

  TRACE_END;
  info.GetReturnValue().Set(json);
}

RTCSessionDescription* RTCSessionDescription::create(RTCSessionDescriptionInit::RTCSessionDescriptionInit& descriptionInit, std::string *error)
{
  Nan::EscapableHandleScope scope;

  Local<Function> cons = Nan::New(RTCSessionDescription::constructor);
  Local<Value> cargv[] = {
    RTCSessionDescriptionInit::toNode(&descriptionInit)
  };
  Local<Object> wrapped = Nan::NewInstance(cons, 1, cargv).ToLocalChecked();
  RTCSessionDescription* description = Nan::ObjectWrap::Unwrap<RTCSessionDescription>(wrapped);

  return description;
}
