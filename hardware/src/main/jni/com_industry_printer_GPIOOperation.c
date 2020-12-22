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
//#include <utils/Log.h>
#include "com_industry_printer_HardwareJni.h"

#define JNI_TAG "GPIO_operation"
#define ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,JNI_TAG,__VA_ARGS__)

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

pthread_t monitor_thread_id = 0;
static volatile int keepRunning = 0;
static volatile int state = 0;
void* monitor_thread(void* argv) {

    ALOGD("enter monitor_thread");
    keepRunning = 1;

    int a = *((int *)argv);
    while(keepRunning) {
        char buf = 0x00;
        state = read(*((int *)argv), &buf, 1);
        usleep(1000);
    }

    ALOGD("exit monitor_thread");
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
/*
       FD_ZERO(&fds);
       FD_SET(fd, &fds);
       maxfd = fd+1;
       timeout.tv_sec = 0;
       timeout.tv_usec = 100;
       ret = select(maxfd, NULL, &fds, NULL, &timeout);
*/
/*
    if(0 == monitor_thread_id) {
        if(pthread_create(&monitor_thread_id, NULL, monitor_thread, &fd) != 0){
            printf("Create command excution thread error [%d]%s.\n", errno, strerror(errno));
            monitor_thread_id = 0;
        }
    }
*/
	char buf = 0x00;
	ret = read(fd, &buf, 1);
       return ret;
//    return state;
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
    if(0 != monitor_thread_id) {
        keepRunning = 0;
        pthread_join(monitor_thread_id, NULL);
    }

	int ret=-1;
	ret = close(fd);
	if(ret < 0) {
		ALOGD("***** gpio close fail\n");
	}
	return ret;
}
