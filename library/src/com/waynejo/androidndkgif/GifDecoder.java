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
    private native int nativeGetDelay(long handle, int n);

    private native int nativeGetWidth(long handle);
    private native int nativeGetHeight(long handle);

    private int width = 0;
    private int height = 0;

    private Bitmap[] bitmaps = new Bitmap[0];
    private int[] delays = new int[0];
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
        bitmaps = new Bitmap[frameNum];
        delays = new int[frameNum];
        for (int i = 0; i < frameNum; ++i) {
            bitmaps[i] = nativeGetFrame(handle, i);
            delays[i] = nativeGetDelay(handle, i);
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
        return bitmaps[idx % frameNum];
    }

    public int delay(int idx) {
        if (0 == frameNum) {
            return 0;
        }
        return delays[idx % frameNum];
    }
}
