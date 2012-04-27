/* This code is PUBLIC DOMAIN, and is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND. See the accompanying 
 * LICENSE file.
 */

#include <v8.h>
#include <node.h>

#include "defaults.h"

// PeerConnection
#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/base/common.h"
#include "talk/base/logging.h"
#include "talk/p2p/client/basicportallocator.h"
#include "talk/session/phone/videorendererfactory.h"

#include "talk/base/scoped_ptr.h"

#define WARN(msg) fprintf(stderr, "\033[01;33m native:%s \033[00m\n", msg)
#define INFO(msg) fprintf(stderr, "\033[01;34m native:%s \033[00m\n", msg)

using namespace node;
using namespace v8;

/*
template<typename VOICE, typename VIDEO>
class MediaEngineImpl<VOICE, VIDEO>: public cricket::CompositeMediaEngine<VOICE, VIDEO> {	
};
*/

class ObserverImpl: public webrtc::PeerConnectionObserver {	
	void OnSignalingMessage(const std::string& msg) {
		WARN("onsignalling message");
	}

	void OnAddStream(const std::string& stream_id, bool video) {
		WARN("onaddstream");
	}

	void OnRemoveStream(const std::string& stream_id, bool video) {
		WARN("onremovestream");
	}
};

class PeerConnection: ObjectWrap {
	private:
	talk_base::scoped_ptr<webrtc::PeerConnectionFactory> connection_factory;
	talk_base::scoped_ptr<talk_base::Thread> worker_thread;
	talk_base::scoped_ptr<cricket::PortAllocator> port_allocator;	

	ObserverImpl observer;
	talk_base::scoped_ptr<webrtc::PeerConnection> connection;

	public:
		
	static Persistent<FunctionTemplate> function_template;	

	static void Init(Handle<Object> target) {	
		HandleScope scope;

		Local<FunctionTemplate> t = FunctionTemplate::New(New);

		function_template = Persistent<FunctionTemplate>::New(t);
		function_template->SetClassName(String::NewSymbol("PeerConnection"));
		function_template->InstanceTemplate()->SetInternalFieldCount(1);			

		NODE_SET_PROTOTYPE_METHOD(function_template, "addStream", AddStream);
		NODE_SET_PROTOTYPE_METHOD(function_template, "send", Send);
		NODE_SET_PROTOTYPE_METHOD(function_template, "connect", Connect);
		NODE_SET_PROTOTYPE_METHOD(function_template, "close", Close);

		target->Set(String::NewSymbol("PeerConnection"), function_template->GetFunction());
	}


	PeerConnection() {		
		INFO("new PeerConnection");

		worker_thread.reset(new talk_base::Thread());
		worker_thread->SetName("WorkerThread", &observer);
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

		connection->RegisterObserver(&observer);
	}

	Handle<Boolean> AddStream(Local<String> label) {
		return Boolean::New(connection->AddStream(*v8::String::Utf8Value(label), false));
	}

	void Send(Local<String> text) {
	}

	Handle<Boolean> Connect() {
		return Boolean::New(connection->Connect());
	}

	Handle<Boolean> Close() {
		return Boolean::New(connection->Close());
	}

	~PeerConnection() {
	}

	static Handle<Value> New(const Arguments& args) {
		HandleScope scope;
		PeerConnection* peerconnection = new PeerConnection();
		peerconnection->Wrap(args.This());
		peerconnection->Ref();
		return args.This();
	}

	static Handle<Value> Send(const Arguments& args) {
		HandleScope scope;
		PeerConnection* peerconnection = ObjectWrap::Unwrap<PeerConnection>(args.This());
		peerconnection->Send(args[0]->ToString());
		return scope.Close(args.This());
	}
	
	static Handle<Value> Connect(const Arguments& args) {
		HandleScope scope;
		
		PeerConnection* peerconnection = ObjectWrap::Unwrap<PeerConnection>(args.This());
		Handle<Boolean> result = peerconnection->Connect();
		return scope.Close(result);
	}

	static Handle<Value> Close(const Arguments& args) {
		HandleScope scope;
		
		PeerConnection* peerconnection = ObjectWrap::Unwrap<PeerConnection>(args.This());
		peerconnection->Unref();

		Handle<Boolean> result = peerconnection->Close();
		return scope.Close(result);
	}

	static Handle<Value> AddStream(const Arguments& args) {
		HandleScope scope;
		PeerConnection* peerconnection = ObjectWrap::Unwrap<PeerConnection>(args.This());
		Handle<Boolean> result = peerconnection->AddStream(args[0]->ToString());
		return scope.Close(result);
	}
};

Persistent<FunctionTemplate> PeerConnection::function_template;

extern "C" {
	static void init (Handle<Object> target) {
		PeerConnection::Init(target);
	}

	NODE_MODULE(webrtc, init);
}
