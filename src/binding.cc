#include <v8.h>
#include <node.h>
#include <ev.h>
#include <gtk/gtk.h>

#include "talk/app/webrtc/peerconnection.h"

#include "gtk_video_renderer.h"
#include "peerconnection_proxy.h"
#include "utils.h"

using namespace node;
using namespace v8;

class PeerConnection: ObjectWrap, webrtc::PeerConnectionObserver {
	private:
	peerproxy::PeerConnectionProxy* connection_proxy;			

	public:
	
	static ev_async eio_signal_ev;
	static std::string message;
	static Local<Value> callback;
	
	static GtkMainWnd window;
	static Persistent<FunctionTemplate> function_template;

	static Persistent<String> onsigmessage_symbol,
	 	onaddstream_symbol, onconnecting_symbol, onclose_symbol;

	static void Init(Handle<Object> target) {
		HandleScope scope;
		
		Local<FunctionTemplate> t = FunctionTemplate::New(New);

		function_template = Persistent<FunctionTemplate>::New(t);
		function_template->SetClassName(String::NewSymbol("PeerConnection"));
		function_template->InstanceTemplate()->SetInternalFieldCount(1);			

		NODE_SET_PROTOTYPE_METHOD(function_template, "addStream", AddStream);
		NODE_SET_PROTOTYPE_METHOD(function_template, "processSignalingMessage", ProcessSignalingMessage);
		NODE_SET_PROTOTYPE_METHOD(function_template, "send", Send);
		NODE_SET_PROTOTYPE_METHOD(function_template, "close", Close);		

		target->Set(String::NewSymbol("PeerConnection"), function_template->GetFunction());		
	}
	
	PeerConnection() {
		connection_proxy = new peerproxy::PeerConnectionProxy(this);
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
		callback = this->handle_->Get(NODE_PSYMBOL("onerror"));

		ev_async_send(EV_DEFAULT_UC_ &eio_signal_ev);
	}
	
	void OnError() {
		message = std::string("");
		callback = this->handle_->Get(NODE_PSYMBOL("onerror"));

		ev_async_send(EV_DEFAULT_UC_ &eio_signal_ev);
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

			ev_async_send(EV_DEFAULT_UC_ &eio_signal_ev);
			return;
		}
		//Trigger onconnecting
		//Trigger onclose
		*/
	}
		
	void OnMessage(const std::string& msg) {
		message = msg;
		callback = this->handle_->Get(NODE_PSYMBOL("onmessage"));

		ev_async_send(EV_DEFAULT_UC_ &eio_signal_ev);
	}
	
	void OnAddStream(webrtc::MediaStreamInterface* stream) {
		talk_base::scoped_refptr<webrtc::VideoTracks> tracks = stream->video_tracks();
      	for (size_t i = 0; i < tracks->count(); ++i) {
        	webrtc::VideoTrackInterface* track = tracks->at(i);
        	INFO("Setting video renderer for track: ");
	        track->SetRenderer(window->remote_renderer());
      	}
      	// If we haven't shared any streams with this peer (we're the receiver)
      	// then do so now.
      	stream->Release();
		message = std::string();
		callback = this->handle_->Get(NODE_PSYMBOL("onaddstream"));

		ev_async_send(EV_DEFAULT_UC_ &eio_signal_ev);
	}
	
	void OnRemoveStream(webrtc::MediaStreamInterface* stream) {
		message = std::string();
		callback = this->handle_->Get(NODE_PSYMBOL("onremovestream"));

		ev_async_send(EV_DEFAULT_UC_ &eio_signal_ev);
	}

	void OnSignalingMessage(const std::string& msg) {
		message = msg;
		callback = this->handle_->Get(NODE_PSYMBOL("onsignalingmessage"));

		ev_async_send(EV_DEFAULT_UC_ &eio_signal_ev);
	}

	static void OnCallback(EV_P_ ev_async *watcher, int revents) {		
		HandleScope scope;

		assert(watcher == &eio_signal_ev);

		if (!callback->IsFunction()) {
			return;
		}
		
		Persistent<Function> callback_function = Persistent<Function>::New(Local<Function>::Cast(callback));	
		Local<Value> argv[1];
		argv[0] = Local<Value>::New(String::New(message.c_str()));
		callback_function->Call(Context::GetCurrent()->Global(), 1, argv);
	}

	
	static Handle<Value> New(const Arguments& args) {
    	ev_async_init(&eio_signal_ev, &OnCallback);
    	ev_async_start(EV_DEFAULT_UC_ &eio_signal_ev);
    	ev_unref(EV_DEFAULT_UC);

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
};

Persistent<FunctionTemplate> PeerConnection::function_template;

ev_async PeerConnection::eio_signal_ev;
std::string PeerConnection::message;
Local<Value> PeerConnection::callback;
GtkMainWnd PeerConnection::window;

static void GtkMain() {
   gdk_threads_enter();
   gtk_main();
   gdk_threads_leave();

   PeerConnection::window.Destroy();
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
