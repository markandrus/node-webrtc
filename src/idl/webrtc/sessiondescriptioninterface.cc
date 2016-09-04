#include "src/idl/webrtc/sessiondescriptioninterface.h"

#include "nan.h"

#include "src/idl/rtcsdptype.h"

using node_webrtc::Convert;
using node_webrtc::Either;
using node_webrtc::RTCSdpType;
using node_webrtc::RTCSessionDescriptionInit;
using v8::Local;
using v8::Value;
using webrtc::CreateSessionDescription;
using webrtc::SdpParseError;
using webrtc::SessionDescriptionInterface;

// TODO(mroberts): Is there a better way to support const than duplicating this
// code for both const and non-const versions?

Either<std::string, const SessionDescriptionInterface*> Convert<const SessionDescriptionInterface*, RTCSessionDescriptionInit>::From(RTCSessionDescriptionInit descriptionInit) {
  SdpParseError error;
  const SessionDescriptionInterface* description = CreateSessionDescription(
    Convert<RTCSdpType, std::string>::To(descriptionInit.type()),
    descriptionInit.sdp().value_or(""),
    &error);
  return description == nullptr
    ? Either<std::string, const SessionDescriptionInterface*>::Left(error.description)
    : Either<std::string, const SessionDescriptionInterface*>::Right(description);
}

RTCSessionDescriptionInit Convert<const SessionDescriptionInterface*, RTCSessionDescriptionInit>::To(const SessionDescriptionInterface* description) {
  // TODO(mroberts): We need a better way to handle null. Maybe Convert should
  // be unidirectional.
  if (description == nullptr) {
    return RTCSessionDescriptionInit();
  }
  Either<std::string, RTCSdpType> type = Convert<RTCSdpType, std::string>::From(description->type());
  if (!type) {
    // TODO(mroberts): Need to better handle when conversion fails (should be impossible).
  }
  std::string sdp;
  description->ToString(&sdp);
  return RTCSessionDescriptionInit(*type, Optional<std::string>(sdp));
}

Either<std::string, SessionDescriptionInterface*> Convert<SessionDescriptionInterface*, RTCSessionDescriptionInit>::From(RTCSessionDescriptionInit descriptionInit) {
  SdpParseError error;
  SessionDescriptionInterface* description = CreateSessionDescription(
    Convert<RTCSdpType, std::string>::To(descriptionInit.type()),
    descriptionInit.sdp().value_or(""),
    &error);
  return description == nullptr
    ? Either<std::string, SessionDescriptionInterface*>::Left(error.description)
    : Either<std::string, SessionDescriptionInterface*>::Right(description);
}

RTCSessionDescriptionInit Convert<SessionDescriptionInterface*, RTCSessionDescriptionInit>::To(SessionDescriptionInterface* description) {
  // TODO(mroberts): We need a better way to handle null. Maybe Convert should
  // be unidirectional.
  if (description == nullptr) {
    return RTCSessionDescriptionInit();
  }
  Either<std::string, RTCSdpType> type = Convert<RTCSdpType, std::string>::From(description->type());
  if (!type) {
    // TODO(mroberts): Need to better handle when conversion fails (should be impossible).
  }
  std::string sdp;
  description->ToString(&sdp);
  return RTCSessionDescriptionInit(*type, Optional<std::string>(sdp));
}
