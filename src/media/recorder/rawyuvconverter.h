#ifndef _RAWYUVCONVERTER_H_
#define _RAWYUVCONVERTER_H_

#include "media/recorder/videoframeconverter.h"

namespace node_webrtc {

class RawYuvConverter
: public VideoFrameConverter
{
 public:
  size_t GetRequiredSize(const std::vector<cricket::VideoFrame*>& frames);

 protected:
  size_t ConvertFrame(uint8_t* buffer, const cricket::VideoFrame* frame, size_t* size);
};

}

#endif  // _RAWYUVCONVERTER_H_
