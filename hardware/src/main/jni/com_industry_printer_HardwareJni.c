/* DO NOT EDIT THIS FILE - it is machine generated */


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
//#include <utils/Log.h>
#include <com_industry_printer_HardwareJni.h>

#define JNI_TAG "serial_jni"

#define ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,JNI_TAG,__VA_ARGS__)
#define ALOGI(...)  __android_log_print(ANDROID_LOG_INFO,JNI_TAG,__VA_ARGS__)

static jboolean isSmfyProduct(JNIEnv *env) {
	jboolean isSmfy;
	jclass cls = (*env)->FindClass(env, "com/industry/printer/Utils/PlatformInfo");
	jclass gCls = (*env)->NewGlobalRef(env, cls);
	jmethodID isSm = (*env)->GetStaticMethodID(env, gCls, "isSmfyProduct", "()Z");
	isSmfy = (*env)->CallStaticBooleanMethod(env, gCls, isSm, NULL);
	return isSmfy;
}

/**
 * 读写UsbSerial的Jni接口
 */
static JNINativeMethod gMethods[] = {

	{"open",		"(Ljava/lang/String;)I", 	(void *)Java_com_industry_printer_UsbSerial_open},
	{"setBaudrate",	"(II)I", 					(void *)Java_com_industry_printer_UsbSerial_setBaudrate},
	{"close",		"(I)I",						(void *)Java_com_industry_printer_UsbSerial_close},
	{"write",		"(I[SI)I",					(void *)Java_com_industry_printer_UsbSerial_write},
	{"read",		"(II)[B",					(void *)Java_com_industry_printer_UsbSerial_read},
	{"set_options",	"(IIII)I",					(void *)Java_com_industry_printer_UsbSerial_set_options},
	//{"get_BuildDate","()Ljava/lang/String;",	(void *)Java_com_industry_printer_UsbSerial_get_BuildDate},
};

/**
 * 读写FPGA的GPIO的jni接口
 */
static JNINativeMethod gGPIOMethods[] = {
	{"open",		"(Ljava/lang/String;)I",	(void *)Java_com_industry_printer_GPIO_open},
	{"write",		"(I[CI)I",					(void *)Java_com_industry_printer_GPIO_write},
	{"read",		"(I)I",						(void *)Java_com_industry_printer_GPIO_read},
	{"ioctl",		"(IIJ)I",					(void *)Java_com_industry_printer_GPIO_ioctl},
	{"poll",		"(I)I",						(void *)Java_com_industry_printer_GPIO_poll},
	{"close",		"(I)I",						(void *)Java_com_industry_printer_GPIO_close},
};

/**
 * 读写RFID的jni接口
 */
static JNINativeMethod gRFIDMethods[] = {
	{"open",		"(Ljava/lang/String;)I",	(void *)Java_com_industry_printer_RFID_open},
	{"write",		"(I[SI)I",					(void *)Java_com_industry_printer_RFID_write},
	{"read",		"(II)[B",					(void *)Java_com_industry_printer_RFID_read},
	{"close",		"(I)I",						(void *)Java_com_industry_printer_RFID_close},
	{"setBaudrate",	"(II)I",					(void *)Java_com_industry_printer_RFID_setBaudrate},
};

/**
 * RTC操作jni接口
 */
static JNINativeMethod gRTCMethods[] = {
	{"open",					"(Ljava/lang/String;)I",	(void *)Java_com_industry_printer_RTC_open},
	{"syncSystemTimeFromRTC",	"(I)V",						(void *)Java_com_industry_printer_RTC_read},
	{"syncSystemTimeToRTC",		"(I)V",						(void *)Java_com_industry_printer_RTC_write},
	{"close",					"(I)I",						(void *)Java_com_industry_printer_RTC_close},
};


/**
 * 注册usb转串口的JNI方法
 */
int register_com_industry_printer_Usb2Serial(JNIEnv *env) {
    const char* kClassPathName = "com/industry/printer/hardware/HardwareJni";
	jclass clazz = (*env)->FindClass(env, kClassPathName);
	if(clazz == NULL)
		return JNI_FALSE;
    return (*env)->RegisterNatives(env, clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
}


/**
 * 注册FPGA-GPIO的JNI方法
 */
int register_com_industry_printer_GPIOOperation(JNIEnv *env) {
	const char* kClassPathName = "com/industry/printer/hardware/FpgaGpioOperation";
	jclass clazz = (*env)->FindClass(env, kClassPathName);
	if(clazz == NULL) {
		return JNI_FALSE;
	}
	return (*env)->RegisterNatives(env, clazz, gGPIOMethods, sizeof(gGPIOMethods) / sizeof(gGPIOMethods[0]));
}

/**
 * 注册RFID操作的JNI方法
 */
int register_com_industry_printer_RFIDOperation(JNIEnv* env) {
	const char* kClassPathName = "com/industry/printer/hardware/RFIDDevice";
	jclass clazz = (*env)->FindClass(env, kClassPathName);
	if(clazz == NULL) {
		return JNI_FALSE;
	}
	return (*env)->RegisterNatives(env, clazz, gRFIDMethods, sizeof(gRFIDMethods) / sizeof(gRFIDMethods[0]));
}

/**
 * 注册RTC操作的JNI方法
 */
int register_com_industry_printer_RTCOperation(JNIEnv* env) {
	const char* kClassPathName = "com/industry/printer/hardware/RTCDevice";
	jclass clazz = (*env)->FindClass(env, kClassPathName);
	if(clazz == NULL) {
		return JNI_FALSE;
	}
	return (*env)->RegisterNatives(env, clazz, gRTCMethods, sizeof(gRTCMethods)/sizeof(gRTCMethods[0]));
}


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

	ALOGI("Hardware_jni.so 1.0.4 Loaded.");

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        //__android_log_print(ANDROID_LOG_INFO, JNI_TAG,"ERROR: GetEnv failed\n");
        goto fail;
    }

    if (register_com_industry_printer_Usb2Serial(env) < 0) {
        goto fail;
    }

    if (register_com_industry_printer_GPIOOperation(env) < 0) {
    	goto fail;
    }

    if (register_com_industry_printer_RFIDOperation(env) < 0) {
    	goto fail;
    }

    if (1 == isSmfyProduct(env)) {
		if (register_com_industry_printer_RTCOperation(env) < 0) {
			goto fail;
		}
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

fail:
    return result;
}

