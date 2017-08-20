package com.example.androidndkgif;

import android.app.Activity;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;
import com.waynejo.androidndkgif.GifDecoder;
import com.waynejo.androidndkgif.GifEncoder;
import com.waynejo.androidndkgif.GifImage;
import com.waynejo.androidndkgif.GifImageIterator;

import java.io.*;

public class ExampleActivity extends Activity {

    private boolean useDither = true;
    private ImageView imageView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.main);

        imageView = (ImageView) findViewById(R.id.image_view);
    }

    private String setupSampleFile() {
        AssetManager assetManager = getAssets();
        String srcFile = "sample1.gif";
        String destFile = getFilesDir().getAbsolutePath() + File.separator + srcFile;
        copyFile(assetManager, srcFile, destFile);
        return destFile;
    }

    private void copyFile(AssetManager assetManager, String srcFile, String destFile) {
        try {
            InputStream is = assetManager.open(srcFile);
            FileOutputStream os = new FileOutputStream(destFile);

            byte[] buffer = new byte[1024];
            int read;
            while ((read = is.read(buffer)) != -1) {
                os.write(buffer, 0, read);
            }
            is.close();
            os.flush();
            os.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void onDecodeGIF(View v) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                String destFile = setupSampleFile();

                final GifDecoder gifDecoder = new GifDecoder();
                final boolean isSucceeded = gifDecoder.load(destFile);
                runOnUiThread(new Runnable() {
                    int idx = 0;
                    @Override
                    public void run() {
                        if (isSucceeded) {
                            Bitmap bitmap = gifDecoder.frame(idx);
                            imageView.setImageBitmap(bitmap);
                            if (idx + 1 < gifDecoder.frameNum()) {
                                imageView.postDelayed(this, gifDecoder.delay(idx));
                            }
                            ++idx;
                        } else {
                            Toast.makeText(ExampleActivity.this, "Failed", Toast.LENGTH_SHORT).show();
                        }
                    }
                });
            }
        }).start();
    }

    public void onDecodeGIFUsingIterator(View v) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                String destFile = setupSampleFile();

                final GifDecoder gifDecoder = new GifDecoder();
                final GifImageIterator iterator = gifDecoder.loadUsingIterator(destFile);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (iterator.hasNext()) {
                            GifImage next = iterator.next();
                            if (null != next) {
                                imageView.setImageBitmap(next.bitmap);
                                imageView.postDelayed(this, next.delayMs);
                            } else {
                                Toast.makeText(ExampleActivity.this, "Failed", Toast.LENGTH_SHORT).show();
                            }
                        } else {
                            iterator.close();
                        }
                    }
                });
            }
        }).start();
    }

    public void onEncodeGIF(View v) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    encodeGIF();
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    private void encodeGIF() throws IOException {
        String dstFile = "result.gif";
        final String filePath = Environment.getExternalStorageDirectory() + File.separator + dstFile;
        int width = 50;
        int height = 50;
        int delayMs = 100;

        GifEncoder gifEncoder = new GifEncoder();
        gifEncoder.init(width, height, filePath, GifEncoder.EncodingType.ENCODING_TYPE_NORMAL_LOW_MEMORY);
        gifEncoder.setDither(useDither);
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        Paint p = new Paint();
        int[] colors = new int[] {0xFFFF0000, 0xFFFFFF00, 0xFFFFFFFF};
        for (int color : colors) {
            p.setColor(color);
            canvas.drawRect(0, 0, width, height, p);
            gifEncoder.encodeFrame(bitmap, delayMs);
        }
        gifEncoder.close();

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(ExampleActivity.this, "done : " + filePath, Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void onDisableDithering(View v) {
        useDither = false;
    }
}
