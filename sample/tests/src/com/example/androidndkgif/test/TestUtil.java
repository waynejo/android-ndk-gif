package com.example.androidndkgif.test;


import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.support.test.InstrumentationRegistry;

import com.waynejo.androidndkgif.GifEncoder;

import org.junit.Assert;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Locale;

public class TestUtil {
    private final String sourceFilePath;
    private final String testResultPath;
    private final int imageWidth;
    private final int imageHeight;
    private final int imageNum;

    public TestUtil(String sourceFilePath, String testResultPath, int imageWidth, int imageHeight, int imageNum) {
        this.sourceFilePath = sourceFilePath;
        this.testResultPath = testResultPath;
        this.imageWidth = imageWidth;
        this.imageHeight = imageHeight;
        this.imageNum = imageNum;
    }

    private static Bitmap getBitmapFromTestAssets(String fileName) throws IOException {
        Context testContext = InstrumentationRegistry.getInstrumentation().getContext();
        AssetManager assetManager = testContext.getAssets();

        InputStream testInput = assetManager.open(fileName);
        return BitmapFactory.decodeStream(testInput);
    }

    private boolean isSameFile(File file, String assetFile) throws IOException {
        Context testContext = InstrumentationRegistry.getInstrumentation().getContext();
        AssetManager assetManager = testContext.getAssets();

        InputStream in1 = new BufferedInputStream(new FileInputStream(file));
        InputStream in2 = new BufferedInputStream(assetManager.open(testResultPath + "/" + assetFile));
        int value1, value2;
        do {
            value1 = in1.read();
            value2 = in2.read();
            if (value1 != value2) {
                return false;
            }
        } while(value1 >= 0);
        return true;
    }

    public void encodeFileTest(GifEncoder.EncodingType encodingType, boolean isDither) {
        try {
            File filesDir = Environment.getExternalStorageDirectory();
            String resultFileName = String.format(Locale.ENGLISH, "result_%s_%d.gif", encodingType.toString(), isDither ? 1 : 0);
            File resultFile = new File(filesDir, resultFileName);
            GifEncoder gifEncoder = new GifEncoder();
            gifEncoder.init(imageWidth, imageHeight, resultFile.getAbsolutePath(), encodingType);
            gifEncoder.setDither(isDither);

            for (int i = 1; i <= imageNum; ++i) {
                gifEncoder.encodeFrame(getBitmapFromTestAssets(String.format(Locale.ENGLISH, sourceFilePath + "/sample_%05d.png", i)), (int) (1.0f / 24.0f * 1000));
            }
            gifEncoder.close();
            Assert.assertEquals("A File not be same", isSameFile(resultFile, resultFileName), true);
        } catch (Exception e) {
            Assert.assertEquals("No Exception Expected.", e, null);
        }
    }
}
