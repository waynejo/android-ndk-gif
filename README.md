Android NDK GIF Library
========

GIF library built with ndk and gradle in aar format for usage with android gradle build system.

0.3.2 Change Log
========
* fix some memory issues.

Encoding Option
========
* Add some encoding options.
 - ENCODING_TYPE_SIMPLE_FAST 
   - use low memory and encode fast. But low quality.
 - ENCODING_TYPE_NORMAL_LOW_MEMORY 
   - use lower memory than ENCODING_TYPE_STABLE_HIGH_MEMORY. and image changing dynamic.
 - ENCODING_TYPE_STABLE_HIGH_MEMORY 
   - slowest and use high memory. But high quality and stable image sequence.

Feature
========
* GIF Encoding.
* GIF Decoding.


How to use
========

Remote repository
--------

```groovy
...

repositories {
    maven { url "http://dl.bintray.com/waynejo/maven" }
    ...
}

dependencies {
    compile('com.waynejo:androidndkgif:0.3.2')
}
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
