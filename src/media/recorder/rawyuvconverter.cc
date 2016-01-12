#include "rawyuvconverter.h"

using namespace node_webrtc;

size_t RawYuvConverter::GetRequiredSize(const std::vector<cricket::VideoFrame*>& frames)
{
  size_t required = 0;
  for (auto frame : frames) {
    required += cricket::VideoFrame::SizeOf(frame->GetWidth(), frame->GetHeight());
  }
  return required;
}

size_t RawYuvConverter::ConvertFrame(uint8_t* buffer, const cricket::VideoFrame* frame, size_t* size)
{
  size_t dataSize = cricket::VideoFrame::SizeOf(frame->GetWidth(), frame->GetHeight());

  if (size != nullptr && *size < dataSize) {
    return 0;
  }

  uint8_t* data = buffer;
  size_t copied = frame->CopyToBuffer(data, dataSize);
  assert(copied == dataSize);
  return dataSize;
}
