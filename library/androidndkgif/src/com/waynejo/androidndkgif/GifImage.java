package com.waynejo.androidndkgif;

import android.graphics.Bitmap;

public class GifImage {
    public final Bitmap bitmap;
    public final int delayMs;

    public GifImage(Bitmap bitmap, int delayMs) {
        this.bitmap = bitmap;
        this.delayMs = delayMs;
    }
}
