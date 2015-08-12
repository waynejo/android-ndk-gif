package com.example.androidndkgif;

import android.app.Activity;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.widget.ImageView;
import android.widget.Toast;
import com.waynejo.androidndkgif.GifDecoder;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class ExampleActivity extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.main);

        final ImageView imageView = (ImageView) findViewById(R.id.image_view);

        new Thread(new Runnable() {
            @Override
            public void run() {
                String destFile = setupSampleFile();

                final GifDecoder gifDecoder = new GifDecoder();
                final boolean isSucceeded = gifDecoder.load(destFile);
                new Handler(Looper.getMainLooper()).post(new Runnable() {
                    @Override
                    public void run() {
                        if (isSucceeded) {
                            Bitmap bitmap = gifDecoder.frame(1);
                            imageView.setImageBitmap(bitmap);
                        } else {
                            Toast.makeText(ExampleActivity.this, "Failed", Toast.LENGTH_SHORT).show();
                        }
                    }
                });
            }
        }).start();
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
}
