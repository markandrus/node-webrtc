#include <nan.h>

#include "getusermedia.h"
#include "mediastream2.h"
#include "mediastreamtrack2.h"
#include "mediastreamconstraints.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/app/webrtc/videosourceinterface.h"
#include "talk/media/base/videocapturer.h"
#include "talk/media/devices/yuvframescapturer.h"

using namespace node_webrtc;
using namespace v8;

static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;
static rtc::scoped_refptr<webrtc::AudioSourceInterface> audioSource;
static rtc::scoped_refptr<webrtc::VideoSourceInterface> videoSource;
// static cricket::VideoCapturer* capturer =
//  static_cast<cricket::VideoCapturer*>(new cricket::YuvFramesCapturer());
static cricket::YuvFramesCapturer* capturer = new cricket::YuvFramesCapturer();

void GetUserMedia::Init(
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _peerConnectionFactory,
  Handle<Object> exports)
{
  peerConnectionFactory= _peerConnectionFactory;
  audioSource = peerConnectionFactory->CreateAudioSource(nullptr);
  // videoSource = peerConnectionFactory->CreateVideoSource(capturer, nullptr);
  capturer->Init();
  videoSource = peerConnectionFactory->CreateVideoSource(
    static_cast<cricket::VideoCapturer*>(capturer), nullptr);
  exports->Set(Nan::New("getUserMedia").ToLocalChecked(),
    Nan::New<FunctionTemplate>(GetUserMedia)->GetFunction());
}

NAN_METHOD(GetUserMedia::GetUserMedia)
{
  Local<Promise::Resolver> resolver = v8::Promise::Resolver::New(Isolate::GetCurrent());
  Local<Promise> promise = resolver->GetPromise();

  std::string error;
  MediaStreamConstraints::MediaStreamConstraints constraints;
  if (!MediaStreamConstraints::fromNode(info[0], constraints, &error)) {
    Nan::ThrowError(error.c_str());
    return;
  }

  if (info.Length() == 3) {
    Nan::Callback *onSuccess = new Nan::Callback(info[1].As<Function>());
    Nan::Callback *onFailure = new Nan::Callback(info[2].As<Function>());
    promise->Then(onSuccess->GetFunction());
    promise->Catch(onFailure->GetFunction());
  }

  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
    peerConnectionFactory->CreateLocalMediaStream("stream1234567890");

  if (constraints.audio.FromMaybe(true)) {
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack =
      peerConnectionFactory->CreateAudioTrack("audioHrmpHrmpHrmp", audioSource);
    Local<Object> audioStreamTrackObject = Nan::NewInstance(
      Nan::New(MediaStreamTrack::constructor), 0, nullptr).ToLocalChecked();
    MediaStreamTrack* audioStreamTrack = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(audioStreamTrackObject);
    audioStreamTrack->setTrack(audioTrack);
    stream->AddTrack(audioTrack);
  }

  if (constraints.video.FromMaybe(true)) {
    rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack =
      peerConnectionFactory->CreateVideoTrack("videoHrmpHrmpHrmp", videoSource);
    Local<Object> videoStreamTrackObject = Nan::NewInstance(
      Nan::New(MediaStreamTrack::constructor), 0, nullptr).ToLocalChecked();
    MediaStreamTrack* videoStreamTrack = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(videoStreamTrackObject);
    videoStreamTrack->setTrack(videoTrack);
    stream->AddTrack(videoTrack);
  }

  Local<Object> mediaStreamObject = Nan::NewInstance(
    Nan::New(MediaStream::constructor), 0, nullptr).ToLocalChecked();
  MediaStream* mediaStream = Nan::ObjectWrap::Unwrap<MediaStream>(mediaStreamObject);
  mediaStream->setStream(stream);

  resolver->Resolve(mediaStreamObject);
  info.GetReturnValue().Set(promise);
}
