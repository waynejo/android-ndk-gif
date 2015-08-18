Android NDK GIF Library
========

GIF library built with ndk and gradle in aar format for usage with android gradle build system.

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

# Reference

* GIF Decoder is originally based on https://code.google.com/p/android-gifview/ .
