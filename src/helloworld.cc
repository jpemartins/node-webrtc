/* This code is PUBLIC DOMAIN, and is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND. See the accompanying 
 * LICENSE file.
 */

#include <v8.h>
#include <node.h>

using namespace node;
using namespace v8;

class WebRTC: ObjectWrap {

private:
  int m_count;
public:

  static Persistent<FunctionTemplate> s_ct;
  
  static void Init(Handle<Object> target)
  {
	HandleScope scope;

	Local<FunctionTemplate> t = FunctionTemplate::New(New);

	s_ct = Persistent<FunctionTemplate>::New(t);
	s_ct->InstanceTemplate()->SetInternalFieldCount(1);
	s_ct->SetClassName(String::NewSymbol("PeerConnection"));

	NODE_SET_PROTOTYPE_METHOD(s_ct, "addStream", AddStream);
	NODE_SET_PROTOTYPE_METHOD(s_ct, "send", Send);
	NODE_SET_PROTOTYPE_METHOD(s_ct, "close", Close);

	target->Set(String::NewSymbol("PeerConnection"), s_ct->GetFunction());
  }

  WebRTC() :
	m_count(0)
  {
  }

  ~WebRTC()
  {
  }

  static Handle<Value> New(const Arguments& args)
  {
	HandleScope scope;
	WebRTC* hw = new WebRTC();
	hw->Wrap(args.This());
	return args.This();
  }
  
  static Handle<Value> Send(const Arguments& args)
  {
	HandleScope scope;
	WebRTC* hw = ObjectWrap::Unwrap<WebRTC>(args.This());
	hw->m_count++;
	Local<String> result = String::New("Send");
	return scope.Close(result);
  }

  static Handle<Value> Close(const Arguments& args)
  {
	HandleScope scope;
	WebRTC* hw = ObjectWrap::Unwrap<WebRTC>(args.This());
	hw->m_count++;
	Local<String> result = String::New("Close");
	return scope.Close(result);
  }

  static Handle<Value> AddStream(const Arguments& args)
  {
	HandleScope scope;
	WebRTC* hw = ObjectWrap::Unwrap<WebRTC>(args.This());
	hw->m_count++;
	Local<String> result = String::New("AddStream");
	return scope.Close(result);
  }

};

Persistent<FunctionTemplate> WebRTC::s_ct;

extern "C" {
  static void init (Handle<Object> target)
  {
	WebRTC::Init(target);
  }

  NODE_MODULE(webrtc, init);
}
