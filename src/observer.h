/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PEERCONNECTION_SAMPLES_CLIENT_CONDUCTOR_H_
#define PEERCONNECTION_SAMPLES_CLIENT_CONDUCTOR_H_
#pragma once

#include <deque>
#include <set>
#include <string>

#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/base/scoped_ptr.h"

namespace cricket {
  class VideoRenderer;
  class PortAllocator;
}  // namespace cricket

class Observer
  : public webrtc::PeerConnectionObserver {

 public:
  enum CallbackID {
    MEDIA_CHANNELS_INITIALIZED = 1,
    PEER_CONNECTION_CLOSED,
    SEND_MESSAGE_TO_PEER,
    PEER_CONNECTION_ADDSTREAMS,
    PEER_CONNECTION_ERROR,
    NEW_STREAM_ADDED,
    STREAM_REMOVED,
  };

  Observer();
  ~Observer();

  bool connection_active() const;

  virtual void Close();

 protected:
  bool InitializePeerConnection();
  void DeletePeerConnection();

  bool AddStream(const std::string& id, bool video);
  void AddStreams();

  //
  // PeerConnectionObserver implementation.
  //
  virtual void OnError();
  virtual void OnSignalingMessage(const std::string& msg);

  // Called when a remote stream is added
  virtual void OnAddStream(const std::string& stream_id, bool video);

  virtual void OnRemoveStream(const std::string& stream_id,
                              bool video);

 protected:
  int peer_id_;
  talk_base::scoped_ptr<webrtc::PeerConnection> peer_connection_;
  talk_base::scoped_ptr<webrtc::PeerConnectionFactory> peer_connection_factory_;
  talk_base::scoped_ptr<talk_base::Thread> worker_thread_;

  std::deque<std::string*> pending_messages_;
  std::set<std::string> active_streams_;
  talk_base::scoped_ptr<cricket::PortAllocator> port_allocator_;
};

#endif  // PEERCONNECTION_SAMPLES_CLIENT_CONDUCTOR_H_
