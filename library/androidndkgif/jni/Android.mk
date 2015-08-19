LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := androidndkgif
LOCAL_SRC_FILES := DataBlock.cpp GifDecoder.cpp GifFrame.cpp com_waynejo_androidndkgif_GifDecoder.cpp
LOCAL_LDFLAGS += -ljnigraphics
include $(BUILD_SHARED_LIBRARY)

