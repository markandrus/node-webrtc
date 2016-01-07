#ifndef _RTCOFFERANSWEROPTIONS_H_
#define _RTCOFFERANSWEROPTIONS_H_

#include <v8.h>
#include "talk/app/webrtc/peerconnectioninterface.h"

namespace node_webrtc {

namespace RTCOfferAnswerOptions {

  bool fromNode(
    v8::Local<v8::Value> unparsed,
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options,
    std::string* error);

}

}

#endif  // _RTCOFFERANSWEROPTIONS_H_
