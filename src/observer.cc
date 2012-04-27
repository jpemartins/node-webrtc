#include <stdio.h>

#include "observer.h"
#include "defaults.h"

#include "talk/base/common.h"
#include "talk/base/logging.h"
#include "talk/p2p/client/basicportallocator.h"
#include "talk/session/phone/videorendererfactory.h"

Observer::Observer()
  : peer_id_(-1) {
}

Observer::~Observer() {
}

bool Observer::connection_active() const {
}

void Observer::Close() {
  DeletePeerConnection();
}

bool Observer::InitializePeerConnection() {
}

void Observer::DeletePeerConnection() {
}

//
// PeerConnectionObserver implementation.
//

void Observer::OnError() {
}

void Observer::OnSignalingMessage(const std::string& msg) {	
  //fprintf(stderr, "Signalling Message %s\n", msg.c_str());
}

// Called when a remote stream is added
void Observer::OnAddStream(const std::string& stream_id, bool video) {
  fprintf(stderr, "OnAddStream \n");
}

void Observer::OnRemoveStream(const std::string& stream_id, bool video) {
  fprintf(stderr, "OnRemoveStream \n");
}
