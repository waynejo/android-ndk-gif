Android NDK GIF Library
========

GIF library built with ndk and gradle in aar format for usage with android gradle build system.

Feature
========

0.1.0
--------

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

Android Java Code
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

# Reference

* GIF Decoder is originally based on https://code.google.com/p/android-gifview/ .
