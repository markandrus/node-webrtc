#include "src/idl/webrtc/bundlepolicy.h"

using node_webrtc::Convert;
using node_webrtc::Either;
using node_webrtc::RTCBundlePolicy;
using BundlePolicy = webrtc::PeerConnectionInterface::BundlePolicy;
using PeerConnectionInterface = webrtc::PeerConnectionInterface;

Either<std::string, BundlePolicy> Convert<BundlePolicy, RTCBundlePolicy>::From(RTCBundlePolicy bundlePolicy) {
  switch (bundlePolicy) {
    case node_webrtc::kBalanced:
      return Either<std::string, BundlePolicy>(PeerConnectionInterface::kBundlePolicyBalanced);
    case node_webrtc::kMaxCompat:
      return Either<std::string, BundlePolicy>(PeerConnectionInterface::kBundlePolicyMaxCompat);
    case node_webrtc::kMaxBundle:
      return Either<std::string, BundlePolicy>(PeerConnectionInterface::kBundlePolicyMaxBundle);
  }
}

RTCBundlePolicy Convert<BundlePolicy, RTCBundlePolicy>::To(BundlePolicy policy) {
  switch (policy) {
    case PeerConnectionInterface::kBundlePolicyBalanced:
      return node_webrtc::kBalanced;
    case PeerConnectionInterface::kBundlePolicyMaxBundle:
      return node_webrtc::kMaxBundle;
    case PeerConnectionInterface::kBundlePolicyMaxCompat:
      return node_webrtc::kMaxCompat;
  }
}
