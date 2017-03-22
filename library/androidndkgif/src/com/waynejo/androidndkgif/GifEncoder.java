package com.waynejo.androidndkgif;

import android.graphics.Bitmap;

import java.io.FileNotFoundException;

public class GifEncoder {

    static {
        System.loadLibrary("androidndkgif");
    }

    public enum EncodingType {
        ENCODING_TYPE_SIMPLE_FAST,
        ENCODING_TYPE_FAST,
        ENCODING_TYPE_NORMAL_LOW_MEMORY,
        ENCODING_TYPE_STABLE_HIGH_MEMORY
    }

    private native long nativeInit(int width, int height, String path, int encodingType, int threadCount);
    private native void nativeClose(long handle);
    private native void nativeSetDither(long handle, boolean useDither);
    private native void nativeSetThreadCount(long handle, int threadCount);

    private native boolean nativeEncodeFrame(long handle, Bitmap bitmap, int delayMs);

    private long instance = 0;
    private int threadCount = 1;

    public void init(int width, int height, String path) throws FileNotFoundException {
        init(width, height, path, EncodingType.ENCODING_TYPE_NORMAL_LOW_MEMORY);
    }

    public void init(int width, int height, String path, EncodingType encodingType) throws FileNotFoundException {
        if (0 != instance) {
            close();
        }
        instance = nativeInit(width, height, path, encodingType.ordinal(), threadCount);
        if (0 == instance) {
            throw new FileNotFoundException();
        }
    }

    public void close() {
        nativeClose(instance);
        instance = 0;
    }

    public void setDither(boolean useDither) {
        if (0 == instance) {
            return ;
        }
        nativeSetDither(instance, useDither);
    }

    public void setThreadCount(int threadCount) {
        this.threadCount = threadCount;
        if (0 == instance) {
            return;
        }
        nativeSetThreadCount(instance, threadCount);
    }

    public boolean encodeFrame(Bitmap bitmap, int delayMs) {
        if (0 == instance) {
            return false;
        }
        nativeEncodeFrame(instance, bitmap, delayMs);
        return true;
    }
}
