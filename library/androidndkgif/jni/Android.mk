LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := androidndkgif
LOCAL_SRC_FILES := DataBlock.cpp BitmapIterator.cpp BitWritingBlock.cpp GifDecoder.cpp GifEncoder.cpp GifFrame.cpp com_waynejo_androidndkgif_GifDecoder.cpp com_waynejo_androidndkgif_GifEncoder.cpp GCTGifEncoder.cpp LCTGifEncoder.cpp FastGifEncoder.cpp SimpleGCTEncoder.cpp BaseGifEncoder.cpp
LOCAL_LDFLAGS += -ljnigraphics
LOCAL_CFLAGS := -D__GXX_EXPERIMENTAL_CXX0X__
LOCAL_CPPFLAGS  := -std=c++11
include $(BUILD_SHARED_LIBRARY)
