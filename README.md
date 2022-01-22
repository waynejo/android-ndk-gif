Android NDK GIF Library
========

GIF library built with ndk and gradle in aar format for usage with android gradle build system.

1.0.1 Change Log
========
* Added Gif Image Iterator for Image decoding.
* Fixed a problem of residual image on transparent background.

Encoding Option
========
 - ENCODING_TYPE_SIMPLE_FAST 
   - use low memory and encode fast. But low quality.
   - ![ENCODING_TYPE_SIMPLE_FAST](https://github.com/waynejo/android-ndk-gif/blob/master/sample/tests/assets/result/result_ENCODING_TYPE_SIMPLE_FAST_1.gif)
 - ENCODING_TYPE_FAST (Thanks to commitor "JBurkeKF")
   - encode fast. And better quality.
   - ![ENCODING_TYPE_SIMPLE_FAST](https://github.com/waynejo/android-ndk-gif/blob/master/sample/tests/assets/result/result_ENCODING_TYPE_FAST_1.gif)
 - ENCODING_TYPE_NORMAL_LOW_MEMORY 
   - use lower memory than ENCODING_TYPE_STABLE_HIGH_MEMORY. and image changing dynamic.
   - ![ENCODING_TYPE_NORMAL_LOW_MEMORY](https://github.com/waynejo/android-ndk-gif/blob/master/sample/tests/assets/result/result_ENCODING_TYPE_NORMAL_LOW_MEMORY_1.gif)
 - ENCODING_TYPE_STABLE_HIGH_MEMORY 
   - slowest and use high memory. But high quality and stable image sequence.
   - ![ENCODING_TYPE_STABLE_HIGH_MEMORY](https://github.com/waynejo/android-ndk-gif/blob/master/sample/tests/assets/result/result_ENCODING_TYPE_STABLE_HIGH_MEMORY_1.gif)

How to use
========

Remote repository
--------

```groovy
...

repositories {
    maven { url 'https://repo1.maven.org/maven2' }
    ...
}

dependencies {
    implementation ('io.github.waynejo:androidndkgif:1.0.1')
}
```

Decoding using Iterator. (Less memory used)
--------
```java

GifDecoder gifDecoder = new GifDecoder();
final GifImageIterator iterator = gifDecoder.loadUsingIterator(destFile);
while (iterator.hasNext()) {
    GifImage next = iterator.next();
    if (null != next) {
        imageView.setImageBitmap(next.bitmap);
    }
}
iterator.close();
```

Decoding
--------
```java

GifDecoder gifDecoder = new GifDecoder();
boolean isSucceeded = gifDecoder.load(destFile);
if (isSucceeded) {
    for (int i = 0; i < gifDecoder.frameNum(); ++i) {
        Bitmap bitmap = gifDecoder.frame(i);
    }
}
```

Encoding
--------

```java

GifEncoder gifEncoder = new GifEncoder();
gifEncoder.init(width, height, filePath, GifEncoder.EncodingType.ENCODING_TYPE_NORMAL_LOW_MEMORY);

// Bitmap is MUST ARGB_8888.
gifEncoder.encodeFrame(bitmap1, delayMs);
gifEncoder.encodeFrame(bitmap2, delayMs);

gifEncoder.close();
```

# Reference

* GIF Decoder is originally based on https://code.google.com/p/android-gifview/ .

---------
[![Android Arsenal](https://img.shields.io/badge/Android%20Arsenal-android--ndk--gif-green.svg?style=true)](https://android-arsenal.com/details/1/3585)
