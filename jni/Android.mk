LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := androidndkgif
LOCAL_SRC_FILES := DataBlock.cpp GifDecoder.cpp GifFrame.cpp
include $(BUILD_SHARED_LIBRARY)

