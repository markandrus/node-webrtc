#ifndef _RTCOFFEROPTIONS_H_
#define _RTCOFFEROPTIONS_H_

#include <v8.h>
#include "talk/app/webrtc/peerconnectioninterface.h"

namespace node_webrtc {

namespace RTCOfferOptions {

  bool fromNode(
    v8::Local<v8::Value> unparsed,
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options,
    std::string* error);

}

}

#endif  // _RTCOFFEROPTIONS_H_
