#include <node.h>
#include <v8.h>

#include "data/datachannel.h"
#include "media/capturer/filevideocapturer.h"
#include "media/getusermedia.h"
#include "media/mediastream.h"
#include "media/mediastreamtrack.h"
#include "media/recorder/mediarecorder.h"
#include "rtcconfiguration.h"
#include "rtcicecandidate.h"
#include "rtcpeerconnection.h"
#include "rtcsessiondescription.h"
#include "stats/rtcstatsreport.h"
#include "stats/rtcstatsresponse.h"

#include "talk/app/webrtc/peerconnectioninterface.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/thread.h"

static rtc::scoped_ptr<rtc::Thread> signalingThread;
static rtc::scoped_ptr<rtc::Thread> workerThread;
static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;

static void setupPeerConnectionFactory(
  rtc::scoped_ptr<rtc::Thread>& signalingThread,
  rtc::scoped_ptr<rtc::Thread>& workerThread,
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>& peerConnectionFactory)
{
  bool result;

  // NOTE(mroberts): Need to create signaling and worker Threads before calling
  // CreatePeerConnectionFactory.
  //
  //   https://bugs.chromium.org/p/webrtc/issues/detail?id=4196
  //
  signalingThread.reset(new rtc::Thread());
  result = signalingThread->Start();
  assert(result);

  workerThread.reset(new rtc::Thread());
  result = workerThread->Start();
  assert(result);

  result = rtc::InitializeSSL();
  assert(result);

  peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
    signalingThread.get(),
    workerThread.get(),
    nullptr,
    nullptr,
    nullptr);
}

void init(v8::Handle<v8::Object> exports) {
  setupPeerConnectionFactory(signalingThread, workerThread, peerConnectionFactory);

  node_webrtc::FileVideoCapturer::Init(peerConnectionFactory, exports);
  node_webrtc::GetUserMedia::Init(peerConnectionFactory, exports);
  node_webrtc::DataChannel::Init(exports);
  node_webrtc::MediaRecorder::Init(exports);
  node_webrtc::MediaStream::Init(peerConnectionFactory, exports);
  node_webrtc::MediaStreamTrack::Init(exports);
  node_webrtc::PeerConnection::Init(peerConnectionFactory, exports);
  node_webrtc::RTCStatsReport::Init(exports);
  node_webrtc::RTCStatsResponse::Init(exports);
  node_webrtc::RTCSessionDescription::Init(exports);
  node_webrtc::RTCIceCandidate::Init(exports);
}

NODE_MODULE(wrtc, init)
