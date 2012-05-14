#ifndef PEERPROXY_OBSERVER_H_
#define PEERPROXY_OBSERVER_H_

#include "talk/app/webrtc/peerconnection.h"

#include "utils.h"

namespace peerproxy {
	
	class PeerConnectionObserver: public webrtc::PeerConnectionObserver {
		void OnError(const std::string& msg);
		void OnError();
		void OnStateChange(webrtc::PeerConnectionObserver::StateType state);
		void OnSignalingMessage(const std::string& msg);
		void OnMessage(const std::string& msg);
		void OnAddStream(webrtc::MediaStreamInterface* stream);
		void OnRemoveStream(webrtc::MediaStreamInterface* stream);		
	};

}

#endif
