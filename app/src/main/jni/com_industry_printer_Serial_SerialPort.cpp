#include "com_industry_printer_Serial_SerialPort.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <errno.h>
#include <sys/vfs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C"
{
#endif

static int __write(int fd, const char *buf, int len);

static speed_t getBaudrate(jint baudrate) {
    switch (baudrate) {
    case 0:
        return B0;
    case 50:
        return B50;
    case 75:
        return B75;
    case 110:
        return B110;
    case 134:
        return B134;
    case 150:
        return B150;
    case 200:
        return B200;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        return -1;
    }
}

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;I)I;
 */
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_open
  (JNIEnv *env, jobject object, jstring path, jint baudrate) {

    speed_t speed;
    int fd = -1;

    LOGI("Check arguments");
    /* Check arguments */
    {
        speed = getBaudrate(baudrate);
        if (speed == -1) {
            /* TODO: throw an exception */
            LOGE("Invalid baudrate");
            return -1;
        }
    }

    LOGI("Opening device!");
    /* Opening device */
    {
        jboolean iscopy;
        const char *path_utf = env->GetStringUTFChars(path, &iscopy);
        LOGD("Opening serial port %s", path_utf);
        fd = open(path_utf, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
        LOGD("Opened serial port[%s] as [%d]", path_utf, fd);
        if (fd == -1) {
            /* Throw an exception */
            LOGE("Failed to open port %s", path_utf);
            env->ReleaseStringUTFChars(path, path_utf);
            return -1;
        }
        env->ReleaseStringUTFChars(path, path_utf);
    }

    LOGI("Configure device!");
    /* Configure device */
    {
        struct termios cfg;
        if (0 != tcgetattr(fd, &cfg)) {
            LOGE("Failed to get port attribute!");
//            close(fd);
//            return -1;
        }

        // cfmakeraw set the following parameters
        //  cfg->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
        //  cfg->c_oflag &= ~OPOST;
        //  cfg->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
        //  cfg->c_cflag &= ~(CSIZE|PARENB);
        //  cfg->c_cflag |= CS8;
        cfmakeraw(&cfg);

        cfsetispeed(&cfg, speed);
        cfsetospeed(&cfg, speed);

        tcflush(fd, TCIOFLUSH);

//        TCSANOW：不等数据传输完毕就立即改变属性。
//        TCSADRAIN：等待所有数据传输结束才改变属性。
//        TCSAFLUSH：清空输入输出缓冲区才改变属性。
        if (tcsetattr(fd, TCSANOW, &cfg)) {
            LOGE("Failed to set port attribute!");
//            close(fd);
            /* TODO: throw an exception */
//            return -1;
        }
    }

    return fd;
}

static bool mKeepRunning = false;

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    stop
 * Signature: ()V
 */

JNIEXPORT void JNICALL Java_com_industry_printer_Serial_SerialPort_stop
        (JNIEnv *, jobject) {

    mKeepRunning = false;
}

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    close
* Signature: (I)I
*/
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_close
  (JNIEnv *env, jobject object, jint fd) {

    if(fd <= 0) {
        LOGE("Invalid fd[%d]!", fd);
        return -1;
    }

    mKeepRunning = false;

    LOGD("close(fd = %d)", fd);

    return close(fd);
}
/*
char* toHexString(const char* buf, int len) {
    char ret_buf[3072];
    memset(ret_buf, 0x00, 3072);

    char s[16];
    for(int i=0; i<len; i++) {
        memset(s, 0x00, 16);
        if(i == 0) {
            sprintf(s, "%02X", buf[i]);
        } else {
            sprintf(s, " %02X", buf[i]);
        }
        strcat(ret_buf, s);
    }
    return ret_buf;
}
*/

#define MAX_RETRIVAL_BUFFER_LEN  920
#define MAX_TEMP_BUFFER_LEN     1024

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    read
* Signature: (I)I
*/
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_read
  (JNIEnv *env, jobject object, jint fd) {

    if (fd <= 0) {
        LOGE("Invalid fd[%d]!", fd);
        return -1;
    }

    LOGD("Reading from serial port (%d)", fd);

    jclass SerialPortClass  = env->GetObjectClass(object);
    jmethodID method = env->GetMethodID(SerialPortClass, "nativeDataReceived", "([B)V");
    if(NULL == method) {
        LOGE("Failed to get JAVA receiver function id!");
        env->DeleteLocalRef(SerialPortClass);
        return -1;
    }

    int maxfd = 0;
    struct timeval timeout;
    fd_set set;

    mKeepRunning = true;

    maxfd = 1;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100;

    char recv_buf[MAX_RETRIVAL_BUFFER_LEN];
    memset(recv_buf, 0x00, MAX_RETRIVAL_BUFFER_LEN);

    int recv_num = 0;
    int timeout_count = 0;

	while(mKeepRunning) {
        FD_ZERO(&set);
        FD_SET(fd, &set);

//        usleep(100000);
        select(maxfd, &set, NULL, NULL, &timeout);
        if(FD_ISSET(fd, &set)) {
            char temp_buf[MAX_TEMP_BUFFER_LEN];
            memset(temp_buf, 0x00, MAX_TEMP_BUFFER_LEN);

            int rnum = read(fd, temp_buf, MAX_TEMP_BUFFER_LEN);

            if(rnum > 0) {
                rnum = ((recv_num + rnum) > MAX_RETRIVAL_BUFFER_LEN ? (MAX_RETRIVAL_BUFFER_LEN - recv_num) : rnum);
                memcpy(&recv_buf[recv_num], temp_buf, (recv_num + rnum));
                recv_num += rnum;
                timeout_count = 0;
            } else if(rnum < 0) {
                usleep(10);
                timeout_count++;
                if(recv_num > 0 && timeout_count > 10000) {
//                if(recv_num > 0) {
                    LOGD("[%d] bytes read.", recv_num);

                    jbyteArray ret_buf = env->NewByteArray(recv_num);
                    env->SetByteArrayRegion(ret_buf, 0, recv_num, (jbyte*)recv_buf);
                    env->CallVoidMethod(object, method, ret_buf);

                    recv_num = 0;
                    memset(recv_buf, 0x00, MAX_RETRIVAL_BUFFER_LEN);
                    timeout_count = 0;
                }
            }
        }
	}

    LOGD("Quit reading process. (fd = %d)", fd);

    mKeepRunning = true;

    env->DeleteLocalRef(SerialPortClass);

    return 0;

}

static int __write(int fd, const char *buf, int len) {
    LOGD("Writing [%d] bytes to serial port (%d) ...", len, fd);

    tcflush(fd, TCIOFLUSH);
    int rnum = write(fd, buf, len);
    if(rnum > 0) {
        LOGD("[%d] bytes written.", rnum);
    } else {
        LOGE("Failed to write to serial port (%d)", fd);
    }

    return rnum;
}

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    write
* Signature: (I[BI)I
*/

JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_write
  (JNIEnv *env, jobject object, jint fd, jbyteArray buf, jint len) {

	jbyte *sbuf = env->GetByteArrayElements (buf, 0);

    int rnum = __write(fd, (char const *)sbuf, len);

    env->ReleaseByteArrayElements(buf, sbuf, 0);

    return rnum;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI("SerialPort.so 1.0.48 Loaded.");


    return JNI_VERSION_1_4;     //这里很重要，必须返回版本，否则加载会失败。
}

#ifdef __cplusplus
}
#endif
