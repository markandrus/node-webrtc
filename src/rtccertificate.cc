#include <nan.h>
#include "rtccertificate.h"

using namespace node_webrtc;
using namespace v8;

Local<RTCCertificate::RTCCertificate> toNode(rtc::RTCCertificate* certificate)
{
  Nan::EscapableHandleScope scope;
  Local<Object> object = Nan::New<Object>();
  Nan::Set(object, Nan::New("expires").ToLocalChecked(),
    Nan::New(certificate->expires_timestamp_ns() * 1e-6));
  return scope.Escape(object);
}
