#include "talk/app/webrtc/mediastreaminterface.h"

#include "media/capturer/filevideocapturer.h"
#include "media/mediastream.h"
#include "media/mediastreamtrack.h"

using namespace node_webrtc;
using namespace v8;

Nan::Persistent<Function> FileVideoCapturer::constructor;
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> FileVideoCapturer::peer_connection_factory_;

FileVideoCapturer::FileVideoCapturer(
    std::string filename,
    cricket::FileVideoCapturer* capturer,
    MimeType mime_type):
  capturer_(capturer),
  filename_(filename),
  mime_type_(mime_type)
{
  this->capture_format_.width = 640;
  this->capture_format_.height = 640;
  this->capture_format_.interval = 10000; // 30 * 1e+9;
  this->capture_format_.fourcc = 0x30323449;  // I420
  bool result = this->capturer_->GetBestCaptureFormat(this->capture_format_, &this->capture_format_);
  assert(result);
  this->capturer_->set_repeat(this->repeat_);
  this->capturer_->set_ignore_framerate(this->ignore_framerate_);
  this->video_source_ = peer_connection_factory_->CreateVideoSource(this->capturer_, nullptr);
}

void FileVideoCapturer::Init(
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory,
  Handle<Object> exports)
{
  peer_connection_factory_ = peer_connection_factory;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("FileVideoCapturer").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("filename").ToLocalChecked(), GetFilename, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("ignoreFramerate").ToLocalChecked(), GetIgnoreFrameRate, SetIgnoreFrameRate);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("mimeType").ToLocalChecked(), GetMimeType, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("repeat").ToLocalChecked(), GetRepeat, SetRepeat);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("state").ToLocalChecked(), GetState, nullptr);
  Nan::SetPrototypeMethod(tpl, "captureStream", CaptureStream);
  Nan::SetPrototypeMethod(tpl, "start", Start);
  Nan::SetPrototypeMethod(tpl, "stop", Stop);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("FileVideoCapturer").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(FileVideoCapturer::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Use the new operator to construct FileVideoCapturer");
  }

  if (info[0].IsEmpty() || !info[0]->IsString()) {
    return Nan::ThrowTypeError("First argument to FileVideoCapturer constructor must be a string");
  }

  std::string filename = *(String::Utf8Value(info[0]));
  cricket::FileVideoCapturer* capturer = new cricket::FileVideoCapturer();
  if (capturer == nullptr) {
    return Nan::ThrowError("Unable to construct FileVideoCapturer");
  } else if (!capturer->Init(filename)) {
    delete capturer;
    return Nan::ThrowError("Unable to open file for capture");
  }

  MimeType mime_type = MimeType::kVideoXRawYuv;
  if (!info[1].IsEmpty() && !info[1]->IsNull() && !info[1]->IsUndefined()) {
    if (!info[1]->IsObject()) {
      delete capturer;
      return Nan::ThrowError("Second argument to FileVideoCapturer must be an object");
    }
    Local<String> key = Nan::New("mimeType").ToLocalChecked();
    Local<Object> options = Nan::To<Object>(info[1]).ToLocalChecked();
    if (Nan::Has(options, key).FromMaybe(false)) {
      Local<Value> value = Nan::Get(options, key).ToLocalChecked();
      if (value->IsNull() || value->IsUndefined()) {
        // Do nothing.
      } else if (value->IsString()) {
        if (!mime_type_from_node(value, &mime_type)) {
          delete capturer;
          return Nan::ThrowError("Not supported");
        }
      } else {
        delete capturer;
        return Nan::ThrowError("FileVideoCapturer mimeType must be a string");
      }
    }
  }

  FileVideoCapturer* file_video_capturer =
    new FileVideoCapturer(filename, capturer, mime_type);
  file_video_capturer->capturer_ = capturer;
  file_video_capturer->Wrap(info.This());

  info.GetReturnValue().Set(file_video_capturer->handle());
}

NAN_PROPERTY_GETTER(FileVideoCapturer::GetFilename)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());
  info.GetReturnValue().Set(Nan::New(self->filename_).ToLocalChecked());
}

NAN_PROPERTY_GETTER(FileVideoCapturer::GetIgnoreFrameRate)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());
  info.GetReturnValue().Set(Nan::New<Boolean>(self->ignore_framerate_));
}

NAN_SETTER(FileVideoCapturer::SetIgnoreFrameRate)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());
  bool ignore_framerate = info.Data().As<Boolean>()->BooleanValue();
  self->capturer_->set_ignore_framerate(ignore_framerate);
  self->ignore_framerate_ = ignore_framerate;
}

NAN_PROPERTY_GETTER(FileVideoCapturer::GetMimeType)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());
  info.GetReturnValue().Set(Nan::New(mime_type_to_string(self->mime_type_)).ToLocalChecked());
}

NAN_PROPERTY_GETTER(FileVideoCapturer::GetRepeat)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());
  info.GetReturnValue().Set(Nan::New<Number>(self->repeat_));
}

NAN_SETTER(FileVideoCapturer::SetRepeat)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());
  int repeat = info.Data().As<Number>()->Int32Value();
  self->capturer_->set_repeat(repeat);
  self->repeat_ = repeat;
}

NAN_PROPERTY_GETTER(FileVideoCapturer::GetState)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());
  std::string state;
  switch (self->state_) {
    case CapturingState::kInactive:
      state = "inactive";
      break;
    case CapturingState::kRunning:
      state = "running";
      break;
    default:
      assert(false);  // impossible
  }
  info.GetReturnValue().Set(Nan::New(state).ToLocalChecked());
}

NAN_METHOD(FileVideoCapturer::CaptureStream)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());

  // MediaStream
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
    peer_connection_factory_->CreateLocalMediaStream("streamXXXXX");
  Local<Object> media_stream_object = Nan::NewInstance(
    Nan::New(MediaStream::constructor), 0, nullptr).ToLocalChecked();
  MediaStream* media_stream = Nan::ObjectWrap::Unwrap<MediaStream>(media_stream_object);
  media_stream->setStream(stream);

  // MediaStreamTrack
  rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track =
    peer_connection_factory_->CreateVideoTrack("asdaskjnasdknj", self->video_source_);
  Local<Object> media_stream_track_object = Nan::NewInstance(
    Nan::New(MediaStreamTrack::constructor), 0, nullptr).ToLocalChecked();
  MediaStreamTrack* media_stream_track = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(
    media_stream_track_object);
  media_stream_track->setTrack(video_track);
  stream->AddTrack(video_track);

  info.GetReturnValue().Set(media_stream_object);
}

NAN_METHOD(FileVideoCapturer::Start)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());
  if (self->state_ == CapturingState::kRunning) {
    return Nan::ThrowError("Invalid state");
  }
  self->state_ = CapturingState::kRunning;
  if (!self->capturer_->Start(self->capture_format_)) {
    return Nan::ThrowError("Failed to start FileVideoCapturer");
  }
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(FileVideoCapturer::Stop)
{
  FileVideoCapturer* self = Nan::ObjectWrap::Unwrap<FileVideoCapturer>(info.This());
  if (self->state_ != CapturingState::kRunning) {
    return Nan::ThrowError("Invalid state");
  }
  self->state_ = CapturingState::kInactive;
  self->capturer_->Stop();
  info.GetReturnValue().Set(Nan::Undefined());
}
