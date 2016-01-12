#include "talk/media/base/videocapturer.h"
#include "webrtc/base/bytebuffer.h"

#include "capturedframeconverter.h"

using namespace node_webrtc;

size_t CapturedFrameConverter::GetRequiredSize(const std::vector<cricket::VideoFrame*>& frames)
{
  size_t required = 40 * frames.size();
  for (auto frame : frames) {
    required += cricket::VideoFrame::SizeOf(frame->GetWidth(), frame->GetHeight());
  }
  return required;
}

size_t CapturedFrameConverter::ConvertFrame(uint8_t* buffer, const cricket::VideoFrame* frame, size_t* size)
{
  size_t dataSize = cricket::VideoFrame::SizeOf(frame->GetWidth(), frame->GetHeight());
  uint32_t headerSize = 40;
  size_t bufferSize = headerSize + dataSize;

  if (size != nullptr && *size < bufferSize) {
    return 0;
  }

  // Copy header
  rtc::ByteBuffer byte_buffer(nullptr, bufferSize);
  byte_buffer.WriteUInt32(frame->GetWidth());
  byte_buffer.WriteUInt32(frame->GetHeight());
  byte_buffer.WriteUInt32(0x30323449);  // I420
  byte_buffer.WriteUInt32(frame->GetPixelWidth());
  byte_buffer.WriteUInt32(frame->GetPixelHeight());
  byte_buffer.WriteUInt64(0);
  byte_buffer.WriteUInt64(frame->GetTimeStamp());
  byte_buffer.WriteUInt32(dataSize);

  // Copy data
  size_t copied = frame->CopyToBuffer(buffer, dataSize);
  assert(copied == dataSize);
  byte_buffer.WriteBytes((char*) buffer, copied);

  assert(byte_buffer.Length() == bufferSize);
  memcpy(buffer, byte_buffer.Data(), bufferSize);

  return bufferSize;
}
