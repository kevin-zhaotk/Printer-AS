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

    for(int k=0; k<8; k++) {
        DOTS[k] = 0;
    }

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
 *      isInchType：是否为25.4种类的打印头
 *      column：    数据总列数
 *      type：      打印头的数量
 * Signature: ([{IIIIZII})[C
 */
/* 这个获取背景缓冲区的实现方法潜入了对25.4xn打印头进行纵向移位（每头308点纵向紧凑排法变成320点靠上排列308点，剩余12点镂空的排法），由于该实现方法转移到了生成bin文件时实现，因此该实现方法取消
JNIEXPORT jcharArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_GetBgBuffer
  (JNIEnv *env, jclass thiz, jbyteArray src, jint length, jint bytesFeed, jint bytesPerHFeed, jint bytesPerH, jboolean isInchType, jint column, jint type) {

    LOGD("GetBgBuffer length=%d, bytesFeed=%d, bytesPerHFeed=%d, bytesPerH=%d, isInchType=%d, column=%d, type=%d", length, bytesFeed, bytesPerHFeed, bytesPerH, isInchType, column, type);

    jbyte *cbuf;
    cbuf = env->GetByteArrayElements(src, 0);

    jbyte *rByteBuf = new jbyte[length];
    jchar *rCharBuf = new jchar[length/2];

    size_t orgPointer = 0;

    if( isInchType ) {
        bytesPerH -= 2;
    }

    // 从当前位置读取mBytesPerH个字节到背景buffer中，由于需要处理多头情况，所以要注意在每个头的列尾要注意补偿问题（双字节对齐
    for(int i=0; i < column; i++) {
//        LOGD("GetBgBuffer Column = %d", column);
        jbyte tempByte = 0x00, bufferByte = 0x00;
        int skipBytes = 0;

        for (int j = 0; j < type; j++) {
//            LOGD("GetBgBuffer Type = %d", type);
            jint pos = i*bytesFeed + j*bytesPerHFeed;

            if( isInchType) {
                if(j%2 == 0) {
                    memcpy(rByteBuf + pos, cbuf + orgPointer, bytesPerH + 1);
                    orgPointer += (bytesPerH + 1);

                    tempByte = rByteBuf[pos + bytesPerH];
                    rByteBuf[pos + bytesPerH] &= 0x0f;
                    tempByte >>= 4;
                    tempByte &= 0x0f;
                    skipBytes += 1;
                } else {
                    memcpy(rByteBuf + pos, cbuf + orgPointer, bytesPerH);
                    orgPointer += bytesPerH;

                    for(int k=0; k<bytesPerH; k++) {
                        bufferByte = rByteBuf[pos + k];
                        tempByte |= (jbyte)((bufferByte << 4) & 0xf0);
                        rByteBuf[pos + k] = tempByte;
                        tempByte = (jbyte)((bufferByte >> 4) & 0x0f);
                    }
                    rByteBuf[pos + bytesPerH] = tempByte;
                    skipBytes += 2;
                }
            } else {
                memcpy(rByteBuf + pos, cbuf + orgPointer, bytesPerH);
                orgPointer += bytesPerH;
            }
        }
        orgPointer += skipBytes;
    }
    //mFStream.close();

    // 如果是奇数列在每列最后添加一个byte

    //把byte[]存为char[]
//    LOGD("GetBgBuffer Byte => Char");
    for(int i = 0; i < length/2; i++) {
        rCharBuf[i] = (jchar) (((jchar)(rByteBuf[2*i+1] << 8) & 0x0ff00) | (rByteBuf[2*i] & 0x0ff));
    }

    jcharArray result = env->NewCharArray(length/2);
    env->SetCharArrayRegion(result, 0, length/2, rCharBuf);
    env->ReleaseByteArrayElements(src, cbuf, 0);
    delete rByteBuf;
    env->ReleaseCharArrayElements(result, rCharBuf, JNI_ABORT);

//    LOGD("GetBgBuffer Done");
    return result;
}
*/

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

    LOGD("GetBgBuffer Done");
    return result;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){
    LOGI("NativeGraphicJni.so Loaded.");
    return JNI_VERSION_1_4;     //这里很重要，必须返回版本，否则加载会失败。
}

#ifdef __cplusplus
}
#endif
