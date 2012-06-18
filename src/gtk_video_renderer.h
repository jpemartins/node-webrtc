/*
 * libjingle
 * Copyright 2011, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef PEERCONNECTION_SAMPLES_CLIENT_LINUX_MAIN_WND_H_
#define PEERCONNECTION_SAMPLES_CLIENT_LINUX_MAIN_WND_H_

#include "talk/session/phone/mediachannel.h"
#include "talk/session/phone/videocommon.h"
#include "talk/session/phone/videoframe.h"
#include "talk/session/phone/videorenderer.h"
#include "talk/app/webrtc/mediastream.h"
 
// Forward declarations.
typedef struct _GtkWidget GtkWidget;
typedef union _GdkEvent GdkEvent;
typedef struct _GdkEventKey GdkEventKey;
typedef struct _GtkTreeView GtkTreeView;
typedef struct _GtkTreePath GtkTreePath;
typedef struct _GtkTreeViewColumn GtkTreeViewColumn;

class WindowCallback {
 public:
  virtual bool StartLogin(const std::string& server, int port) = 0;
  virtual void DisconnectFromServer() = 0;
  virtual void ConnectToPeer(int peer_id) = 0;
  virtual void DisconnectFromCurrentPeer() = 0;
  virtual void UIThreadCallback(int msg_id, void* data) = 0;
  virtual void Close() = 0;
 protected:
  virtual ~WindowCallback() {}
};

// Implements the main UI of the peer connection client.
// This is functionally equivalent to the MainWnd class in the Windows
// implementation.
class GtkMainWnd {
 public:
  GtkMainWnd();
  ~GtkMainWnd();

  virtual void RegisterObserver(WindowCallback* callback);
  virtual bool IsWindow();
  virtual void SwitchToStreamingUI();
  
  virtual webrtc::VideoRendererWrapperInterface* local_renderer();
  virtual webrtc::VideoRendererWrapperInterface* remote_renderer();

  virtual void QueueUIThreadCallback(int msg_id, void* data);

  // Creates and shows the main window with the |Connect UI| enabled.
  bool Create();

  // Destroys the window.  When the window is destroyed, it ends the
  // main message loop.
  bool Destroy();

  // Callback for when the main window is destroyed.
  void OnDestroyed(GtkWidget* widget, GdkEvent* event);

  // Callback for keystrokes.  Used to capture Esc and Return.
  void OnKeyPress(GtkWidget* widget, GdkEventKey* key);


  void OnRedraw();

 protected:
  class VideoRenderer : public cricket::VideoRenderer {
   public:
    explicit VideoRenderer(GtkMainWnd* main_wnd);
    virtual ~VideoRenderer();

    virtual bool SetSize(int width, int height, int reserved);

    virtual bool RenderFrame(const cricket::VideoFrame* frame);

    const uint8* image() const {
      return image_.get();
    }

    int width() const {
      return width_;
    }

    int height() const {
      return height_;
    }

   protected:
    talk_base::scoped_array<uint8> image_;
    int width_;
    int height_;
    GtkMainWnd* main_wnd_;
  };

 protected:
  GtkWidget* window_;  // Our main window.
  GtkWidget* draw_area_;  // The drawing surface for rendering video streams.
  
  WindowCallback* callback_;
  std::string server_;
  std::string port_;
  talk_base::scoped_refptr<webrtc::VideoRendererWrapperInterface>
      local_renderer_wrapper_;
  talk_base::scoped_refptr<webrtc::VideoRendererWrapperInterface>
      remote_renderer_wrapper_;
  talk_base::scoped_ptr<uint8> draw_buffer_;
  int draw_buffer_size_;
};

#endif  // PEERCONNECTION_SAMPLES_CLIENT_LINUX_MAIN_WND_H_
