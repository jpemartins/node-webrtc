#include "peerconnection_observer.h"

using namespace peerproxy;

void PeerConnectionObserver::OnError(const std::string& msg) {
	WARN("onerror");
	ERROR(msg.c_str());
}

void PeerConnectionObserver::OnError() {
	WARN("onerror");
}

void PeerConnectionObserver::OnStateChange(webrtc::PeerConnectionObserver::StateType state) {
	WARN("onstatechange");
}

void PeerConnectionObserver::OnSignalingMessage(const std::string& msg) {
	WARN("onsignalingmessage");
	WARN(msg.c_str());
}

void PeerConnectionObserver::OnMessage(const std::string& msg) {
	WARN("onmessage");
}

void PeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface* stream) {
	WARN("onaddstream");
}

void PeerConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface* stream) {
	WARN("onremovestream");
}