#include "mediastreamtrack2.h"

using namespace node_webrtc;
using namespace v8;

Nan::Persistent<v8::Function> MediaStreamTrack::constructor;

MediaStreamTrack::MediaStreamTrack()
{
}

MediaStreamTrack::MediaStreamTrack(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
  this->setTrack(track);
}

void MediaStreamTrack::setTrack(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
  this->track = track;
}

void MediaStreamTrack::Init(Handle<Object> exports)
{
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("MediaStreamTrack").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("kind").ToLocalChecked(), GetKind, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("id").ToLocalChecked(), GetId, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("label").ToLocalChecked(), GetLabel, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("enabled").ToLocalChecked(), GetEnabled, SetEnabled);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("muted").ToLocalChecked(), GetMuted, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("readonly").ToLocalChecked(), GetReadonly, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("remote").ToLocalChecked(), GetRemote, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("readyState").ToLocalChecked(), GetReadyState, nullptr);
  Nan::SetPrototypeMethod(tpl, "clone", Clone);
  Nan::SetPrototypeMethod(tpl, "stop", Stop);
  Nan::SetPrototypeMethod(tpl, "getCapabilities", GetCapabilities);
  Nan::SetPrototypeMethod(tpl, "getConstraints", GetConstraints);
  Nan::SetPrototypeMethod(tpl, "getSettings", GetSettings);
  Nan::SetPrototypeMethod(tpl, "applyConstraints", ApplyConstraints);

  constructor.Reset(tpl->GetFunction());
}

NAN_METHOD(MediaStreamTrack::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Use the new operator to construct the MediaStreamTrack");
  }

  MediaStreamTrack* track = new MediaStreamTrack();
  track->Wrap(info.This());
  track->Ref();
  info.GetReturnValue().Set(track->handle());
}

NAN_PROPERTY_GETTER(MediaStreamTrack::GetKind)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  Local<String> kind = Nan::New(self->track->kind()).ToLocalChecked();
  info.GetReturnValue().Set(kind);
}

NAN_PROPERTY_GETTER(MediaStreamTrack::GetId)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  Local<String> id = Nan::New(self->track->id()).ToLocalChecked();
  info.GetReturnValue().Set(id);
}

NAN_PROPERTY_GETTER(MediaStreamTrack::GetLabel)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  // TODO(mroberts): Not sure about this.
  Local<String> label = Nan::New(self->track->id()).ToLocalChecked();
  info.GetReturnValue().Set(label);
}

NAN_PROPERTY_GETTER(MediaStreamTrack::GetEnabled)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  Local<Boolean> enabled = Nan::New<Boolean>(self->track->enabled());
  info.GetReturnValue().Set(enabled);
}

NAN_SETTER(MediaStreamTrack::SetEnabled)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  // TODO(mroberts): Do some proper checking.
  Local<Boolean> enabled = Nan::To<Boolean>(info.Data()).ToLocalChecked();
  self->track->set_enabled(enabled->BooleanValue());
}

NAN_PROPERTY_GETTER(MediaStreamTrack::GetMuted)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  // TODO(mroberts): Not sure about this.
  Local<Boolean> muted = Nan::New<Boolean>(!self->track->enabled());
  info.GetReturnValue().Set(muted);
}

NAN_PROPERTY_GETTER(MediaStreamTrack::GetReadonly)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  // TODO(mroberts): Not sure about this.
  Local<Boolean> readonly = Nan::New<Boolean>(false);
  info.GetReturnValue().Set(readonly);
}

NAN_PROPERTY_GETTER(MediaStreamTrack::GetRemote)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  // TODO(mroberts): Not sure about this.
  Local<Boolean> remote = Nan::New<Boolean>(false);
  info.GetReturnValue().Set(remote);
}

NAN_PROPERTY_GETTER(MediaStreamTrack::GetReadyState)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  // TODO(mroberts): Not sure about this.
  Local<String> readyState;
  switch (self->track->state()) {
    case webrtc::MediaStreamTrackInterface::TrackState::kInitializing:
    case webrtc::MediaStreamTrackInterface::TrackState::kLive:
      readyState = Nan::New("live").ToLocalChecked();
      break;
    case webrtc::MediaStreamTrackInterface::TrackState::kEnded:
    case webrtc::MediaStreamTrackInterface::TrackState::kFailed:
      readyState = Nan::New("ended").ToLocalChecked();
      break;
    default:
      assert(false);  // impossible
  }
  info.GetReturnValue().Set(readyState);
}

NAN_METHOD(MediaStreamTrack::Clone)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaStreamTrack::Stop)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaStreamTrack::GetCapabilities)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaStreamTrack::GetConstraints)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaStreamTrack::GetSettings)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaStreamTrack::ApplyConstraints)
{
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.This());
  info.GetReturnValue().Set(Nan::Null());
}
