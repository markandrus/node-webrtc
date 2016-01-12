#ifndef _VIDEOFRAMECONVERTER_H_
#define _VIDEOFRAMECONVERTER_H_

#include "talk/media/base/videoframe.h"

namespace node_webrtc {

class VideoFrameConverter {
 public:
  virtual uint8_t* Convert(const std::vector<cricket::VideoFrame*>& frames, size_t& size);

  virtual size_t GetRequiredSize(const std::vector<cricket::VideoFrame*>& frames) = 0;

 protected:
  virtual size_t ConvertFrame(uint8_t* buffer, const cricket::VideoFrame* frame, size_t* size) = 0;
};

}

#endif  // _VIDEOFRAMECONVERTER_H_
