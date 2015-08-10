package com.waynejo.androidndkgif;

import android.graphics.Bitmap;

public class GifDecoder {
    private native long nativeInit();
    private native void nativeClose(long handle);

    private native boolean nativeLoad(long handle, String fileName);

    private native int nativeGetFrameCount(long handle);

    private native Bitmap nativeGetFrame(long handle, int n);

    private native int nativeGetWidth(long handle);
    private native int nativeGetHeight(long handle);

    private int width = 0;
    private int height = 0;

    public boolean load(String fileName) {
        long handle = nativeInit();
        if (!nativeLoad(handle, fileName)) {
            nativeClose(handle);
            return false;
        }
        width = nativeGetWidth(handle);
        height = nativeGetHeight(handle);

        nativeClose(handle);
        return true;
    }

    public int width() {
        return width;
    }

    public int height() {
        return height;
    }
}
