#include <nan.h>

#include "talk/app/webrtc/peerconnectioninterface.h"

using namespace node;
using namespace v8;

namespace node_webrtc {

class PeerConnection;

class StatsObserver
  : public webrtc::StatsObserver
{
  private:
    PeerConnection* parent;
    Nan::Callback* callback;

  public:
    StatsObserver( PeerConnection* parent, Nan::Callback *callback )
    : parent(parent), callback(callback) {};

    virtual void OnComplete(const webrtc::StatsReports& reports);
};

}
