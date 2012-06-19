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


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stddef.h>

#include "talk/base/common.h"
#include "talk/base/logging.h"
#include "talk/base/stringutils.h"

#include "gtk_video_renderer.h"
#include "utils.h"

using talk_base::sprintfn;

namespace {

//
// Simple static functions that simply forward the callback to the
// GtkMainWnd instance.
//

gboolean OnDestroyedCallback(GtkWidget* widget, GdkEvent* event,
                             gpointer data) {
  reinterpret_cast<GtkMainWnd*>(data)->OnDestroyed(widget, event);
  return FALSE;
}

gboolean OnKeyPressCallback(GtkWidget* widget, GdkEventKey* key,
                            gpointer data) {
  reinterpret_cast<GtkMainWnd*>(data)->OnKeyPress(widget, key);
  return false;
}

// Creates a tree view, that we use to display the list of peers.
void InitializeList(GtkWidget* list) {
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes(
      "List Items", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
  GtkListStore* store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
  gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));
  g_object_unref(store);
}

// Adds an entry to a tree view.
void AddToList(GtkWidget* list, const gchar* str, int value) {
  GtkListStore* store = GTK_LIST_STORE(
      gtk_tree_view_get_model(GTK_TREE_VIEW(list)));

  GtkTreeIter iter;
  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, str, 1, value, -1);
}

struct UIThreadCallbackData {
  explicit UIThreadCallbackData(WindowCallback* cb, int id, void* d)
      : callback(cb), msg_id(id), data(d) {}
  WindowCallback* callback;
  int msg_id;
  void* data;
};

gboolean HandleUIThreadCallback(gpointer data) {
  UIThreadCallbackData* cb_data = reinterpret_cast<UIThreadCallbackData*>(data);
  cb_data->callback->UIThreadCallback(cb_data->msg_id, cb_data->data);
  delete cb_data;
  return false;
}

gboolean Redraw(gpointer data) {
  GtkMainWnd* wnd = reinterpret_cast<GtkMainWnd*>(data);
  wnd->OnRedraw();
  return false;
}
}  // end anonymous

//
// GtkMainWnd implementation.
//

GtkMainWnd::GtkMainWnd()
    : window_(NULL), draw_area_(NULL), callback_(NULL),
      server_("localhost") {
  char buffer[10];
  sprintfn(buffer, sizeof(buffer), "%i", 8888);
  port_ = buffer;
}

GtkMainWnd::~GtkMainWnd() {
  ASSERT(!IsWindow());
}

void GtkMainWnd::RegisterObserver(WindowCallback* callback) {
  callback_ = callback;
}

bool GtkMainWnd::IsWindow() {
  return window_ != NULL && GTK_IS_WINDOW(window_);
}

webrtc::VideoRendererWrapperInterface* GtkMainWnd::local_renderer() {
  if (!local_renderer_wrapper_.get())
    local_renderer_wrapper_  =
        webrtc::CreateVideoRenderer(new VideoRenderer(this));
  return local_renderer_wrapper_.get();
}

webrtc::VideoRendererWrapperInterface*  GtkMainWnd::remote_renderer() {
  if (!remote_renderer_wrapper_.get())
    remote_renderer_wrapper_ =
        webrtc::CreateVideoRenderer(new VideoRenderer(this));
  return remote_renderer_wrapper_.get();
}

void GtkMainWnd::QueueUIThreadCallback(int msg_id, void* data) {
  g_idle_add(HandleUIThreadCallback,
             new UIThreadCallbackData(callback_, msg_id, data));
}

bool GtkMainWnd::Create() {
  ASSERT(window_ == NULL);

  window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL); 
    gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window_), 640, 480);
    gtk_window_set_title(GTK_WINDOW(window_), "Remote Media");
    g_signal_connect(G_OBJECT(window_), "delete-event",
                     G_CALLBACK(&OnDestroyedCallback), this);
    g_signal_connect(window_, "key-press-event", G_CALLBACK(OnKeyPressCallback),
                     this);

    SwitchToStreamingUI();

  return window_ != NULL;
}

bool GtkMainWnd::Destroy() {
  if (!IsWindow())
    return false;

  gtk_widget_destroy(window_);
  window_ = NULL;

  return true;
}

void GtkMainWnd::SwitchToStreamingUI() {
  LOG(INFO) << __FUNCTION__;

  ASSERT(draw_area_ == NULL);

  gtk_container_set_border_width(GTK_CONTAINER(window_), 0);
  draw_area_ = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window_), draw_area_);

  gtk_widget_show_all(window_);
}

void GtkMainWnd::OnDestroyed(GtkWidget* widget, GdkEvent* event) {
  callback_->Close();
  window_ = NULL;
  draw_area_ = NULL;
}

void GtkMainWnd::OnKeyPress(GtkWidget* widget, GdkEventKey* key) {
  if (key->type == GDK_KEY_PRESS) {
    switch (key->keyval) {
     case GDK_Escape:
       if (draw_area_) {
         callback_->DisconnectFromCurrentPeer();
       } 
       break;

     case GDK_KP_Enter:
     case GDK_Return:
       break;

     default:
       break;
    }
  }
}

void GtkMainWnd::OnRedraw() {
  gdk_threads_enter();

  VideoRenderer* remote_renderer =
      static_cast<VideoRenderer*>(remote_renderer_wrapper_->renderer());
  if (remote_renderer && remote_renderer->image() != NULL &&
      draw_area_ != NULL) {
    int width = remote_renderer->width();
    int height = remote_renderer->height();

    if (!draw_buffer_.get()) {
      draw_buffer_size_ = (width * height) * 4;
      draw_buffer_.reset((unsigned char*) remote_renderer->image());
      gtk_widget_set_size_request(draw_area_, width, height);
    }

    gdk_draw_rgb_32_image(draw_area_->window,
                          draw_area_->style->fg_gc[GTK_STATE_NORMAL],
                          0,
                          0,
                          width,
                          height,
                          GDK_RGB_DITHER_NONE,
                          draw_buffer_.get(),
                          width * 4);
  }

  gdk_threads_leave();
}

GtkMainWnd::VideoRenderer::VideoRenderer(GtkMainWnd* main_wnd)
    : width_(0), height_(0), main_wnd_(main_wnd) {
}

GtkMainWnd::VideoRenderer::~VideoRenderer() {
}

bool GtkMainWnd::VideoRenderer::SetSize(int width, int height, int reserved) {
  gdk_threads_enter();
  width_ = width;
  height_ = height;
  image_.reset(new uint8[width * height * 4]);
  gdk_threads_leave();
  return true;
}

bool GtkMainWnd::VideoRenderer::RenderFrame(const cricket::VideoFrame* frame) {
  gdk_threads_enter();

  int size = width_ * height_ * 4;
  // TODO: Convert directly to RGBA
  frame->ConvertToRgbBuffer(cricket::FOURCC_ARGB,
                            image_.get(),
                            size,
                            width_ * 4);
  // Convert the B,G,R,A frame to R,G,B,A, which is accepted by GTK.
  // The 'A' is just padding for GTK, so we can use it as temp.
  uint8* pix = image_.get();
  uint8* end = image_.get() + size;
  while (pix < end) {
    pix[3] = pix[0];     // Save B to A.
    pix[0] = pix[2];  // Set Red.
    pix[2] = pix[3];  // Set Blue.
    pix[3] = 0xFF;     // Fixed Alpha.
    pix += 4;
  }

  gdk_threads_leave();

  g_idle_add(Redraw, main_wnd_);  
  return true;
}


