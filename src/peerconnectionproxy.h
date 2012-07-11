#ifndef PEERPROXY_PROXY_H_
#define PEERPROXY_PROXY_H_

#include "talk/base/scoped_ptr.h"

#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/webrtcsession.h"

#include "talk/session/phone/channelmanager.h"
#include "talk/session/phone/videorendererfactory.h"

#include "talk/p2p/client/httpportallocator.h"

#include "utils.h"
#include "gtk_video_renderer.h"


namespace node_webrtc {

class PeerConnectionProxy {
	private:
		talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> connection_factory;
		webrtc::PeerConnectionFactory* connection_factory_impl;
		
		talk_base::scoped_refptr<webrtc::PeerConnectionInterface> connection;
		cricket::ChannelManager* channel_manager;

		talk_base::scoped_ptr<cricket::PortAllocator> port_allocator;

		webrtc::PeerConnectionObserver* proxy_observer;

		GtkMainWnd* window;

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
