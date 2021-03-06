# Mozilla-specific add-on library
MODULE		= libndnrtc
# stand-alone library (not Mozilla-specific)
MODULE_SA	= libndnrtc-sa
NAME 		= ndnrtc

CURRENT     = $(shell pwd)
IDL_PATH 	= $(CURRENT)/idl
SRC_PATH	= $(CURRENT)/src
OBJSRC_PATH	= $(SRC_PATH)/objc
BIN_PATH	= $(CURRENT)/bin
OBJ_PATH	= $(CURRENT)/obj
ADDON_SRC_PATH = $(CURRENT)/addon
ADDON_PATH	= $(BIN_PATH)/$(NAME)
CONTENT_PATH = $(CURRENT)/content
THIRDPARTY_PATH = $(CURRENT)/thirdparty

# third party
NDNC = ndn-c
NDNCPP = ndn-cpp
WEBRTC = ndn-webrtc

NDNCPPLIB = lib$(NDNCPP).a
NDNCLIB = lib$(NDNC).a

LIBBIN_PATH = $(THIRDPARTY_PATH)/bin
NDNCPPLIB_PATH = $(THIRDPARTY_PATH)/$(NDNCPP)

# dir for storing add-on chrome resources
ADDON_CHROME_DIR = $(ADDON_PATH)/chrome
# dir for storing add-on resources, js, etc. files
ADDON_CHROME_CONTENT_DIR = $(ADDON_CHROME_DIR)/content
# dir for storing add-on .xpt and .dylib files
ADDON_COMPONENTS_DIR = $(ADDON_PATH)/components

# Change this to point at your Gecko SDK directory or setup XULDSDK variable
GECKO_SDK_PATH ?= ../../mozilla/obj-xulrunner/dist
PYTHON 			= python
export PYTHONPATH=$(GECKO_SDK_PATH)/sdk/bin


XPIDL_H	    = $(PYTHON) $(GECKO_SDK_PATH)/sdk/bin/header.py
XPIDL_XPT	= $(PYTHON) $(GECKO_SDK_PATH)/sdk/bin/typelib.py
CXX	       	= clang++

XPIDLSRCS	= $(patsubst $(IDL_PATH)/%.idl,%.idl,$(wildcard $(IDL_PATH)/*.idl))
CPPSRCS		= $(patsubst $(SRC_PATH)/%.cpp,%.cpp,$(wildcard $(SRC_PATH)/*.cpp))
CPPSRCS_SA	= $(filter-out ndNrtc%,$(CPPSRCS))
OBJCSRCS    = $(patsubst $(OBJSRC_PATH)/%.mm,%.mm,$(wildcard $(OBJSRC_PATH)/*.mm))

DEBUGFLAGS = -g -DDEBUG -DNDN_TRACE -O0
LOGFLAGS = -DWEBRTC_LOGGING -DNDN_LOGGING -DNDN_TRACE -DNDN_DEBUG -DNDN_INFO -DNDN_WARN -DNDN_ERROR 
CPPFLAGS += -fshort-wchar		\
		-fPIC			\
        --std=c++0x \
        $(DEBUGFLAGS) $(LOGFLAGS) \
		-mmacosx-version-min=10.7 \
		$(NULL)
OBJCFLAGS += -fobjc-link-runtime -framework Cocoa -framework OpenGL -framework CoreVideo -framework QTKit -framework CoreAudio -framework AudioToolbox
LDFLAGS +=		-Wl, -dynamiclib -undefined suppress -flat_namespace

WEBRTC_TRUNK = $(THIRDPARTY_PATH)/$(WEBRTC)/trunk
WEBRTC_SRCPATH = $(WEBRTC_TRUNK)/webrtc

# change it for your WebRTC build directory
WEBRTC_LIBPATH = $(WEBRTC_TRUNK)/xcodebuild/Debug # change to Release for releasing
WEBRTC_INCLUDES = -I$(WEBRTC_SRCPATH) -I$(WEBRTC_TRUNK)

# add necessary libs from WebRTC
# use this command inside WEBRTC_LIBPATH directory to extract the list of the libraries:
# find . -name '*.a' | cut -c6- | cut -d'.' -f1 | awk '{print "\t-l"$0}' | awk '{print $0" \\"}'
WEBRTC_LDFLAGS = -L$(WEBRTC_LIBPATH) \
	-lacm2 \
	-laudio_coding_module \
	-laudio_conference_mixer \
	-laudio_device \
	-laudio_processing \
	-laudio_processing_sse2 \
	-laudioproc_debug_proto \
	-laudioproc_unittest_proto \
	-lbitrate_controller \
	-lchannel_transport \
	-lCNG \
	-lcommand_line_parser \
	-lcommon_audio \
	-lcommon_audio_sse2 \
	-lcommon_video \
	-ldesktop_capture \
	-ldesktop_capture_differ_sse2 \
	-lframe_editing_lib \
	-lframe_generator \
	-lG711 \
	-lG722 \
	-lgenperf_libs \
	-lgflags \
	-lgmock \
	-lgtest \
	-liLBC \
	-liSAC \
	-liSACFix \
	-ljpeg_turbo \
	-lmedia_file \
	-lmetrics \
	-lNetEq \
	-lNetEq4 \
	-lNetEq4TestTools \
	-lneteq_unittest_tools \
	-lNetEqTestTools \
	-lopus \
	-lpaced_sender \
	-lPCM16B \
	-lprotobuf_full_do_not_use \
	-lprotobuf_lite \
	-lrbe_components \
	-lremote_bitrate_estimator \
	-lrtp_rtcp \
	-lsystem_wrappers \
	-ltest \
	-ltest_framework \
	-ltest_support \
	-ltest_support_main \
	-ltest_support_main_threaded_mac \
	-lvideo_capture_module \
	-lvideo_codecs_test_framework \
	-lvideo_coding_utility \
	-lvideo_engine_core \
	-lvideo_processing \
	-lvideo_processing_sse2 \
	-lvideo_quality_analysis \
	-lvideo_render_module \
	-lvideo_tests_common \
	-lvietest \
	-lvoice_engine \
	-lvpx \
	-lvpx_asm_offsets_vp8 \
	-lvpx_intrinsics_mmx \
	-lvpx_intrinsics_sse2 \
	-lvpx_intrinsics_ssse3 \
	-lwebrtc_i420 \
	-lwebrtc_opus \
	-lwebrtc_utility \
	-lwebrtc_video_coding \
	-lwebrtc_vp8 \
	-lyuv 

LDLIBFLAGS = -L$(LIBBIN_PATH) -l$(NDNCPP) -l$(NDNC) $(WEBRTC_LDFLAGS) -lcrypto
LIB_INCLUDES = -I$(LIBBIN_PATH)/include $(WEBRTC_INCLUDES)

# GCC only define which allows us to not have to #include mozilla-config
# in every .cpp file.  If your not using GCC remove this line and add
# #include "mozilla-config.h" to each of your .cpp files.
GECKO_CONFIG_INCLUDE = -include mozilla-config.h

GECKO_DEFINES  = -DMOZILLA_STRICT_API -DXP_UNIX -DXP_MACOSX # -DMOZ_NO_MOZALLOC

GECKO_INCLUDES = -I$(GECKO_SDK_PATH)                    \
                 -I$(GECKO_SDK_PATH)/idl                \
				 -I$(GECKO_SDK_PATH)/sdk/bin			\
                 -I$(GECKO_SDK_PATH)/include			\
				 -I$(GECKO_SDK_PATH)/include/nspr
                 
GECKO_LDFLAGS =  -L$(GECKO_SDK_PATH)/bin \
		 -L$(GECKO_SDK_PATH)/lib \
		 -lxpcomglue_s	\
		 -shared	\
		 $(NULL)

#		 -lxpcomglue_s_nomozalloc \

ALL_CFLAGS = $(CPPFLAGS) $(CXXFLAGS) $(OTHER_CFLAGS) $(GECKO_DEFINES) $(OBJCFLAGS)
ALL_LDFLAGS = $(LDFLAGS) $(LDLIBFLAGS) $(GECKO_LFDFLAGS)
ALL_INCLUDES = $(WEBRTC_INCLUDES) $(GECKO_CONFIG_INCLUDE) $(GECKO_INCLUDES) $(LIB_INCLUDES)

.PHONY: build addon all
.PRECIOUS: $(OBJ_PATH)/%.o $(OBJ_PATH)/%-test.o

all: addon

build: $(MODULE).dylib

libclean:
	cd $(NDNCPPLIB_PATH) && make clean && cd $(CURRENT)

configlib:
	cd $(NDNCPPLIB_PATH) && ./configure && cd $(CURRENT)

libbuild: ndnbuild #webrtcbuild

webrtcbuild:
	export GYP_GENERATORS=make
	cd $(WEBRTC_TRUNK) && gclient sync && make BUILDTYPE=Release

ndnbuild:
	cd $(NDNCPPLIB_PATH) && ./configure --enable-tlv=yes && make  && cd $(CURRENT)
	mkdir -p $(LIBBIN_PATH)/include/$(NDNCPP)
	cp $(NDNCPPLIB_PATH)/.libs/$(NDNCLIB) $(LIBBIN_PATH)
	cp $(NDNCPPLIB_PATH)/.libs/$(NDNCPPLIB) $(LIBBIN_PATH)
	cp -Rf $(NDNCPPLIB_PATH)/include $(LIBBIN_PATH)
	find $(LIBBIN_PATH)/include/$(NDNCPP)/ -name '*.[co]*' | xargs rm
	cp $(NDNCPPLIB_PATH)/config.h $(LIBBIN_PATH)/include/    
	cp -Rf $(NDNCPPLIB_PATH)/include/ndnboost $(LIBBIN_PATH)/include/

# creates a package for add-on in $(BIN_PATH)
addon: $(MODULE).dylib
	mkdir -p $(BIN_PATH)
	mkdir -p $(ADDON_PATH)
	mkdir -p $(ADDON_CHROME_DIR)
	mkdir -p $(ADDON_CHROME_CONTENT_DIR)
	mkdir -p $(ADDON_COMPONENTS_DIR)
	cp $(BIN_PATH)/*.* $(ADDON_COMPONENTS_DIR)
	cp $(CONTENT_PATH)/* $(ADDON_CHROME_CONTENT_DIR) 2>/dev/null || :
	cp -r $(ADDON_SRC_PATH)/* $(ADDON_PATH) 
	cd $(ADDON_PATH) && zip -X -r ../$(NAME).xpi * && cd ../..

# generates header files from .idl
gen_hdrs: $(XPIDLSRCS:%.idl=%.h)

# generates xpt files from .idl 
gen_xpt: $(XPIDLSRCS:%.idl=%.xpt)

%.h: $(IDL_PATH)/%.idl
	$(XPIDL_H) $(GECKO_INCLUDES) $< -o $(SRC_PATH)/$@

%.xpt: $(IDL_PATH)/%.idl
	mkdir -p $(BIN_PATH)
	$(XPIDL_XPT) $(GECKO_INCLUDES) $< -o $(BIN_PATH)/$@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp Makefile
	mkdir -p $(OBJ_PATH)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(OTHER_CFLAGS) $(WEBRTC_INCLUDES) $(GECKO_CONFIG_INCLUDE) $(GECKO_DEFINES) $(GECKO_INCLUDES) $(LIB_INCLUDES) $< -o $@

$(OBJ_PATH)/%.om: $(OBJSRC_PATH)/%.mm Makefile
	mkdir -p $(OBJ_PATH)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(OTHER_CFLAGS) $(WEBRTC_INCLUDES) $(GECKO_CONFIG_INCLUDE) $(GECKO_DEFINES) $(GECKO_INCLUDES) $(LIB_INCLUDES) $< -o $@

#foo: $(OBJCSRCS:%.mm=%.om)
#	echo $(OBJCSRCS)

$(MODULE).dylib: $(XPIDLSRCS:%.idl=%.h) $(XPIDLSRCS:%.idl=%.xpt) $(CPPSRCS:%.cpp=%.o) $(OBJCSRCS:%.mm=%.om)
	mkdir -p $(BIN_PATH)
	$(CXX) -o $(BIN_PATH)/$@ $(LDFLAGS) $(OBJCFLAGS) $(LDLIBFLAGS) $(CPPSRCS:%.cpp=$(OBJ_PATH)/%.o) $(OBJCSRCS:%.mm=$(OBJ_PATH)/%.om) $(GECKO_LDFLAGS)
	chmod +x $(BIN_PATH)/$@

# creating stand-alone NDN-RTC library (not Mozilla-specific add-on library)
undef:
	echo "removing mozilla-specific build flags"
	$(eval GECKO_CONFIG_INCLUDE=)
	$(eval GECKO_DEFINES=)
	$(eval GECKO_INCLUDES=)

undefdebug:
	$(eval DEBUGFLAGS=)

$(MODULE_SA).dylib: $(addprefix $(OBJ_PATH)/,$(CPPSRCS_SA:%.cpp=%.o)) $(addprefix $(OBJ_PATH)/,$(OBJCSRCS:%.mm=%.om))
	mkdir -p $(BIN_PATH)
	$(CXX) -o $(BIN_PATH)/$@ $(LDFLAGS) $(OBJCFLAGS) $(LDLIBFLAGS) $(CPPSRCS_SA:%.cpp=$(OBJ_PATH)/%.o) $(OBJCSRCS:%.mm=$(OBJ_PATH)/%.om)
	chmod +x $(BIN_PATH)/$@

libndnrtc: undef $(MODULE_SA).dylib


##########################################################################################
### TESTING WITH GTEST FRAMEWORK
##########################################################################################
TEST_PATH = $(CURRENT)/test
GTEST_DIR = $(THIRDPARTY_PATH)/ndn-gtest

GT_CXX = $(CXX)
GT_CPPFLAGS += $(CPPFLAGS) $(OTHER_CFLAGS) $(LOG_FLAGS) $(DEBUG_FLAGS)
GT_DEFINES = -DDEBUG -DNDN_LOGGING -DNDN_DETAILED -DNDN_TRACE -DNDN_INFO -DWEBRTC_LOGGING
GT_CXXFLAGS += -g -Wall -Wextra -pthread -Wno-missing-field-initializers $(GT_DEFINES)
GT_GECKO_DEFINES = $(GECKO_DEFINES) -DMOZ_NO_MOZALLOC 
GT_GECKO_LDFLAGS =  -L$(GECKO_SDK_PATH)/bin \
		 -L$(GECKO_SDK_PATH)/lib \
		 -lxpcomglue_s	\
		 $(NULL)
GT_LDFLAGS += -lpthread -Wl, -flat_namespace -undefined suppress # $(GT_GECKO_LDFLAGS)

TESTS = $(patsubst $(TEST_PATH)/%-test.cc,%-test,$(wildcard $(TEST_PATH)/*-test.cc))
CPPSRCS_TEST = $(CPPSRCS_SA)
#GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
#                $(GTEST_DIR)/include/gtest/internal/*.h

testall : $(TESTS:%=build-%)

${TESTS} : ; $(BIN_PATH)/$@

testrun : libndnrtc testall ${TESTS}
	
	
# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

$(OBJ_PATH)/libgtest-all.a : $(GTEST_SRCS_)
	$(GT_CXX) -I$(GTEST_DIR)/include -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) $(GT_CXXFLAGS) $(GT_LDFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -static -o $@
            
$(OBJ_PATH)/libtest-main.a : $(GTEST_SRCS_)
	$(GT_CXX) $(GT_CPPFLAGS) -I$(GTEST_DIR)/include -I$(GTEST_DIR) $(GT_CXXFLAGS) $(GT_LDFLAGS) -c \
            $(TEST_PATH)/test-main.cc -static -o $@
            
$(OBJ_PATH)/libtest-main-mac.a : $(GTEST_SRCS_) $(TEST_PATH)/test-main-mac.mm
	$(GT_CXX) $(GT_CPPFLAGS) -I$(GTEST_DIR)/include -I$(GTEST_DIR) -I$(SRC_PATH) $(LIB_INCLUDES) $(GT_CXXFLAGS) $(OBJCFLAGS) $(GT_LDFLAGS) -c \
            $(TEST_PATH)/test-main-mac.mm -static -o $@

$(OBJ_PATH)/test-common.o : $(GTEST_SRCS_)
	$(GT_CXX) $(GT_CPPFLAGS) $(OTHER_CFLAGS) $(GT_LDFLAGS) -Wno-unused-parameter -I$(GTEST_DIR)/include -I$(GTEST_DIR) -I$(SRC_PATH) $(LIB_INCLUDES) $(GT_CXXFLAGS) -c \
		$(TEST_PATH)/test-common.mm -o $@
            
$(OBJ_PATH)/%-test.o : $(TEST_PATH)/%-test.cc
	$(GT_CXX) --std=c++0x $(CPPFLAGS) $(OTHER_CFLAGS) $(GT_LDFLAGS) -I$(GTEST_DIR)/include -I$(SRC_PATH) $(LIB_INCLUDES) -c $< -o $@

%-test.om : $(TEST_PATH)/%-test.mm
	$(GT_CXX) --std=c++0x $(OTHER_CFLAGS) $(GT_LDFLAGS) -Wno-return-type -I$(GTEST_DIR)/include -I$(SRC_PATH) $(LIB_INCLUDES) $(OBJCFLAGS) -c $< -o $(OBJ_PATH)/$@

###
# TESTS have automatic targets starting with build-...
###
TESTOBJ = $(addprefix $(OBJ_PATH)/,$(filter-out nsthread-tasks.o ndNrtcModule.o ndNrtcObject.o ndnrtc-library.o ndnworker.o,$(CPPSRCS:%.cpp=%.o)))

build-%: undef $(OBJ_PATH)/%.o $(TESTOBJ) $(addprefix $(OBJ_PATH)/,test-common.o cocoa-renderer.om libgtest-all.a libtest-main-mac.a)
	mkdir -p $(BIN_PATH)
	$(GT_CXX) -g --std=c++0x \
	$(TESTOBJ) \
	$(OBJ_PATH)/$(patsubst build-%,%.o,$@) \
	$(addprefix $(OBJ_PATH)/,test-common.o cocoa-renderer.om) \
	-L$(OBJ_PATH) -lgtest-all -ltest-main-mac $(LDLIBFLAGS) $(OBJCFLAGS) $(GT_LDFLAGS)\
	-o $(BIN_PATH)/$(patsubst build-%,%,$@)

clean: libclean
	rm -Rf $(wildcard $(BIN_PATH))
	rm -Rf $(wildcard $(OBJ_PATH))
#	rm -Rf $(wildcard $(LIBBIN_PATH))