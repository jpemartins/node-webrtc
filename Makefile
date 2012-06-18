DEFS= -DWEBRTC_SVNREVISION="2272" \
	-DLARGEFILE_SOURCE \
	-D_FILE_OFFSET_BITS=64 \
	-DWEBRTC_TARGET_PC  \
	-DWEBRTC_LINUX  \
	-DWEBRTC_THREAD_RR  \
	-DEXPAT_RELATIVE_PATH  \
	-DGTEST_RELATIVE_PATH \
	-DJSONCPP_RELATIVE_PATH \
	-DWEBRTC_RELATIVE_PATH  \
	-DLINUX  \
	-DPOSIX  \
	-D__STDC_FORMAT_MACROS  \
	-DDYNAMIC_ANNOTATIONS_ENABLED=0  \

# Flags passed to all source files.
#   -fvisibility=hidden NodeJS Does not like it
CFLAGS= -pthread \
	-fno-exceptions \
	-fno-strict-aliasing \
	-Wall \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-Wextra \
	-pipe \
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
	-fno-ident \
	-fdata-sections \
	-ffunction-sections \
	-g

CCFLAGS = -fno-rtti \
	-fno-threadsafe-statics \
	-fvisibility-inlines-hidden \
	-Wsign-compare \
	-D_LARGEFILE_SOURCE \
	-D_FILE_OFFSET_BITS=64

INCLUDE_DIRS = -I/usr/include/nodejs/  \
	-I$(WEBRTC_ROOT_PATH)/third_party/webrtc \
	-I$(WEBRTC_ROOT_PATH)/third_party/ \
	-I$(WEBRTC_ROOT_PATH)/third_party/libjingle/source \
	-I$(WEBRTC_ROOT_PATH)/third_party/libjingle/overrides \
	-I$(WEBRTC_ROOT_PATH)/src \
	-I$(WEBRTC_ROOT_PATH)/src/modules/interface \
	-I$(WEBRTC_ROOT_PATH)/testing/gtest/include

BUILD_DIR = build/
NODE_MODULE = webrtc.node

NODE_FLAGS = -o$(BUILD_DIR)/$(NODE_MODULE)
FLAGS = -obin/sample

NODE_OBJS = $(WEBRTC_OBJS) $(BUILD_DIR)/*.o
SAMPLE_OBJS = $(WEBRTC_OBJS) $(BUILD_DIR)/main.o

WEBRTC_LIB = lib/webrtc.a
WEBRTC_LIB_BUILD = $(WEBRTC_ROOT_PATH)/out/Debug/obj
WEBRTC_LIBS_TRUNK := $(WEBRTC_LIB_BUILD).target/third_party/libjingle/libjingle_peerconnection.a \
		$(WEBRTC_LIB_BUILD).target/third_party/libsrtp/libsrtp.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libvideo_capture_module.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libwebrtc_utility.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libaudio_coding_module.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libCNG.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/common_audio/libsignal_processing.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libG711.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libG722.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libiLBC.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libiSAC.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libiSACFix.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libPCM16B.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libNetEq.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/common_audio/libresampler.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/common_audio/libvad.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/system_wrappers/source/libsystem_wrappers.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libwebrtc_video_coding.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libwebrtc_i420.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libwebrtc_vp8.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/common_video/libwebrtc_libyuv.a \
		$(WEBRTC_LIB_BUILD).target/third_party/libyuv/libyuv.a \
		$(WEBRTC_LIB_BUILD).target/third_party/libjpeg_turbo/libjpeg_turbo.a \
		$(WEBRTC_LIB_BUILD).target/third_party/libvpx/libvpx.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libvideo_render_module.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/video_engine/libvideo_engine_core.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/common_video/libwebrtc_jpeg.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libmedia_file.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/librtp_rtcp.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libudp_transport.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libbitrate_controller.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libvideo_processing.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libvideo_processing_sse2.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/voice_engine/libvoice_engine_core.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libaudio_conference_mixer.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libaudio_processing.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libaec.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libapm_util.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libaec_sse2.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libaecm.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libagc.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libns.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libaudioproc_debug_proto.a \
		$(WEBRTC_LIB_BUILD).target/third_party/protobuf/libprotobuf_lite.a \
		$(WEBRTC_LIB_BUILD).target/third_party/webrtc/modules/libaudio_device.a \
		$(WEBRTC_LIB_BUILD).target/third_party/libjingle/libjingle.a \
		$(WEBRTC_LIB_BUILD).target/third_party/jsoncpp/libjsoncpp.a \
		$(WEBRTC_LIB_BUILD).target/third_party/libjingle/libjingle_p2p.a

NODE_WEBRTC_LDFLAGS = -pthread -Wl,-z,noexecstack -fPIC -L/usr/lib/i386-linux-gnu -m32 -mmmx -march=pentium4 -msse2 -mfpmath=see -O0 $(NODE_FLAGS) -Wl,--start-group $(NODE_OBJS) $(WEBRTC_LIBS_TRUNK) -Wl,--end-group -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lm -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 -lX11 -lXext -lexpat -ldl -lasound -lpulse -shared

WEBRTC_LDFLAGS = -pthread -Wl,-z,noexecstack -fPIC -L/usr/lib/i386-linux-gnu -m32 $(FLAGS) -Wl,--start-group $(SAMPLE_OBJS) $(WEBRTC_LIBS_TRUNK) -Wl,--end-group -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lm -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 -lX11 -lXext -lexpat -ldl -lasound -lpulse

all: node_module

node_module:	
ifndef WEBRTC_ROOT_PATH
	$(error WEBRTC_ROOT_PATH is undefined)
endif
	mkdir -p build
	g++ $(DEFS) $(CFLAGS) $(CCFLAGS) $(INCLUDE_DIRS) src/peerconnection.cc -c -o $(BUILD_DIR)/peerconnection.o	
	g++ $(DEFS) $(CFLAGS) $(CCFLAGS) $(INCLUDE_DIRS) src/gtk_video_renderer.cc -c -o $(BUILD_DIR)/gtk_video_renderer.o	
	g++ $(DEFS) $(CFLAGS) $(CCFLAGS) $(INCLUDE_DIRS) src/binding.cc -c -o $(BUILD_DIR)/binding.o
	g++ $(NODE_WEBRTC_LDFLAGS)
	strip build/webrtc.node
	unlink webrtc.node; ln -s build/webrtc.node webrtc.node

clean: 
	rm -rf build/*

.PHONY: sample node_module
