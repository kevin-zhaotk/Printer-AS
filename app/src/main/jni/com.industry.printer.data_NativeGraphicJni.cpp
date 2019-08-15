#include "com.industry.printer.data_NativeGraphicJni.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define PAI 3.14159265358979
#define RED(a) (((a) >> 16) & 0x000000ff)
#define GREEN(a) (((a) >> 8) & 0x000000ff)
#define BLUE(a) (((a) >> 0) & 0x000000ff)
#define ALPHA(a) (((a) >> 24) & 0x000000ff)
#define ARGB(a, r, g, b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define ABS(a) ((a) > 0 ? (a) : -(a))

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    Binarization
 * Signature: ([IIII)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_Binarization
  (JNIEnv *env, jclass thiz, jintArray src, jint width, jint height, jint value) {

	jint *cbuf;
    cbuf = env->GetIntArrayElements(src, 0);

    int colEach = (((height % 8) == 0) ? height/8 : height/8+1);
    int newSize = colEach * width;
    jbyte *rbuf = new jbyte[newSize];

    LOGD("Bitmap Size %d - [%d, %d] - %d - %d", sizeof(cbuf), width, height, width * height, sizeof(int));
    for(int i=0; i<width; i++) {
    	for(int j=0; j<height; j++) {
    	    int curr_color = *(cbuf + j * width + i);
    	    int pixR = RED(curr_color);
    	    int pixG = GREEN(curr_color);
    	    int pixB = BLUE(curr_color);
    	    int pixA = ALPHA(curr_color);
    		int avg = (pixR + pixG + pixB) / 3;
    		int iPixel = 0;
    		iPixel = (avg >= value ? 255 : 0);
    		*(rbuf+j * width + i) = ARGB(pixA, iPixel, iPixel, iPixel);
//    	    LOGD("[%d, %d] - %d", i, j, j * width + i);
    	}
    }

    jbyteArray result = env->NewByteArray(newSize);
    env->SetByteArrayRegion(result, 0, newSize, rbuf);
    env->ReleaseIntArrayElements(src, cbuf, 0);
    env->ReleaseByteArrayElements(result, rbuf, JNI_ABORT);

    return result;
}

#ifdef __cplusplus
}
#endif
