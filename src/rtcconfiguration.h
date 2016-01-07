#ifndef _RTCCONFIGURATION_H_
#define _RTCCONFIGURATION_H_

#include <v8.h>
#include "talk/app/webrtc/peerconnectioninterface.h"

namespace node_webrtc {

namespace RTCConfiguration {

  typedef v8::Object RTCConfiguration;

  bool fromNode(
    v8::Local<v8::Value> unparsed,
    webrtc::PeerConnectionInterface::RTCConfiguration& configuration,
    std::string* error);

  v8::Local<RTCConfiguration> toNode(webrtc::PeerConnectionInterface::RTCConfiguration* configuration);

}

}

#endif  // _RTCCONFIGURATION_H_
