#include <v8.h>
#include "common.h"
#include "peerconnection.h"
#include "setsessiondescriptionobserver.h"

using namespace v8;
using namespace node_webrtc;

void SetSessionDescriptionObserver::HandleAsyncEvent()
{
  TRACE_CALL;
  Nan::HandleScope scope;
  if (this->error) {
    this->RejectWithError(this->error);
    TRACE_END;
    return;
  } else if (this->local) {
    this->pc->pendingLocalDescription = nullptr;
    this->pc->currentLocalDescription = this->description;
  } else {
    this->pc->pendingRemoteDescription = nullptr;
    this->pc->currentRemoteDescription = this->description;
  }
  this->Resolve(Nan::Undefined());
  TRACE_END;
}

void SetSessionDescriptionObserver::OnFailure(const std::string& error)
{
  TRACE_CALL;
  this->error = new std::string(error);
  this->Done();
  TRACE_END;
}

void SetSessionDescriptionObserver::OnSuccess()
{
  TRACE_CALL;
  this->Done();
  TRACE_END;
}
