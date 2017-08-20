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
import java.io.IOException;
import java.io.InputStream;
import java.util.Locale;

@RunWith(AndroidJUnit4.class)
@SmallTest
public class TransparentSampleEncodingTest {

    private final static String TEST_SOURCE_PATH = "transparentSample";
    private final static String TEST_RESULT_PATH = "transparentResult";
    private final static int TEST_IMAGE_WIDTH = 300;
    private final static int TEST_IMAGE_HEIGHT = 300;
    private final static int TEST_IMAGE_NUM = 20;

    private TestUtil testUtil = new TestUtil(TEST_SOURCE_PATH, TEST_RESULT_PATH, TEST_IMAGE_WIDTH, TEST_IMAGE_HEIGHT, TEST_IMAGE_NUM);

    @Test
    public void testEncodingSimpleFastWithDither() {
        testUtil.encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_SIMPLE_FAST, true);
    }

    @Test
    public void testEncodingSimpleFastWithNoDither() {
        testUtil.encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_SIMPLE_FAST, false);
    }

    @Test
    public void testEncodingFastWithDither() {
        testUtil.encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_FAST, true);
    }

    @Test
    public void testEncodingFastWithNoDither() {
        testUtil.encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_FAST, false);
    }

    @Test
    public void testEncodingNormalLowMemoryWithDither() {
        testUtil.encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_NORMAL_LOW_MEMORY, true);
    }

    @Test
    public void testEncodingNormalLowMemoryWithNoDither() {
        testUtil.encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_NORMAL_LOW_MEMORY, false);
    }

    @Test
    public void testEncodingStableHighMemoryWithDither() {
        testUtil.encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_STABLE_HIGH_MEMORY, true);
    }

    @Test
    public void testEncodingStableHighMemoryWithNoDither() {
        testUtil.encodeFileTest(GifEncoder.EncodingType.ENCODING_TYPE_STABLE_HIGH_MEMORY, false);
    }
}