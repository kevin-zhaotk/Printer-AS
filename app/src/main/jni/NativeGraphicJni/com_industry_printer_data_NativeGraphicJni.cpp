#include "com_industry_printer_data_NativeGraphicJni.h"
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

static jint *DOTS = new jint[8];

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    ShiftImage
 * Signature: ([IIIIII)[I
 */
JNIEXPORT jintArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_ShiftImage
  (JNIEnv *env, jclass thiz, jintArray src, jint width, jint height, jint head, jint orgLines, jint tarLines) {

    LOGD("ShiftImage: [%d, %d], head=%d, orgLines=%d, tarLines=%d", width, height, head, orgLines, tarLines);

    jint *cbuf;
    cbuf = env->GetIntArrayElements(src, 0);

    jsize length = width * height;
    jint *rbuf = new jint[length];
    memset(rbuf, 0xff, length * sizeof(jint));

    for(int i=head-1; i>=0; i--) {
        memcpy(rbuf + i * tarLines * width, cbuf + i * orgLines * width, orgLines * width * sizeof(jint));
    }

    jintArray result = env->NewIntArray(length);
    env->SetIntArrayRegion(result, 0, length, rbuf);
    env->ReleaseIntArrayElements(src, cbuf, 0);
    env->ReleaseIntArrayElements(result, rbuf, JNI_ABORT);

    return result;
}

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    Binarize
 * Signature: ([IIIII)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_Binarize
        (JNIEnv *env, jclass thiz, jintArray src, jint width, jint height, int head, jint value) {

    LOGD("Binarize: [%d, %d], head=%d, value=%d", width, height, head, value);

    jint *cbuf;
    cbuf = env->GetIntArrayElements(src, 0);

    int colEach = (((height % 8) == 0) ? height/8 : height/8+1);
    int newSize = colEach * width;
    jbyte *rbuf = new jbyte[newSize];

    int heighEachHead = height / head;

    memset(DOTS, 0x00, 8 * sizeof(jint));

    for(int i=0; i<width; i++) {
        for(int j=0; j<height; j++) {
            int curr_color = *(cbuf + j * width + i);
            int pixR = RED(curr_color);
            int pixG = GREEN(curr_color);
            int pixB = BLUE(curr_color);
//    	    int pixA = ALPHA(curr_color);

            int grey = (int)((float) pixR * 0.3 + (float)pixG * 0.59 + (float)pixB * 0.11);

            if(grey > value)
                rbuf[i*colEach + j/8] &= ~( 0x01 << (j%8));
            else {
                rbuf[i*colEach + j/8] |= (0x01 << (j%8));
                DOTS[(int)(j / heighEachHead)]++;
            }
        }
    }

    jbyteArray result = env->NewByteArray(newSize);
    env->SetByteArrayRegion(result, 0, newSize, rbuf);
    env->ReleaseIntArrayElements(src, cbuf, 0);
    env->ReleaseByteArrayElements(result, rbuf, JNI_ABORT);

    return result;
}

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    GetDots
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_GetDots
  (JNIEnv *env, jclass thiz) {

    LOGD("GetDots");

    jintArray result = env->NewIntArray(8);
    env->SetIntArrayRegion(result, 0, 8, DOTS);

    return result;
}

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    GetBgBuffer
 * Parameters:
 *      src:        原始位图数据缓冲区，一个bit代表一个dot;
 *      length:     缓冲区总长度;
 *      bytesFeed:  目标缓冲区每列的数据长度（因为有不同打印头之间的缝隙，要比实际数据长）
 *      bytesPerHFeed：目标缓冲区每个打印头的数据长度
 *      bytesPerH： 原始缓冲区每个头侧数据长度
 *      column：    数据总列数
 *      type：      打印头的数量
 * Signature: ([BIIIIII})[C
 */
JNIEXPORT jcharArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_GetBgBuffer
        (JNIEnv *env, jclass thiz, jbyteArray src, jint length, jint bytesFeed, jint bytesPerHFeed, jint bytesPerH, jint column, jint type) {

    LOGD("GetBgBuffer length=%d, bytesFeed=%d, bytesPerHFeed=%d, bytesPerH=%d, column=%d, type=%d", length, bytesFeed, bytesPerHFeed, bytesPerH, column, type);

    jbyte *cbuf;
    cbuf = env->GetByteArrayElements(src, 0);

    jbyte *rByteBuf = NULL;
    jchar *rCharBuf = NULL;

    // 当每个头需要的DOT数据字节数大于每个头实际拥有的DOT字节数（12.7xn喷头时，每个头的实际数据为152点，19个字节，但打印缓冲区每个头必须为20字节，因此会出现这个需要补齐的情况）
    if(bytesPerHFeed > bytesPerH) {
        rByteBuf = new jbyte[length];
        memset(rByteBuf, 0x00, length);

        size_t orgPointer = 0;
        for(int i=0; i < column; i++) {
//        LOGD("GetBgBuffer Column = %d", column);
            for (int j = 0; j < type; j++) {
//            LOGD("GetBgBuffer Type = %d", type);
                jint pos = i * bytesFeed + j * bytesPerHFeed;
                memcpy(rByteBuf + pos, cbuf + orgPointer, bytesPerH);
                orgPointer += bytesPerH;
            }
        }
    // 当每个头需要的DOT数据字节数等于每个头实际拥有的DOT字节数的时候，无需移位操作
    } else if(bytesPerHFeed == bytesPerH) {
        rByteBuf = cbuf;
    }

    if(NULL != rByteBuf) {
        rCharBuf = new jchar[length/2];
        for(int i=0; i<length/2; i++) {
            rCharBuf[i] = (jchar) (((jchar)(rByteBuf[2*i+1] << 8) & 0x0ff00) | (rByteBuf[2*i] & 0x0ff));
        }
        if(rByteBuf != cbuf) {
            delete[] rByteBuf;
        }
    }

    jcharArray result = NULL;
    env->ReleaseByteArrayElements(src, cbuf, 0);
    if(NULL != rCharBuf) {
        result = env->NewCharArray(length/2);
        env->SetCharArrayRegion(result, 0, length/2, rCharBuf);
        env->ReleaseCharArrayElements(result, rCharBuf, JNI_ABORT);
    }

    return result;
}

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    GetPrintDots
 * Parameters:
 *      src:                原始位图数据缓冲区，一个bit代表一个dot;
 *      bytesPerHFeed：     目标缓冲区每个打印头的数据长度
 *      heads：             打印头的数量
 * Signature: ([CIII})[I
 */
JNIEXPORT jintArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_GetPrintDots
        (JNIEnv *env, jclass thiz, jcharArray src, jint length, jint charsPerHFeed, jint heads) {

    jchar *srcBuf = env->GetCharArrayElements(src, 0);

    LOGD("GetPrintDots length=%d, charsPerHFeed=%d, heads=%d", length, charsPerHFeed, heads);

    jint *dots = new jint[8];
    memset(dots, 0x00, 8 * sizeof(jint));

    int headIndex = -1;      // 当前数据所属打印头。初值为-1，为进入循环处理做准备
    for(int i=0; i<length; i++) {
        if((i % charsPerHFeed) == 0) {
            headIndex++;
            headIndex %= heads;
        }

        if( (srcBuf[i] & 0x0001) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x0002) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x0004) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x0008) != 0x0000) {
            dots[headIndex]++;
        }

        if( (srcBuf[i] & 0x0010) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x0020) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x0040) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x0080) != 0x0000) {
            dots[headIndex]++;
        }

        if( (srcBuf[i] & 0x0100) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x0200) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x0400) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x0800) != 0x0000) {
            dots[headIndex]++;
        }

        if( (srcBuf[i] & 0x1000) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x2000) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x4000) != 0x0000) {
            dots[headIndex]++;
        }
        if( (srcBuf[i] & 0x8000) != 0x0000) {
            dots[headIndex]++;
        }
    }

    env->ReleaseCharArrayElements(src, srcBuf, 0);

    jintArray result = env->NewIntArray(8);
    env->SetIntArrayRegion(result, 0, 8, dots);

    return result;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){
    LOGI("NativeGraphicJni.so 1.0.2 Loaded.");
    return JNI_VERSION_1_4;     //这里很重要，必须返回版本，否则加载会失败。
}

#ifdef __cplusplus
}
#endif
