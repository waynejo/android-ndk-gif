Android NDK GIF Library
========

GIF library built with ndk and gradle in aar format for usage with android gradle build system.

Feature
========

0.2.0
--------

Support Encoding.
Support Decoding.

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
    compile('com.waynejo:androidndkgif:0.1.0')
}
```

Android Java Code (Decoding)
--------

```java
...

GifDecoder gifDecoder = new GifDecoder();
boolean isSucceeded = gifDecoder.load(destFile);
if (isSucceeded) {
    for (int i = 0; i < gifDecoder.frameNum(); ++i) {
        Bitmap bitmap = gifDecoder.frame(i);
    }
}
```

Android Java Code (Encoding)
--------

```java
...

GifEncoder gifEncoder = new GifEncoder();
gifEncoder.init(width, height, filePath);

gifEncoder.encodeFrame(bitmap1, delayMs);
gifEncoder.encodeFrame(bitmap2, delayMs);

gifEncoder.close();
```

# Reference

* GIF Decoder is originally based on https://code.google.com/p/android-gifview/ .
