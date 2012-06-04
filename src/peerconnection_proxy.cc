#include "peerconnection_proxy.h"

#include <vector>

#include "modules/video_capture/main/interface/video_capture_factory.h"
#include "talk/session/phone/voiceprocessor.h"

using namespace peerproxy;

PeerConnectionProxy::PeerConnectionProxy(webrtc::PeerConnectionObserver *observer) {
    connection_factory = webrtc::CreatePeerConnectionFactory(); 
    connection_factory_impl = (webrtc::PeerConnectionFactory*) connection_factory.get();

    channel_manager = connection_factory_impl->channel_manager();
#ifdef LEGACY
	connection = connection_factory->CreatePeerConnection(
      "STUN stun.l.google.com:19302", observer);
#else
	connection = connection_factory->CreateRoapPeerConnection(
      "STUN stun.l.google.com:19302", observer);
#endif

    connection.get();

	proxy_observer = observer; 
}

PeerConnectionProxy::~PeerConnectionProxy() {
}

void PeerConnectionProxy::SetWindow(GtkMainWnd* window_) {
	window = window_;
}

talk_base::scoped_refptr<webrtc::VideoCaptureModule> PeerConnectionProxy::OpenVideoCaptureDevice() 
{
  webrtc::VideoCaptureModule::DeviceInfo* device_info(
      webrtc::VideoCaptureFactory::CreateDeviceInfo(0));
  talk_base::scoped_refptr<webrtc::VideoCaptureModule> video_device;

  const size_t kMaxDeviceNameLength = 128;
  const size_t kMaxUniqueIdLength = 256;
#ifdef LEGACY_BUILD
  uint8 device_name[kMaxDeviceNameLength];
  uint8 unique_id[kMaxUniqueIdLength];
#else
  char device_name[kMaxDeviceNameLength];
  char unique_id[kMaxUniqueIdLength];
#endif

  const size_t device_count = device_info->NumberOfDevices();  
  for (size_t i = 0; i < device_count; ++i) {  	
    // Get the name of the video capture device.
    device_info->GetDeviceName(i, device_name, kMaxDeviceNameLength, unique_id,
        kMaxUniqueIdLength);

    // Try to open this device.
    video_device = webrtc::VideoCaptureFactory::Create(0, unique_id);
    if (video_device.get())
      break;
  }
  
  delete device_info;

  // TODO getUserMedia
  /* 
  vie = webrtc::ViERender::GetInterface(webrtc::VideoEngine::Create());
  int ret = vie->AddRenderer(0, window, 0, 0, 0, 0, 0);
  if (ret < 0){
    ERROR("fail to render");
  }
  */
  return video_device;
}

void PeerConnectionProxy::AddStream() {
	talk_base::scoped_refptr<webrtc::LocalAudioTrackInterface> audio_track(
	    connection_factory->CreateLocalAudioTrack(
	    	"audio_label", NULL));

  	talk_base::scoped_refptr<webrtc::LocalVideoTrackInterface> video_track(
      	connection_factory->CreateLocalVideoTrack(
        	"video_label", CreateVideoCapturer(OpenVideoCaptureDevice())));
	
  	talk_base::scoped_refptr<webrtc::LocalMediaStreamInterface> stream =
    	connection_factory->CreateLocalMediaStream("stream_label");

	stream->AddTrack(audio_track);
	stream->AddTrack(video_track);

	connection->AddStream(stream);		
	connection->CommitStreamChanges();
}

void PeerConnectionProxy::Send(const std::string& text) {
	connection->Send(text);
}

void PeerConnectionProxy::ProcessSignalingMessage(const std::string& text) {
	connection->ProcessSignalingMessage(text);
}

void PeerConnectionProxy::Close() {
	connection->Close();
}

int PeerConnectionProxy::sdp_state() {
	return connection->sdp_state();
}

int PeerConnectionProxy::ready_state() {
	return connection->ready_state();
}
