#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <jni.h>
#include "com_industry_printer_HardwareJni.h"

#define JNI_TAG "GPIO_operation"
#define ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,JNI_TAG,__VA_ARGS__)
#define ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR,JNI_TAG,__VA_ARGS__)

/**
 * Fpga Gpio operation APIs
 **/
JNIEXPORT jint JNICALL Java_com_industry_printer_GPIO_open
	(JNIEnv *env, jclass arg, jstring dev)
{
	int ret=-1;
	ALOGD("===>open gpio\n");

	char *dev_utf = (*env)->GetStringUTFChars(env, dev, JNI_FALSE);
	if(dev_utf == NULL)
	{
		return -1;
	}
	ret = open(dev_utf, O_RDWR);
	if(ret < 0) {
		ALOGD("***** gpio open fail, err=%s\n", strerror(errno));
	}
	(*env)->ReleaseStringUTFChars(env, dev, dev_utf);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_industry_printer_GPIO_write
	(JNIEnv *env, jclass arg, jint fd, jcharArray buff, jint count)
{
	int i,ret;
	jchar *buf_utf = (*env)->GetCharArrayElements(env, buff, NULL);

	if(fd <= 0)
		return 0;
	ret = write(fd, buf_utf, count);
	(*env)->ReleaseCharArrayElements(env, buff, buf_utf, 0);
	return ret;
}


JNIEXPORT jint JNICALL Java_com_industry_printer_GPIO_ioctl
	(JNIEnv *env, jclass arg, jint fd, jint cmd, jlong arg1)
{
	int i,ret;

	if(fd <= 0)
		return 0;
	ret = ioctl(fd, cmd, arg1);
	return ret;
}

static int sFd;
static int keepRunning = 0;
static int driverReady = 0;

void timer_proc(int signo) {
    if(SIGALRM == signo) {
        if(sFd > 0) {
            char buf = 0x00;
            if(read(sFd, &buf, 1) > 0 && keepRunning) {
//                ALOGD("driver ready");
                driverReady = 1;
            }
        }
    }
}

JNIEXPORT jint JNICALL Java_com_industry_printer_GPIO_start_monitor(JNIEnv *env, jclass arg, jint fd) {
    ALOGD("enter monitor. 6");

    sFd = fd;

    keepRunning = 1;

    jmethodID method = (*env)->GetStaticMethodID(env, arg, "onReady", "()V");
    if(NULL == method) {
        ALOGE("Failed to get JAVA receiver function id!");
        return -1;
    }

    struct itimerval value, ovalue;

    signal(SIGALRM, timer_proc);

    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 1000;
    setitimer(ITIMER_REAL, &value, &ovalue);

    while(keepRunning) {
        if(driverReady) {
            (*env)->CallVoidMethod(env, arg, method);
            driverReady = 0;
        }
        usleep(1000);
    };

    ALOGD("quit monitor");
    return 0;
}

JNIEXPORT jint JNICALL Java_com_industry_printer_GPIO_stop_monitor(JNIEnv *env, jclass arg, jint fd) {
    struct itimerval value,ovalue,value2;

    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &value, &ovalue);

    keepRunning = 0;

    return 0;
}

JNIEXPORT jint JNICALL Java_com_industry_printer_GPIO_poll
       (JNIEnv *env, jclass arg, jint fd)
{
    int ret=0;
    int maxfd=0;
    struct timeval timeout;
    fd_set fds;

    if (fd <= 0)
           return -1;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    maxfd = fd+1;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100;
    ret = select(maxfd, NULL, &fds, NULL, &timeout);
    return ret;

/*    char buf = 0x00;
    return read(fd, &buf, 1);
*/
}

JNIEXPORT jint JNICALL Java_com_industry_printer_GPIO_read
       (JNIEnv *env, jclass arg, jint fd)
{
       int ret=0;
       char buf[4];
       ret = read(fd, buf, sizeof(buf));
       ALOGD("*****read %s\n", buf);
       return ret;
}


JNIEXPORT jint JNICALL Java_com_industry_printer_GPIO_close
	(JNIEnv *env, jclass arg, jint fd)
{
	int ret=-1;
	ret = close(fd);
	if(ret < 0) {
		ALOGD("***** gpio close fail\n");
	}
	return ret;
}
