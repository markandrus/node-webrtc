#include "common.h"
#include "rtcicecandidate.h"
#include "talk/app/webrtc/jsep.h"
#include "utilities.h"

using namespace node_webrtc;
using namespace v8;
using namespace webrtc;

Nan::Persistent<Function> RTCIceCandidate::constructor;

void RTCIceCandidate::Init(Handle<Object> exports)
{
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("RTCIceCandidate").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Nan::SetPrototypeMethod(tpl, "toJSON", ToJSON);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("candidate").ToLocalChecked(), GetCandidate, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("sdpMid").ToLocalChecked(), GetSdpMid, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("sdpMLineIndex").ToLocalChecked(), GetSdpMLineIndex, nullptr);

  /* Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("foundation").ToLocalChecked(), GetFoundation, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("priority").ToLocalChecked(), GetPriority, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("ip").ToLocalChecked(), GetIp, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("protocol").ToLocalChecked(), GetProtocol, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("port").ToLocalChecked(), GetPort, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("type").ToLocalChecked(), GetType, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("tcpType").ToLocalChecked(), GetTcpType, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("relatedAddress").ToLocalChecked(), GetRelatedAddress, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("relatedPort").ToLocalChecked(), GetRelatedPort, nullptr); */

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("RTCIceCandidate").ToLocalChecked(), tpl->GetFunction());
}

NAN_PROPERTY_GETTER(RTCIceCandidate::GetCandidate) {
  TRACE_CALL;

  RTCIceCandidate* self = Nan::ObjectWrap::Unwrap<RTCIceCandidate>(info.This());
  std::string candidate;
  self->cand->ToString(&candidate);

  TRACE_END;
  info.GetReturnValue().Set(Nan::New(candidate).ToLocalChecked());
}

NAN_PROPERTY_GETTER(RTCIceCandidate::GetSdpMid) {
  TRACE_CALL;

  RTCIceCandidate* self = Nan::ObjectWrap::Unwrap<RTCIceCandidate>(info.This());
  std::string sdpMid = self->cand->sdp_mid();

  TRACE_END;
  info.GetReturnValue().Set(Nan::New(sdpMid).ToLocalChecked());
}

NAN_PROPERTY_GETTER(RTCIceCandidate::GetSdpMLineIndex) {
  TRACE_CALL;

  RTCIceCandidate* self = Nan::ObjectWrap::Unwrap<RTCIceCandidate>(info.This());
  int mLineIndex = self->cand->sdp_mline_index();

  TRACE_END;
  info.GetReturnValue().Set(Nan::New(mLineIndex));
}

NAN_METHOD(RTCIceCandidate::New) {
  TRACE_CALL;

  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Use the new operator to construct RTCIceCandidate");
  }

  std::string error;
  RTCIceCandidateInit::RTCIceCandidateInit candidateInit;
  if (!RTCIceCandidateInit::fromNode(info[0], candidateInit, &error)) {
    Nan::ThrowError(Nan::Error(error.c_str()));
    TRACE_END;
    return;
  }

  SdpParseError parseError;
  IceCandidateInterface* candidate = webrtc::CreateIceCandidate(candidateInit.sdpMid.FromMaybe(""), candidateInit.sdpMLineIndex.FromMaybe(0), candidateInit.candidate, &parseError);
  if (!candidate) {
    Nan::ThrowError(parseError.description.c_str());
    TRACE_END;
    return;
  }

  RTCIceCandidate* obj = new RTCIceCandidate(candidate);
  obj->Wrap(info.This());
  
  TRACE_END;
  info.GetReturnValue().Set(info.This());
}

RTCIceCandidate* RTCIceCandidate::create(RTCIceCandidateInit::RTCIceCandidateInit& candidateInit, std::string *error)
{
  Nan::EscapableHandleScope scope;

  Local<Function> cons = Nan::New(RTCIceCandidate::constructor);
  Local<Value> cargv[] = {
    RTCIceCandidateInit::toNode(&candidateInit)
  };
  MaybeLocal<Object> maybeWrapped = Nan::NewInstance(cons, 1, cargv);
  // TODO(mroberts): Raise error string.
  if (maybeWrapped.IsEmpty()) {
    if (error) {
      *error = "Invalid RTCIceCandidate";
    }
    return nullptr;
  }

  Local<Object> wrapped = maybeWrapped.ToLocalChecked();
  scope.Escape(wrapped);

  RTCIceCandidate* candidate = Nan::ObjectWrap::Unwrap<RTCIceCandidate>(wrapped);
  return candidate;
}
