#include <iostream>
#include <new>

#include "event.h"
#include "media/recorder/mediarecorder.h"
#include "media/recorder/mediarecorderoptions.h"
#include "media/recorder/mediarecorderrenderer.h"
#include "media/recorder/capturedframeconverter.h"
#include "media/recorder/rawyuvconverter.h"

#include "talk/media/base/videocapturer.h"

using namespace node_webrtc;
using namespace v8;

Nan::Persistent<Function> MediaRecorder::constructor;

MediaRecorder::MediaRecorder(
    MediaStream* media_stream,
    MimeType mime_type):
  media_stream_(media_stream),
  mime_type_(mime_type)
{
  // NOTE(mroberts): Currently MediaRecorder will only record the first video
  // track, if any.
  webrtc::MediaStreamInterface* stream = this->media_stream_->getStream();
  webrtc::VideoTrackVector video_tracks = stream->GetVideoTracks();
  if (video_tracks.size() > 0) {
    webrtc::VideoTrackInterface* video_track = video_tracks.front();
    this->renderer_ = new MediaRecorderRenderer(this, video_track);
  }

  switch (this->mime_type_) {
    case MimeType::kVideoVndWebrtcCapturedFrame:
      this->converter_ = new CapturedFrameConverter();
      break;
    case MimeType::kVideoXRawYuv:
      this->converter_ = new RawYuvConverter();
      break;
    default:
      assert(false);  // impossible
  }

  this->loop_ = uv_default_loop();
  uv_mutex_init(&this->lock_);
  uv_async_init(this->loop_, &this->async_, reinterpret_cast<uv_async_cb>(Run));
  this->async_.data = this;
}

MediaRecorder::MediaRecorder(
    MediaStream* media_stream):
  MediaRecorder(media_stream, MimeType::kVideoXRawYuv)
{
  // Do nothing.
}

void MediaRecorder::Init(Handle<Object> exports)
{
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("MediaRecorder").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("stream").ToLocalChecked(), GetStream, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("mimeType").ToLocalChecked(), GetMimeType, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("state").ToLocalChecked(), GetState, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("ignoreMutedMedia").ToLocalChecked(), GetIgnoreMutedMedia, SetIgnoreMutedMedia);
  Nan::SetPrototypeMethod(tpl, "start", Start);
  Nan::SetPrototypeMethod(tpl, "stop", Stop);
  Nan::SetPrototypeMethod(tpl, "pause", Pause);
  Nan::SetPrototypeMethod(tpl, "resume", Resume);
  Nan::SetPrototypeMethod(tpl, "requestData", RequestData);
  Nan::SetMethod(tpl, "isTypeSupported", IsTypeSupported);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("MediaRecorder").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(MediaRecorder::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Use the new operator to construct MediaRecorder");
  }

  MediaStream* media_stream = Nan::ObjectWrap::Unwrap<MediaStream>(info[0].As<Object>());

  MediaRecorderOptions::MediaRecorderOptions options;
  if (!info[1].IsEmpty() && !info[1]->IsNull() && !info[1]->IsUndefined()) {
    std::string error;
    if (!MediaRecorderOptions::from_node(info[1], options, &error)) {
      return Nan::ThrowTypeError(error.c_str());
    }
  }

  MimeType mime_type = MimeType::kVideoXRawYuv;
  if (!options.mime_type.empty()
    && !mime_type_from_node(Nan::New(options.mime_type).ToLocalChecked(), &mime_type))
  {
    return Nan::ThrowTypeError("Not supported");
  }

  MediaRecorder* media_recorder = new MediaRecorder(media_stream, mime_type);
  media_recorder->Wrap(info.This());

  info.GetReturnValue().Set(media_recorder->handle());
}

NAN_PROPERTY_GETTER(MediaRecorder::GetStream)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  info.GetReturnValue().Set(self->media_stream_->handle());
}

NAN_PROPERTY_GETTER(MediaRecorder::GetMimeType)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  info.GetReturnValue().Set(Nan::New(mime_type_to_string(self->mime_type_)).ToLocalChecked());
}

NAN_PROPERTY_GETTER(MediaRecorder::GetState)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  std::string state;
  switch (self->state_) {
    case RecordingState::kInactive:
      state = "inactive";
      break;
    case RecordingState::kRecording:
      state = "recording";
      break;
    case RecordingState::kPaused:
      state = "paused";
      break;
    default:
      assert(false);  // impossible
  }
  info.GetReturnValue().Set(Nan::New(state).ToLocalChecked());
}

NAN_PROPERTY_GETTER(MediaRecorder::GetIgnoreMutedMedia)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  info.GetReturnValue().Set(Nan::New<Boolean>(self->ignore_muted_media_));
}

NAN_SETTER(MediaRecorder::SetIgnoreMutedMedia)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  bool ignore_muted_media = info.Data().As<Boolean>()->BooleanValue();
  self->ignore_muted_media_ = ignore_muted_media;
}

NAN_METHOD(MediaRecorder::Start)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  if (self->state_ != RecordingState::kInactive) {
    return Nan::ThrowError("Invalid state");
  }
  self->state_ = RecordingState::kRecording;
  self->renderer_->Start();
  // TODO(mroberts): Handle the timeSlice argument.
  Local<Function> callback = Local<Function>::Cast(self->handle()->Get(
    Nan::New("onstart").ToLocalChecked()));
  if (!callback.IsEmpty()) {
    Local<Event::Event> event = Event::create(self->handle());
    Nan::MakeCallback(self->handle(), callback, 1, (Local<Value>*) &event);
  }
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaRecorder::Stop)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  if (self->state_ != RecordingState::kInactive) {
    return Nan::ThrowError("Invalid state");
  }
  self->state_ = RecordingState::kInactive;
  self->renderer_->Stop();
  self->QueueRequestDataRequest();
  // TODO(mroberts): We need to fire onstop after ondataavailable.
  Local<Function> callback = Local<Function>::Cast(self->handle()->Get(
    Nan::New("onstop").ToLocalChecked()));
  if (!callback.IsEmpty()) {
    Local<Event::Event> event = Event::create(self->handle());
    Nan::MakeCallback(self->handle(), callback, 1, (Local<Value>*) &event);
  }
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaRecorder::Pause)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  if (self->state_ != RecordingState::kInactive) {
    return Nan::ThrowError("Invalid state");
  }
  self->state_ = RecordingState::kPaused;
  Local<Function> callback = Local<Function>::Cast(self->handle()->Get(
    Nan::New("onpause").ToLocalChecked()));
  if (!callback.IsEmpty()) {
    Local<Event::Event> event = Event::create(self->handle());
    Nan::MakeCallback(self->handle(), callback, 1, (Local<Value>*) &event);
  }
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaRecorder::Resume)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  self->state_ = RecordingState::kRecording;
  Local<Function> callback = Local<Function>::Cast(self->handle()->Get(
    Nan::New("onresume").ToLocalChecked()));
  if (!callback.IsEmpty()) {
    Local<Event::Event> event = Event::create(self->handle());
    Nan::MakeCallback(self->handle(), callback, 1, (Local<Value>*) &event);
  }
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaRecorder::RequestData)
{
  MediaRecorder* self = Nan::ObjectWrap::Unwrap<MediaRecorder>(info.This());
  if (self->state_ != RecordingState::kRecording) {
    return Nan::ThrowError("Invalid state");
  }
  self->QueueRequestDataRequest();
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(MediaRecorder::IsTypeSupported)
{
  info.GetReturnValue().Set(Nan::New<Boolean>(mime_type_from_node(info[0], nullptr)));
}

uint8_t* MediaRecorder::ConvertAndDelete(size_t& size)
{
  uint8_t* buffer = this->converter_->Convert(this->frames_, size);
  for (auto frame : this->frames_) {
    delete frame;
  }
  this->frames_.clear();
  return buffer;
}

void MediaRecorder::QueueFrame(cricket::VideoFrame* frame)
{
  if (frame != nullptr) {
    uv_mutex_lock(&this->lock_);
    this->queued_frames_.push(frame);
    uv_mutex_unlock(&this->lock_);
  }
  uv_async_send(&this->async_);
}

void MediaRecorder::QueueRequestDataRequest()
{
  // TODO(mroberts): This is a little hack...
  uv_mutex_lock(&this->lock_);
  this->queued_frames_.push(nullptr);
  uv_mutex_unlock(&this->lock_);
  uv_async_send(&this->async_);
}

void MediaRecorder::Run(uv_async_t* handle, int status)
{
  Nan::HandleScope scope;
  MediaRecorder* self = static_cast<MediaRecorder*>(handle->data);
  Handle<Object> mediaRecorderObject = self->handle();

  while (true) {
    uv_mutex_lock(&self->lock_);
    bool empty = self->queued_frames_.empty();
    if (empty) {
      uv_mutex_unlock(&self->lock_);
      break;
    }

    cricket::VideoFrame* frame = self->queued_frames_.front();
    self->queued_frames_.pop();
    uv_mutex_unlock(&self->lock_);

    if (frame != nullptr) {
      self->frames_.push_back(frame);
    } else {
      size_t size;
      uint8_t* buffer = self->ConvertAndDelete(size);
      std::cout << "# of frames left: " << self->frames_.size() << std::endl;
      assert(self->frames_.size() == 0);
      if (buffer == nullptr) {
        std::cout << "Buffer was null" << std::endl;
        continue;
      }

      std::cout << "Did convert" << std::endl;

      Nan::MaybeLocal<Object> maybeBufferObject = Nan::CopyBuffer((char*) buffer, size);
      if (buffer != nullptr) {
        std::cout << "Deleting buffer: " << (void*) buffer << std::endl;
        delete[] buffer;
      }
      std::cout << "Did not delete buffer >_>\n";

      if (maybeBufferObject.IsEmpty()) {
        continue;
      }
      Local<Object> bufferObject = maybeBufferObject.ToLocalChecked();

      std::cout << "Created buffer object" << std::endl;

      Local<Function> callback = Local<Function>::Cast(mediaRecorderObject->Get(Nan::New("ondataavailable").ToLocalChecked()));
      if (!callback.IsEmpty()) {
        std::cout << "Making callback" << std::endl;
        Local<Event::Event> event = Event::create(mediaRecorderObject);
        Nan::Set(event, Nan::New("data").ToLocalChecked(), bufferObject);
        Nan::MakeCallback(mediaRecorderObject, callback, 1, (Local<Value>*) &event);
      }
      std::cout << "Done" << std::endl;
    }
  }
}
