package com.waynejo.androidndkgif;

import android.graphics.Bitmap;

import java.io.FileNotFoundException;

public class GifEncoder {

    static {
        System.loadLibrary("androidndkgif");
    }

    private native long nativeInit(int width, int height, String path);
    private native void nativeClose(long handle);
    private native void nativeSetDither(long handle, boolean useDither);

    private native boolean nativeEncodeFrame(long handle, Bitmap bitmap, int delayMs);

    private long instance = 0;

    public void init(int width, int height, String path) throws FileNotFoundException {
        if (0 != instance) {
            close();
        }
        instance = nativeInit(width, height, path);
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

    public boolean encodeFrame(Bitmap bitmap, int delayMs) {
        if (0 == instance) {
            return false;
        }
        nativeEncodeFrame(instance, bitmap, delayMs);
        return true;
    }
}
