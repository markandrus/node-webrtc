#include "rtcansweroptions.h"
#include "rtcofferansweroptions.h"

using namespace node_webrtc;
using namespace v8;

bool RTCAnswerOptions::fromNode(Local<Value> unparsed, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options, std::string* error)
{
  return RTCOfferAnswerOptions::fromNode(unparsed, options, error);
}
