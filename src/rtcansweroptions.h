#ifndef _RTCANSWEROPTIONS_H_
#define _RTCANSWEROPTIONS_H_

#include <v8.h>
#include "talk/app/webrtc/peerconnectioninterface.h"

namespace node_webrtc {

namespace RTCAnswerOptions {

  bool fromNode(
    v8::Local<v8::Value> unparsed,
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options,
    std::string* error);

}

}

#endif  // _RTCANSWEROPTIONS_H_
