#include "observer.h"
#include "defaults.h"

#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/base/common.h"
#include "talk/base/logging.h"
#include "talk/p2p/client/basicportallocator.h"
#include "talk/session/phone/videorendererfactory.h"

#include <stdio.h>
#include <unistd.h>

#define INFO(msg) fprintf(stderr, "\033[01;34m%s \033[00m\n", msg)
#define INFO_TRACE(a, success) fprintf(stderr, "\033[01;34m%s\033[00m: %s\n", a, success ? "\033[01;32msuccess.\033[00m" : "\033[01;31mfailed.\033[00m")

int main(int argc, char **argv) {
		
	Observer obs;

	talk_base::scoped_ptr<webrtc::PeerConnection> connection;
	talk_base::scoped_ptr<webrtc::PeerConnectionFactory> connection_factory;
	talk_base::scoped_ptr<talk_base::Thread> worker_thread;
	talk_base::scoped_ptr<cricket::PortAllocator> port_allocator;

	worker_thread.reset(new talk_base::Thread());
	worker_thread->SetName("WorkerThread", &obs);
    worker_thread->Start();

	connection_factory.reset(new webrtc::PeerConnectionFactory(worker_thread.get()));    
    connection_factory->Initialize();

  	port_allocator.reset(new cricket::BasicPortAllocator(
      new talk_base::BasicNetworkManager(),
      talk_base::SocketAddress("stun.l.google.com", 19302),
      talk_base::SocketAddress(),
      talk_base::SocketAddress(),
      talk_base::SocketAddress()));

  	connection.reset(connection_factory->CreatePeerConnection(
      port_allocator.get(), worker_thread.get()));

    connection.get();

	connection->RegisterObserver(&obs);
	INFO("Registering Observer");

	bool success = connection->SetAudioDevice("", "", 0);
	INFO_TRACE("SetAudioDevice", success);

	success = connection->AddStream("audio_stream", false);
	INFO_TRACE("AddStream", success);

	success = connection->Connect();
	INFO_TRACE("Connect", success);
	
	while(1) {
		sleep(2);
	}
	return 0;
}