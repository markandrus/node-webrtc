#include <iostream>

#include "common.h"
#include "peerconnection.h"
#include "stats-observer.h"

using namespace node_webrtc;

void StatsObserver::OnComplete( const std::vector<webrtc::StatsReport>& reports )
{
  TRACE_CALL;
  webrtc::StatsReports* reports_ = new webrtc::StatsReports(reports.size());
  webrtc::StatsReports::const_iterator it;
  for (it = reports.begin(); it != reports.end(); ++it) {
    reports_->push_back(*it);
  }
  PeerConnection::GetStatsEvent* data = new PeerConnection::GetStatsEvent(callback, reports_);
  parent->QueueEvent(PeerConnection::GET_STATS_SUCCESS, static_cast<void*>(data));
  TRACE_END;
}
