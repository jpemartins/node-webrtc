#include <v8.h>
#include <node.h>
#include <uv.h>
#include <gtk/gtk.h>

#include "talk/app/webrtc/peerconnection.h"

#include "gtk_video_renderer.h"
#include "peerconnectionproxy.h"
#include "utils.h"

using namespace node;
using namespace v8;

static void GtkIteration();

class PeerConnection: ObjectWrap, webrtc::PeerConnectionObserver {
	private:
	node_webrtc::PeerConnectionProxy* connection_proxy;			

	public:
	
	static uv_async_t uv_signal;
	
	static std::string message;	
	static ObjectWrap* wrap;
	static Local<Value> callback;
	static std::string callback_name;
	
	static GtkMainWnd window;
	static Persistent<FunctionTemplate> function_template;

	static Persistent<String> onsigmessage_symbol,
	 	onaddstream_symbol, onconnecting_symbol, onclose_symbol;

	static void Init(Handle<Object> target) {
		HandleScope scope;
		
		Local<FunctionTemplate> t = FunctionTemplate::New(New);
		Local<FunctionTemplate> r = FunctionTemplate::New(Render);

		function_template = Persistent<FunctionTemplate>::New(t);
		function_template->SetClassName(String::NewSymbol("PeerConnection"));
		function_template->InstanceTemplate()->SetInternalFieldCount(1);			

		NODE_SET_PROTOTYPE_METHOD(function_template, "addStream", AddStream);
		NODE_SET_PROTOTYPE_METHOD(function_template, "processSignalingMessage", ProcessSignalingMessage);
		NODE_SET_PROTOTYPE_METHOD(function_template, "send", Send);
		NODE_SET_PROTOTYPE_METHOD(function_template, "close", Close);		

		target->Set(String::NewSymbol("PeerConnection"), function_template->GetFunction());		
		target->Set(String::NewSymbol("render"), r->GetFunction());
	}
	
	PeerConnection() {
		connection_proxy = new node_webrtc::PeerConnectionProxy(this);
		connection_proxy->SetWindow(&window);
	}

	~PeerConnection() {		
	}

	void AddStream() {
		connection_proxy->AddStream();
	}

	void Send(Local<String> text) {
		connection_proxy->Send(*String::AsciiValue(text));
	}

	void ProcessSignalingMessage(Local<String> text) {
		connection_proxy->ProcessSignalingMessage(*String::AsciiValue(text));
	}

	void Close() {
		connection_proxy->Close();		
	}

	void OnError(const std::string& msg) {
		message = msg;
		wrap = this;
		callback_name = "onerror";

		uv_async_send(&uv_signal);
	}
	
	void OnError() {
		message = std::string("");
		wrap = this;
		callback_name = "onerror";

		uv_async_send(&uv_signal);
	}
	
	void OnStateChange(webrtc::PeerConnectionObserver::StateType state) {		
		/*
		bool isReady = state == 0
		   , isIce = state == 1
		   , isSDP = state == 2;
		
		//Trigger onconnecting
		if (isReady && connection_proxy->ready_state() == 0) {
			callback = this->handle_->Get(NODE_PSYMBOL("onconnecting"));
			message = std::string();

			uv_async_send(EV_DEFAULT_UC_ &uv_signal);
			return;
		}
		//Trigger onconnecting
		//Trigger onclose
		*/
	}
		
	void OnMessage(const std::string& msg) {
		message = msg;
		wrap = this;
		callback_name = "onmessage";

		uv_async_send(&uv_signal);
	}
	
	void OnAddStream(webrtc::MediaStreamInterface* stream) {
		talk_base::scoped_refptr<webrtc::VideoTracks> tracks = stream->video_tracks();
      	for (size_t i = 0; i < tracks->count(); ++i) {
        	webrtc::VideoTrackInterface* track = tracks->at(i);
        	INFO("Setting video renderer for track: ");
	        track->SetRenderer(window.remote_renderer());
      	}
      	// If we haven't shared any streams with this peer (we're the receiver)
      	// then do so now.
      	stream->Release();
      	
		message = std::string();
		wrap = this;
		callback_name = "onaddstream";

		uv_async_send(&uv_signal);
	}
	
	void OnRemoveStream(webrtc::MediaStreamInterface* stream) {
		message = std::string();
		wrap = this;
		callback_name = "onremovestream";

		uv_async_send(&uv_signal);
	}

	void OnSignalingMessage(const std::string& msg) {
		message = msg;
		callback_name = "onsignalingmessage";
		wrap = this;		

		uv_async_send(&uv_signal);
	}

	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
	}

	void OnIceComplete() {
	}

	static void OnCallback(uv_async_t *watcher, int status) {		
		HandleScope scope;
		
		callback = wrap->handle_->Get(NODE_PSYMBOL(callback_name.c_str()));

		if (!callback->IsFunction()) {
			return;
		}
		
		Persistent<Function> callback_function = Persistent<Function>::New(Local<Function>::Cast(callback));	
		Local<Value> argv[1];
		argv[0] = Local<Value>::New(String::New(message.c_str()));
		callback_function->Call(Context::GetCurrent()->Global(), 1, argv);
	}

	
	static Handle<Value> New(const Arguments& args) {
    	uv_async_init(uv_default_loop(), &uv_signal, OnCallback);

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
	
	static Handle<Value> Close(const Arguments& args) {
		HandleScope scope;
		PeerConnection* peerconnection = ObjectWrap::Unwrap<PeerConnection>(args.This());
		peerconnection->Unref();
		peerconnection->Close();
		return scope.Close(args.This());
	}

	static Handle<Value> AddStream(const Arguments& args) {		
		// start event handler for callback
		HandleScope scope;
		PeerConnection* peerconnection = ObjectWrap::Unwrap<PeerConnection>(args.This());
		peerconnection->AddStream();
		return scope.Close(args.This());
	}

	static Handle<Value> ProcessSignalingMessage(const Arguments& args) {		
		HandleScope scope;
		PeerConnection* peerconnection = ObjectWrap::Unwrap<PeerConnection>(args.This());
		peerconnection->ProcessSignalingMessage(args[0]->ToString());
		return scope.Close(args.This());
	}

	static Handle<Value> Render(const Arguments& args) {
		HandleScope scope;
		GtkIteration();
		return scope.Close(args.This());
	}
};

Persistent<FunctionTemplate> PeerConnection::function_template;

uv_async_t PeerConnection::uv_signal;
std::string PeerConnection::message;
Local<Value> PeerConnection::callback;
std::string PeerConnection::callback_name;
GtkMainWnd PeerConnection::window;
ObjectWrap* PeerConnection::wrap;

static void GtkIteration() {
	while (gtk_events_pending())
        gtk_main_iteration();
}

static void GtkInit() {
	gtk_init(NULL, NULL);
  	
  	g_type_init();
	g_thread_init(NULL);

	PeerConnection::window.Create();
}

extern "C" {	
	static void init (Handle<Object> target) {		
		PeerConnection::Init(target);

		GtkInit();	
	}

	NODE_MODULE(webrtc, init);
}
