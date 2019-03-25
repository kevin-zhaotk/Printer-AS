//
// Created by kevin on 2019/2/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <jni.h>


JNIEXPORT jint JNICALL Java_com_Smartcard_open
        (JNIEnv *env, jclass arg, jstring dev)
{
    int ret=0;
    return ret;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_close
        (JNIEnv *env, jclass arg, jint fd)
{
    int ret=0;
    return ret;
}
/**
 * RTC操作jni接口
 */
static JNINativeMethod gMethods[] = {
        {"open",					"(Ljava/lang/String;)I",	(void *)Java_com_Smartcard_open},
        {"close",					"(I)I",						(void *)Java_com_Smartcard_close},
};

/**
 * 注册RTC操作的JNI方法
 */
int register_com_smartcard(JNIEnv* env) {
    const char* kClassPathName = "com/industry/printer/hardware/RTCDevice";
    jclass clazz = (*env)->FindClass(env, kClassPathName);
    if(clazz == NULL) {
        return JNI_FALSE;
    }
    return (*env)->RegisterNatives(env, clazz, gMethods, sizeof(gMethods)/sizeof(gMethods[0]));
}


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        //__android_log_print(ANDROID_LOG_INFO, JNI_TAG,"ERROR: GetEnv failed\n");
        goto fail;
    }

    if (register_com_smartcard(env) < 0) {
        goto fail;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    fail:
    return result;
}

