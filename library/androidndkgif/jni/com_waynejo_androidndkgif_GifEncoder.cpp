#include "com_waynejo_androidndkgif_GifEncoder.h"
#include "GifEncoder.h"
#include <string.h>
#include <wchar.h>
#include <android/bitmap.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_com_waynejo_androidndkgif_GifEncoder_nativeInit
  (JNIEnv *env, jobject, jint width, jint height, jstring path)
{
    GifEncoder* gifEncoder = new GifEncoder();
    const char* pathChars = env->GetStringUTFChars(path, 0);
    gifEncoder->init(width, height, pathChars);
    env->ReleaseStringUTFChars(path, pathChars);
    return (jlong)gifEncoder;
}

JNIEXPORT void JNICALL Java_com_waynejo_androidndkgif_GifEncoder_nativeClose
  (JNIEnv *, jobject, jlong handle)
{
    GifEncoder* gifEncoder = (GifEncoder*)handle;
    gifEncoder->release();
    delete gifEncoder;
}

JNIEXPORT jboolean JNICALL Java_com_waynejo_androidndkgif_GifEncoder_nativeLoad
  (JNIEnv * env, jobject, jlong handle, jobject jBmpObj, jint delayMs)
{
    GifEncoder* gifEncoder = (GifEncoder*)handle;
    void* bitmapPixels;
    if (AndroidBitmap_lockPixels(env, jBmpObj, &bitmapPixels) < 0) {
        return false;
    }
    uint16_t imgWidth = gifEncoder->getWidth();
    uint16_t imgHeight = gifEncoder->getHeight();
    uint32_t* src = (uint32_t*) bitmapPixels;
    uint32_t* tempPixels = new unsigned int[imgWidth * imgHeight];
    int stride = imgWidth * 4;
    int pixelsCount = stride * imgHeight;
    memcpy(tempPixels, bitmapPixels, pixelsCount);
    AndroidBitmap_unlockPixels(env, jBmpObj);
    gifEncoder->encodeFrame(tempPixels, delayMs);
    delete[] tempPixels;
    return true;
}

#ifdef __cplusplus
}
#endif
