#include "videoframeconverter.h"

using namespace node_webrtc;

uint8_t* VideoFrameConverter::Convert(const std::vector<cricket::VideoFrame*>& frames, size_t& size)
{
  size_t required = this->GetRequiredSize(frames);
  if (required == 0) {
    size = 0;
    return nullptr;
  }

  uint8_t* buffer = new (std::nothrow) uint8_t[required];
  if (buffer == nullptr) {
    size = 0;
    return nullptr;
  }

  size_t remaining = required;
  uint8_t* cursor = buffer;

  for (auto frame : frames) {
    size_t copied = this->ConvertFrame(cursor, frame, &remaining);
    assert(copied > 0);
    remaining -= copied;
    assert(remaining >= 0);
    cursor += copied;
  }

  assert(remaining == 0);
  size = required;
  return buffer;
}
