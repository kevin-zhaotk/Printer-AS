#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <jni.h>
#include <time.h>
//#include <utils/Log.h>
#include <linux/rtc.h>
#include <com_industry_printer_HardwareJni.h>

#define JNI_TAG "RTC_jni"
#define ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,JNI_TAG,__VA_ARGS__)

/**
 * 打开RTC设备
 */
JNIEXPORT jint JNICALL Java_com_industry_printer_RTC_open
  (JNIEnv *env, jclass arg, jstring dev)
{
	int ret;
	char *dev_utf = (*env)->GetStringUTFChars(env, dev, JNI_FALSE);
	ret = open(dev_utf, O_RDWR);

	if( ret == -1)
	{
		ALOGD("can not open RTC device %s", dev_utf);
	}
	else
	{
		ALOGD("RTC device open success");
	}
	(*env)->ReleaseStringUTFChars(env, dev, dev_utf);
	return ret;
}



/**
 * 关闭RTC设备
 */
JNIEXPORT jint JNICALL Java_com_industry_printer_RTC_close
  (JNIEnv *env, jclass arg, jint fd)
{
	close(fd);
	return 0;
}


/**
 * Class:     com_industry_printer_RTC
 * 读取RTC时间
 */
JNIEXPORT void JNICALL Java_com_industry_printer_RTC_read
  (JNIEnv *env, jclass arg, jint fd)
{
	int retval = -1;
	struct rtc_time rtc_tm;
	struct timeval tv;
	struct tm time;
	struct tm *tm;

	if (fd <= 0) {
		ALOGD("--->RTC device not opened");
		return;
	}
	ALOGD("--->RTC device read time");
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		return ;
	}
	gettimeofday(&tv, NULL);
	ALOGD("--->tv_sec:%d", tv.tv_sec);
	ALOGD("--->system time day=%d, month=%d, year=%d, hour=%d, min=%d, sec=%d", rtc_tm.tm_mday, rtc_tm.tm_mon, rtc_tm.tm_year,
            rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	memset(&time, 0, sizeof(time));
	time.tm_sec = rtc_tm.tm_sec;         /* seconds */
	ALOGD("--->tm_sec:%d", time.tm_sec);
	time.tm_min = rtc_tm.tm_min;         /* minutes */
	ALOGD("--->tm_min:%d", time.tm_min);
	time.tm_hour= rtc_tm.tm_hour;        /* hours */
	ALOGD("--->tm_hour:%d", time.tm_hour);
	time.tm_mday= rtc_tm.tm_mday;        /* day of the month */
	ALOGD("--->tm_mday:%d", time.tm_mday);
	time.tm_mon = rtc_tm.tm_mon;         /* month */
	ALOGD("--->tm_mon:%d", time.tm_mon);
	time.tm_year= rtc_tm.tm_year;  /* year */
	ALOGD("--->tm_year:%d", time.tm_year);

	//time(&timep);
	tv.tv_sec = mktime(&time);
	if(tv.tv_sec == -1) {
		ALOGD("--->errno:%s", strerror(errno));
	}

	tv.tv_usec = 0;
	ALOGD("--->tv_sec:%d", tv.tv_sec);
	retval = settimeofday(&tv, NULL);

	ALOGD("--->settimeoday ret :%s", strerror(errno));
}

/*
 * Class:     com_industry_printer_RTC
 * Method:    设置RTC时间
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT void JNICALL Java_com_industry_printer_RTC_write
  (JNIEnv *env, jclass arg, jint fd)
{

}

