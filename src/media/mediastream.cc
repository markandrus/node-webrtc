#include "media/mediastream.h"

using namespace node_webrtc;
using namespace v8;

using MediaStreamTrack = node_webrtc::MediaStreamTrack;

Nan::Persistent<v8::Function> MediaStream::constructor;

static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;

MediaStream::MediaStream()
{
}

MediaStream::MediaStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
  this->setStream(stream);
}

void MediaStream::setStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
  this->stream = stream;
}

void MediaStream::Init(
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _peerConnectionFactory,
  Handle<Object> exports)
{
  peerConnectionFactory = _peerConnectionFactory;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("MediaStream").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("id").ToLocalChecked(), GetId, nullptr);
  Nan::SetPrototypeMethod(tpl, "getAudioTracks", GetAudioTracks);
  Nan::SetPrototypeMethod(tpl, "getVideoTracks", GetVideoTracks);
  Nan::SetPrototypeMethod(tpl, "getTracks", GetTracks);
  Nan::SetPrototypeMethod(tpl, "getTrackById", GetTrackById);
  Nan::SetPrototypeMethod(tpl, "addTrack", AddTrack);
  Nan::SetPrototypeMethod(tpl, "removeTrack", RemoveTrack);
  Nan::SetPrototypeMethod(tpl, "clone", Clone);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("active").ToLocalChecked(), GetActive, nullptr);
  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("MediaStream").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(MediaStream::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Use the new operator to construct the MediaStream");
  }

  MediaStream* stream = nullptr;
  if (info.Length() == 0) {
    stream = new MediaStream();
  } else if (info.Length() == 1) {
    if (info[0]->IsArray()) {
      // TODO(mroberts): ...
    } else if (info[0]->IsObject()) {
      // stream = new MediaStream(Nan::ObjectWrap::Unwrap<MediaStream>(info[0]));
    } else {
      return Nan::ThrowTypeError("MediaStream constructor expects a MediaStream or an array of MediaStreamTracks");
    }
  } else {
    return Nan::ThrowTypeError("MediaStream constructor expects 0 or 1 arguments");
  }

  stream->Wrap(info.This());
  stream->Ref();
  info.GetReturnValue().Set(stream->handle());
}

NAN_PROPERTY_GETTER(MediaStream::GetId)
{
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.This());
  Local<String> id = Nan::New(self->stream->label()).ToLocalChecked();
  info.GetReturnValue().Set(id);
}

NAN_METHOD(MediaStream::GetAudioTracks)
{
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.This());
  webrtc::MediaStreamInterface* stream = self->getStream();

  Local<Array> sequence = Nan::New<Array>();
  uint32_t i = 0;

  // Get AudioTracks
  for (webrtc::MediaStreamTrackInterface* track : stream->GetAudioTracks()) {
    Local<Object> audioStreamTrackObject = Nan::NewInstance(
      Nan::New(MediaStreamTrack::constructor), 0, nullptr).ToLocalChecked();
    MediaStreamTrack* audioStreamTrack = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(audioStreamTrackObject);
    audioStreamTrack->setTrack(track);
    Nan::Set(sequence, i++, audioStreamTrackObject);
  }

  info.GetReturnValue().Set(sequence);
}

NAN_METHOD(MediaStream::GetVideoTracks)
{
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.This());
  webrtc::MediaStreamInterface* stream = self->getStream();

  Local<Array> sequence = Nan::New<Array>();
  uint32_t i = 0;

  // Get VideoTracks
  for (webrtc::MediaStreamTrackInterface* track : stream->GetVideoTracks()) {
    Local<Object> videoStreamTrackObject = Nan::NewInstance(
      Nan::New(MediaStreamTrack::constructor), 0, nullptr).ToLocalChecked();
    MediaStreamTrack* videoStreamTrack = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(videoStreamTrackObject);
    videoStreamTrack->setTrack(track);
    Nan::Set(sequence, i++, videoStreamTrackObject);
  }

  info.GetReturnValue().Set(sequence);
}

NAN_METHOD(MediaStream::GetTracks)
{
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.This());
  webrtc::MediaStreamInterface* stream = self->getStream();

  Local<Array> sequence = Nan::New<Array>();
  uint32_t i = 0;

  // Get AudioTracks
  for (webrtc::MediaStreamTrackInterface* track : stream->GetAudioTracks()) {
    Local<Object> audioStreamTrackObject = Nan::NewInstance(
      Nan::New(MediaStreamTrack::constructor), 0, nullptr).ToLocalChecked();
    MediaStreamTrack* audioStreamTrack = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(audioStreamTrackObject);
    audioStreamTrack->setTrack(track);
    Nan::Set(sequence, i++, audioStreamTrackObject);
  }

  // Get VideoTracks
  for (webrtc::MediaStreamTrackInterface* track : stream->GetVideoTracks()) {
    Local<Object> videoStreamTrackObject = Nan::NewInstance(
      Nan::New(MediaStreamTrack::constructor), 0, nullptr).ToLocalChecked();
    MediaStreamTrack* videoStreamTrack = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(videoStreamTrackObject);
    videoStreamTrack->setTrack(track);
    Nan::Set(sequence, i++, videoStreamTrackObject);
  }

  info.GetReturnValue().Set(sequence);
}

NAN_METHOD(MediaStream::GetTrackById)
{
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.This());
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaStream::AddTrack)
{
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.This());
  webrtc::MediaStreamInterface* stream = self->getStream();

  MediaStreamTrack* mediaStreamTrack = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info[0].As<Object>());
  webrtc::MediaStreamTrackInterface* track = mediaStreamTrack->getTrack();

  if (track->kind() == "audio") {
    bool result = stream->AddTrack(static_cast<webrtc::AudioTrackInterface*>(track));
    assert(result);
  } else {
    bool result = stream->AddTrack(static_cast<webrtc::VideoTrackInterface*>(track));
    assert(result);
  }

  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaStream::RemoveTrack)
{
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.This());
  webrtc::MediaStreamInterface* stream = self->getStream();

  MediaStreamTrack* mediaStreamTrack = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info[0].As<Object>());
  webrtc::MediaStreamTrackInterface* track = mediaStreamTrack->getTrack();

  if (track->kind() == "audio") {
    bool result = stream->RemoveTrack(static_cast<webrtc::AudioTrackInterface*>(track));
    assert(result);
  } else {
    bool result = stream->RemoveTrack(static_cast<webrtc::VideoTrackInterface*>(track));
    assert(result);
  }

  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaStream::Clone)
{
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.This());
  webrtc::MediaStreamInterface* stream = self->getStream();

  rtc::scoped_refptr<webrtc::MediaStreamInterface> clone =
    peerConnectionFactory->CreateLocalMediaStream("cloneymcclonerston");

  // Copy AudioTracks
  for (webrtc::AudioTrackInterface* track : stream->GetAudioTracks()) {
    clone->AddTrack(track);
  }

  // Copy VideoTracks
  for (webrtc::VideoTrackInterface* track : stream->GetVideoTracks()) {
    clone->AddTrack(track);
  }

  Local<Object> mediaStreamObject = Nan::NewInstance(
    Nan::New(MediaStream::constructor), 0, nullptr).ToLocalChecked();
  MediaStream* mediaStream = Nan::ObjectWrap::Unwrap<MediaStream>(mediaStreamObject);
  mediaStream->setStream(clone);

  info.GetReturnValue().Set(mediaStreamObject);
}

NAN_PROPERTY_GETTER(MediaStream::GetActive)
{
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.This());
  webrtc::MediaStreamInterface* stream = self->getStream();

  // Check for an active AudioTrack
  for (webrtc::MediaStreamTrackInterface* track : stream->GetAudioTracks()) {
    if (track->state() == webrtc::MediaStreamTrackInterface::TrackState::kInitializing
      || track->state() == webrtc::MediaStreamTrackInterface::TrackState::kLive)
    {
      info.GetReturnValue().Set(Nan::New<Boolean>(true));
      return;
    }
  }

  // Check for an active VideoTrack
  for (webrtc::MediaStreamTrackInterface* track : stream->GetVideoTracks()) {
    if (track->state() == webrtc::MediaStreamTrackInterface::TrackState::kInitializing
      || track->state() == webrtc::MediaStreamTrackInterface::TrackState::kLive)
    {
      info.GetReturnValue().Set(Nan::New<Boolean>(true));
      return;
    }
  }

  info.GetReturnValue().Set(Nan::New<Boolean>(false));
}
