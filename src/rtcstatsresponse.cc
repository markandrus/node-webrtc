#include "rtcstatsresponse.h"
#include "rtcstatsreport.h"

#include "talk/base/refcount.h"

using namespace node;
using namespace v8;

using namespace node_webrtc;

Persistent<Function> RTCStatsResponse::constructor;

RTCStatsResponse::RTCStatsResponse(webrtc::StatsReports* response)
: response(response) {};

RTCStatsResponse::~RTCStatsResponse() {
  response = NULL;
}

NAN_METHOD(RTCStatsResponse::New) {
  TRACE_CALL;
  NanScope();

  if (!args.IsConstructCall()) {
    return NanThrowTypeError("Use the new operator to construct the RTCStatsResponse");
  }

  Local<External> _reports = Local<External>::Cast(args[0]);
  webrtc::StatsReports* reports = static_cast<webrtc::StatsReports*>(_reports->Value());

  RTCStatsResponse* obj = new RTCStatsResponse(reports);
  obj->Wrap( args.This() );

  TRACE_END;
  NanReturnValue( args.This() );
}

NAN_METHOD(RTCStatsResponse::result) {
  TRACE_CALL;
  NanScope();

  RTCStatsResponse* self = ObjectWrap::Unwrap<RTCStatsResponse>( args.This() );

  Local<Array> reports = NanNew<Array>(self->response->size());
  for (std::vector<int>::size_type i = 0; i != self->response->size(); i++) {
    webrtc::StatsReport *copy = &self->response->at(i);
    v8::Local<v8::Value> cargv[1];
    cargv[0] = NanNew<v8::External>(copy);
    reports->Set(i, NanNew(RTCStatsReport::constructor)->NewInstance(1, cargv));
  }

  TRACE_END;
  NanReturnValue( reports );
}

void RTCStatsResponse::Init( Handle<Object> exports ) {
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate> ( New );
  tpl->SetClassName( NanNew( "RTCStatsResponse" ) );
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->PrototypeTemplate()->Set( NanNew( "result" ),
    NanNew<FunctionTemplate>( result )->GetFunction() );
  NanAssignPersistent( constructor, tpl->GetFunction() );
  exports->Set( NanNew( "RTCStatsResponse" ), tpl->GetFunction() );
}
