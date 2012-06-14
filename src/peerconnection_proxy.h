#ifndef PEERPROXY_PROXY_H_
#define PEERPROXY_PROXY_H_

#include "talk/base/scoped_ptr.h"

#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/webrtcsession.h"

#include "talk/session/phone/channelmanager.h"
#include "talk/session/phone/videorendererfactory.h"

#include "talk/p2p/client/httpportallocator.h"

#include "video_engine/include/vie_base.h"
#include "video_engine/include/vie_render.h"

#include "utils.h"
#include "peerconnection_observer.h"
#include "gtk_video_renderer.h"


namespace peerproxy {

class PeerConnectionProxy /*: public webrtc::PeerConnectionObserver*/ {
	private:
		talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> connection_factory;
		webrtc::PeerConnectionFactory* connection_factory_impl;
		
		talk_base::scoped_refptr<webrtc::PeerConnectionInterface> connection;
		cricket::ChannelManager* channel_manager;

		talk_base::scoped_ptr<cricket::PortAllocator> port_allocator;
		talk_base::scoped_ptr<webrtc::WebRtcSession> session;

		webrtc::PeerConnectionObserver* proxy_observer;

		GtkMainWnd* window;
		webrtc::ViERender* vie;

	public:
		PeerConnectionProxy();
		PeerConnectionProxy(webrtc::PeerConnectionObserver *observer);
		~PeerConnectionProxy();

		void SetWindow(GtkMainWnd* window_);
		talk_base::scoped_refptr<webrtc::VideoCaptureModule> OpenVideoCaptureDevice();

		void AddStream();
		void Send(const std::string& text);
		void ProcessSignalingMessage(const std::string& text);
		void Close();


		int ready_state();
		int sdp_state();
};

}

#endif
