#ifndef _GETUSERMEDIA_H_
#define _GETUSERMEDIA_H_

#include <v8.h>

#include "talk/app/webrtc/peerconnectioninterface.h"
#include "webrtc/base/scoped_ref_ptr.h"

namespace node_webrtc {

namespace GetUserMedia {

  void Init(
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory,
    v8::Handle<v8::Object> exports);

  NAN_METHOD(GetUserMedia);

}

}

#endif  // _GETUSERMEDIA_H_
