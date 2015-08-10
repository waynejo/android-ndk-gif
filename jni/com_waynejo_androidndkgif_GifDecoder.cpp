#include "com_waynejo_androidndkgif_GifDecoder.h"
#include "GifDecoder.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_com_waynejo_androidndkgif_GifDecoder_nativeInit
  (JNIEnv *, jobject)
{
    return (jlong)new GifDecoder();
}

JNIEXPORT void JNICALL Java_com_waynejo_androidndkgif_GifDecoder_nativeClose
  (JNIEnv *, jobject, jlong handle)
{
    delete (GifDecoder*)handle;
}

JNIEXPORT jboolean JNICALL Java_com_waynejo_androidndkgif_GifDecoder_nativeLoad
  (JNIEnv *, jobject, jlong handle, jstring fileName)
{
    return ((GifDecoder*)handle)->load("");
}

JNIEXPORT jint JNICALL Java_com_waynejo_androidndkgif_GifDecoder_nativeGetFrameCount
  (JNIEnv *, jobject, jlong handle)
{
    return ((GifDecoder*)handle)->getFrameCount();
}

JNIEXPORT jobject JNICALL Java_com_waynejo_androidndkgif_GifDecoder_nativeGetFrame
  (JNIEnv *, jobject, jlong handle, jint)
{
    return 0;
}

JNIEXPORT jint JNICALL Java_com_waynejo_androidndkgif_GifDecoder_nativeGetWidth
  (JNIEnv *, jobject, jlong handle)
{
    return ((GifDecoder*)handle)->getWidth();
}

JNIEXPORT jint JNICALL Java_com_waynejo_androidndkgif_GifDecoder_nativeGetHeight
  (JNIEnv *, jobject, jlong handle)
{
    return ((GifDecoder*)handle)->getHeight();
}

#ifdef __cplusplus
}
#endif
