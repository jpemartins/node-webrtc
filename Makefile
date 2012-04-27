WEBRTC_ROOT_PATH = deps/webrtc/stable

DEFS= -D_LARGEFILE_SOURCE \
	-D_FILE_OFFSET_BITS=64 \
	-DWEBRTC_TARGET_PC  \
	-DWEBRTC_LINUX  \
	-DWEBRTC_THREAD_RR  \
	-DEXPAT_RELATIVE_PATH  \
	-DWEBRTC_RELATIVE_PATH  \
	-DLINUX  \
	-DPOSIX  \
	-D__STDC_FORMAT_MACROS  \
	-DDYNAMIC_ANNOTATIONS_ENABLED=1  \
	-DWTF_USE_DYNAMIC_ANNOTATIONS=1

# Flags passed to all source files.
#   -fvisibility=hidden NodeJS Does not like it
CFLAGS= -pthread \
	-fno-exceptions \
	-fno-strict-aliasing \
	-Wall \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-Wextra \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-I/usr/include/atk-1.0 \
	-I/usr/include/pango-1.0 \
	-I/usr/include/gio-unix-2.0/ \
	-I/usr/include/glib-2.0 \
	-I/usr/lib/i386-linux-gnu/glib-2.0/include \
	-I/usr/include/freetype2 \
	-I/usr/include/libpng12 \
	-I/usr/include/gtk-2.0 \
	-I/usr/lib/gtk-2.0/include \
	-I/usr/include/cairo \
	-I/usr/include/gdk-pixbuf-2.0 \
	-I/usr/include/pixman-1 \
	-g

CCFLAGS = -fno-rtti \
	-fno-threadsafe-statics \
	-fvisibility-inlines-hidden \
	-Wsign-compare \
	-D_LARGEFILE_SOURCE \
	-D_FILE_OFFSET_BITS=64

INCLUDE_DIRS = -I/usr/include/nodejs/  \
	-I$(WEBRTC_ROOT_PATH)/third_party/libjingle/source \
	-I$(WEBRTC_ROOT_PATH)/third_party_mods/libjingle/source \
	-I$(WEBRTC_ROOT_PATH)/src

BUILD_DIR = build/
NODE_MODULE = webrtc.node

NODE_FLAGS = -o$(BUILD_DIR)/$(NODE_MODULE)
FLAGS = -obin/sample

WEBRTC_OBJS = $(BUILD_DIR)/defaults.o $(BUILD_DIR)/observer.o

NODE_OBJS = $(WEBRTC_OBJS) $(BUILD_DIR)/binding.o
SAMPLE_OBJS = $(WEBRTC_OBJS) $(BUILD_DIR)/main.o

WEBRTC_LIB = lib/webrtc.a
WEBRTC_LIBS := $(WEBRTC_ROOT_PATH)/out/Debug/obj.target/third_party_mods/libjingle/libjingle_app.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/third_party/libsrtp/libsrtp.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/third_party_mods/libjingle/libjsoncpp.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libvideo_capture_module.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libwebrtc_utility.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libaudio_coding_module.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libCNG.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/common_audio/libsignal_processing.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libG711.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libG722.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libiLBC.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libiSAC.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libiSACFix.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libPCM16B.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libNetEq.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/common_audio/libresampler.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/common_audio/libvad.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/system_wrappers/source/libsystem_wrappers.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libwebrtc_video_coding.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libwebrtc_i420.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libwebrtc_vp8.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/common_video/libwebrtc_libyuv.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/third_party/libyuv/libyuv.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/third_party/libvpx/libvpx.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libvideo_render_module.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/video_engine/libvideo_engine_core.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/common_video/libwebrtc_jpeg.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/third_party/libjpeg_turbo/libjpeg_turbo.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libmedia_file.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/librtp_rtcp.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libudp_transport.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libvideo_processing.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libvideo_processing_sse2.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/voice_engine/libvoice_engine_core.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libaudio_conference_mixer.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libaudio_processing.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libaec.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libapm_util.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libaec_sse2.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libaecm.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libagc.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libns.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libaudioproc_debug_proto.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/third_party/protobuf/libprotobuf_lite.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/src/modules/libaudio_device.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/third_party_mods/libjingle/libjingle_p2p.a \
	$(WEBRTC_ROOT_PATH)/out/Debug/obj.target/third_party_mods/libjingle/libjingle.a \

NODE_WEBRTC_LDFLAGS = -pthread -Wl,-z,noexecstack -fPIC -L/usr/lib/i386-linux-gnu -m32 $(NODE_FLAGS) -Wl,--start-group $(NODE_OBJS) $(WEBRTC_LIBS) -Wl,--end-group -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lm -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 -lX11 -lXext -lexpat -ldl -lasound -lpulse -shared

WEBRTC_LDFLAGS = -pthread -Wl,-z,noexecstack -fPIC -L/usr/lib/i386-linux-gnu -m32 $(FLAGS) -Wl,--start-group $(SAMPLE_OBJS) $(WEBRTC_LIBS) -Wl,--end-group -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lm -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 -lX11 -lXext -lexpat -ldl -lasound -lpulse

all: binding sample node_module

sample:
	g++ $(WEBRTC_LDFLAGS) 

node_module:
	g++ $(DEFS) $(CFLAGS) $(CCFLAGS) $(INCLUDE_DIRS) src/binding.cc -c -o $(BUILD_DIR)/binding.o
	g++ $(NODE_WEBRTC_LDFLAGS)
	node --debug index.js

binding:
	g++ $(DEFS) $(CFLAGS) $(CCFLAGS) $(INCLUDE_DIRS) src/defaults.cc -c -o $(BUILD_DIR)/defaults.o
	g++ $(DEFS) $(CFLAGS) $(CCFLAGS) $(INCLUDE_DIRS) src/peerconnection_client.cc -c -o $(BUILD_DIR)/peerconnection_client.o
	g++ $(DEFS) $(CFLAGS) $(CCFLAGS) $(INCLUDE_DIRS) src/observer.cc -c -o $(BUILD_DIR)/observer.o	
	g++ $(DEFS) $(CFLAGS) $(CCFLAGS) $(INCLUDE_DIRS) src/main.cc -c -o $(BUILD_DIR)/main.o	

webrtc:
	cd $(WEBRTC_ROOT_PATH) && python src/build/merge_libs.py out/Debug/ webrtc-debug-merged.a
	cp $(WEBRTC_ROOT_PATH)/webrtc-debug-merged.a lib/webrtc-debug.a

clean: 
	rm -rf build/*

cleanall: clean
	rm lib/webrtc-debug-merged.a

.PHONY: sample node_module
