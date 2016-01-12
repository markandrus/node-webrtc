#include "media/recorder/mediarecorderrenderer.h"

using namespace cricket;
using namespace node_webrtc;

using RecordingState = MediaRecorder::RecordingState;

MediaRecorderRenderer::MediaRecorderRenderer(
    MediaRecorder* recorder,
    webrtc::VideoTrackInterface* track):
  recorder_(recorder),
  track_(track)
{
  assert(this->recorder_ != nullptr);
  assert(this->track_ != nullptr);
}

bool MediaRecorderRenderer::Start()
{
  if (this->started_) {
    return false;
  }
  this->started_ = true;
  this->track_->AddRenderer(this);
  return true;
}

bool MediaRecorderRenderer::Stop()
{
  if (!this->started_) {
    return false;
  }
  this->started_ = false;
  this->track_->RemoveRenderer(this);
  return true;
}

void MediaRecorderRenderer::SetSize(int width, int height)
{
  (void) width;
  (void) height;
}

void MediaRecorderRenderer::RenderFrame(const VideoFrame* frame)
{
  // Drop the frame if the MediaRecorder is not recording..
  if (this->recorder_->state() != RecordingState::kRecording) {
    return;
  }

  if (this->recorder_->ignore_muted_media()) {
    // TODO(mroberts): We should be able to take action based on whether the
    // MediaRecorder is ignoring muted media or not.
  }

  // Otherwise, queue a copy.
  auto copy = frame->Copy();
  assert(copy != nullptr);
  this->recorder_->QueueFrame(copy);
}
