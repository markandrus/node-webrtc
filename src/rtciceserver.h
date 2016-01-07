#ifndef _RTCICESERVER_H_
#define _RTCICESERVER_H_

#include <v8.h>
#include "talk/app/webrtc/peerconnectioninterface.h"

namespace node_webrtc {

namespace RTCIceServer {

  typedef v8::Object RTCIceServer;

  bool fromNode(
    v8::Local<v8::Value> unparsed,
    webrtc::PeerConnectionInterface::IceServer& iceServer,
    std::string* error);

  v8::Local<RTCIceServer> toNode(webrtc::PeerConnectionInterface::IceServer* iceServer);

}

}

#endif  // _RTCICESERVER_H_
