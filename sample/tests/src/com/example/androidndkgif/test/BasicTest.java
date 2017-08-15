package com.example.androidndkgif.test;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;
import android.test.suitebuilder.annotation.SmallTest;

import com.waynejo.androidndkgif.GifEncoder;

import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Locale;

@RunWith(AndroidJUnit4.class)
@SmallTest
public class BasicTest {

    private final static int TEST_IMAGE_WIDTH = 576;
    private final static int TEST_IMAGE_HEIGHT = 240;
    private final static int TEST_IMAGE_NUM = 24;
    private final static String TEST_RESULT_PATH = "result/";

    private static Bitmap getBitmapFromTestAssets(String fileName) throws IOException {
        Context testContext = InstrumentationRegistry.getInstrumentation().getContext();
        AssetManager assetManager = testContext.getAssets();

        InputStream testInput = assetManager.open(fileName);
        return BitmapFactory.decodeStream(testInput);
    }

    private static boolean isSameFile(File file, String assetFile) throws IOException {
        Context testContext = InstrumentationRegistry.getInstrumentation().getContext();
        AssetManager assetManager = testContext.getAssets();

        InputStream in1 = new BufferedInputStream(new FileInputStream(file));
        InputStream in2 = new BufferedInputStream(assetManager.open(TEST_RESULT_PATH + assetFile));
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

    private static void encodeFileTest(GifEncoder.EncodingType encodingType, boolean isDither) {
        try {
            File filesDir = Environment.getExternalStorageDirectory();
            String resultFileName = String.format(Locale.ENGLISH, "result_%s_%d.gif", encodingType.toString(), isDither ? 1 : 0);
            File resultFile = new File(filesDir, resultFileName);
            GifEncoder gifEncoder = new GifEncoder();
            gifEncoder.init(TEST_IMAGE_WIDTH, TEST_IMAGE_HEIGHT, resultFile.getAbsolutePath(), encodingType);
            gifEncoder.setDither(isDither);

            for (int i = 1; i <= TEST_IMAGE_NUM; ++i) {
                gifEncoder.encodeFrame(getBitmapFromTestAssets(String.format(Locale.ENGLISH, "sample/sample_%05d.png", i)), (int) (1.0f / 24.0f * 1000));
            }
            gifEncoder.close();
            Assert.assertEquals("A File not be same", isSameFile(resultFile, resultFileName), true);
        } catch (Exception e) {
            Assert.assertEquals("No Exception Expected.", e, null);
        }
    }

    @Test
    public void testEncodingSimpleFastWithDither() {
        encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_SIMPLE_FAST, true);
    }

    @Test
    public void testEncodingSimpleFastWithNoDither() {
        encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_SIMPLE_FAST, false);
    }

    @Test
    public void testEncodingFastWithDither() {
        encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_FAST, true);
    }

    @Test
    public void testEncodingFastWithNoDither() {
        encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_FAST, false);
    }

    @Test
    public void testEncodingNormalLowMemoryWithDither() {
        encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_NORMAL_LOW_MEMORY, true);
    }

    @Test
    public void testEncodingNormalLowMemoryWithNoDither() {
        encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_NORMAL_LOW_MEMORY, false);
    }

    @Test
    public void testEncodingStableHighMemoryWithDither() {
        encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_STABLE_HIGH_MEMORY, true);
    }

    @Test
    public void testEncodingStableHighMemoryWithNoDither() {
        encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_STABLE_HIGH_MEMORY, false);
    }
}