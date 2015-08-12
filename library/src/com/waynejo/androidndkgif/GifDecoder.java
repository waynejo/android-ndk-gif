package com.waynejo.androidndkgif;

import android.graphics.Bitmap;

import java.util.ArrayList;

public class GifDecoder {

    static {
        System.loadLibrary("androidndkgif");
    }

    private native long nativeInit();
    private native void nativeClose(long handle);

    private native boolean nativeLoad(long handle, String fileName);

    private native int nativeGetFrameCount(long handle);

    private native Bitmap nativeGetFrame(long handle, int n);

    private native int nativeGetWidth(long handle);
    private native int nativeGetHeight(long handle);

    private int width = 0;
    private int height = 0;

    private ArrayList<Bitmap> bitmaps = new ArrayList<Bitmap>();
    private int frameNum;

    public boolean load(String fileName) {
        long handle = nativeInit();
        if (!nativeLoad(handle, fileName)) {
            nativeClose(handle);
            return false;
        }
        width = nativeGetWidth(handle);
        height = nativeGetHeight(handle);

        frameNum = nativeGetFrameCount(handle);
        for (int i = 0; i < frameNum; ++i) {
            bitmaps.add(nativeGetFrame(handle, i));
        }

        nativeClose(handle);
        return true;
    }

    public int width() {
        return width;
    }

    public int height() {
        return height;
    }

    public int frameNum() {
        return frameNum;
    }

    public Bitmap frame(int idx) {
        if (0 == frameNum) {
            return null;
        }
        return bitmaps.get(idx % frameNum);
    }
}
