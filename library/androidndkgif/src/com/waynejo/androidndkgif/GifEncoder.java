package com.waynejo.androidndkgif;

import android.graphics.Bitmap;

public class GifEncoder {

    static {
        System.loadLibrary("androidndkgif");
    }

    private native long nativeInit(int width, int height, String path);
    private native void nativeClose(long handle);

    private native boolean nativeEncodeFrame(long handle, Bitmap bitmap, int delayMs);

    private long instance = 0;

    public void init(int width, int height, String path) {
        if (0 != instance) {
            close();
        }
        instance = nativeInit(width, height, path);
    }

    public void close() {
        nativeClose(instance);
        instance = 0;
    }

    public void encodeFrame(Bitmap bitmap, int delayMs) {
        nativeEncodeFrame(instance, bitmap, delayMs);
    }
}
