#include "com_industry_printer_Serial_SerialPort.h"
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

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
 * Signature: (Ljava/lang/String;I)Ljava/io/FileDescriptor;
 */
JNIEXPORT jobject JNICALL Java_com_industry_printer_Serial_SerialPort_openStream
  (JNIEnv *env, jobject object, jstring path, jint baudrate) {

    speed_t speed;
    int fd = -1;
    jobject mFileDescriptor;

    LOGI("Check arguments");
    /* Check arguments */
    {
        speed = getBaudrate(baudrate);
        if (speed == -1) {
            /* TODO: throw an exception */
            LOGE("Invalid baudrate");
            return NULL;
        }
    }

    LOGI("Opening device!");
    /* Opening device */
    {
        jboolean iscopy;
        const char *path_utf = env->GetStringUTFChars(path, &iscopy);
        LOGD("Opening serial port %s", path_utf);
//        fd = open(path_utf, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
        fd = open(path_utf, O_RDWR);
        LOGD("Opened serial port[%s] as [%d]", path_utf, fd);
        if (fd == -1) {
            /* Throw an exception */
            LOGE("Failed to open port %s", path_utf);
            env->ReleaseStringUTFChars(path, path_utf);
            return NULL;
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

    {
        jclass cFileDescriptor = env->FindClass("java/io/FileDescriptor");
        jmethodID iFileDescriptor = env->GetMethodID(cFileDescriptor, "<init>", "()V");
        jfieldID descriptorID = env->GetFieldID(cFileDescriptor, "descriptor", "I");
        mFileDescriptor = env->NewObject(cFileDescriptor, iFileDescriptor);
        env->SetIntField(mFileDescriptor, descriptorID, (jint)fd);
    }

    return mFileDescriptor;
}

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;I)I;
 */

JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_openSerial
        (JNIEnv *env, jobject object, jstring path, jint baudrate) {

    speed_t speed;
    int fd = -1;

    LOGI("Check arguments");
    // Check arguments
    {
        speed = getBaudrate(baudrate);
        if (speed == -1) {
            LOGE("Invalid baudrate");
            return -1;
        }
    }

    LOGI("Opening device!");
    // Opening device
    {
        jboolean iscopy;
        const char *path_utf = env->GetStringUTFChars(path, &iscopy);
        LOGD("Opening serial port %s", path_utf);

        // 一定要设置O_NONBLOCK，否则read函数会一直等待MAX_RETRIVAL_BUFFER_LEN字节接收完成
        fd = open(path_utf, O_RDWR | O_NOCTTY | O_NONBLOCK/* | O_NDELAY*/);
        LOGD("Opened serial port[%s] as [%d]", path_utf, fd);
        if (fd == -1) {
            LOGE("Failed to open port %s", path_utf);
            env->ReleaseStringUTFChars(path, path_utf);
            return -1;
        }
        env->ReleaseStringUTFChars(path, path_utf);
    }

    LOGI("Configure device!");
    // Configure device
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
//            return -1;
        }
    }

    return fd;
}

//static bool mKeepRunning = false;

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    stop
 * Signature: ()V
 */

JNIEXPORT void JNICALL Java_com_industry_printer_Serial_SerialPort_stop
        (JNIEnv *, jobject) {

//    mKeepRunning = false;
}

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    close
* Signature: (Ljava/io/FileDescriptor)I
*/
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_closeStream
  (JNIEnv *env, jobject object, jobject fd) {

//    jclass SerialPortClass = env->GetObjectClass(object);
    jclass FileDescriptorClass = env->FindClass("java/io/FileDescriptor");

//    jfieldID mFdID = env->GetFieldID(SerialPortClass, "mFd", "Ljava/io/FileDescriptor;");
    jfieldID descriptorID = env->GetFieldID(FileDescriptorClass, "descriptor", "I");

//    jobject mFd = env->GetObjectField(object, mFdID);
    jint descriptor = env->GetIntField(fd, descriptorID);

    return close(descriptor);
}

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    close
* Signature: (I)I
*/

JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_closeSerial
        (JNIEnv *env, jobject object, jint fd) {

    if(fd <= 0) {
        LOGE("Invalid fd[%d]!", fd);
        return -1;
    }

//    mKeepRunning = false;

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

#define MAX_RETRIVAL_BUFFER_LEN  1024
#define MAX_TEMP_BUFFER_LEN      128

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    read
* Signature: (I)[B
*/
JNIEXPORT jbyteArray JNICALL Java_com_industry_printer_Serial_SerialPort_read
  (JNIEnv *env, jobject object, jint fd) {

    if (fd <= 0) {
        LOGE("Invalid fd[%d]!", fd);
        return NULL;
    }

    LOGD("Reading from Serial Port (%d) ...", fd);

    struct timeval timeout;
    fd_set set;

    char recv_buf[MAX_RETRIVAL_BUFFER_LEN];
//    jbyte *recv_buf = new jbyte[MAX_RETRIVAL_BUFFER_LEN];
//    memset(recv_buf, 0x00, MAX_RETRIVAL_BUFFER_LEN);
    char temp_buf[MAX_TEMP_BUFFER_LEN];

    int recv_num = 0;

	while(true) {
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;        // 等待100ms，如果100ms没有数据即按超时处理，此时，如果缓冲区有数据，视为数据接收完成，如果没有数据视为没有数据传送

        // 无论O_NONBLOCK,O_NDELAY设与不设，这里都会等待到超时，如果中间没有数据发生
        select(fd+1, &set, NULL, NULL, &timeout);

        if(FD_ISSET(fd, &set)) {
            // 阻断模式会等到MAX_TEMP_BUFFER_LEN字节全部接收完成，非阻断模式则会在接收到现有数据后立即返回
            int rnum = read(fd, temp_buf, MAX_TEMP_BUFFER_LEN);

            if(rnum > 0) {
//                LOGD("-[%d] bytes read.", recv_num);
                rnum = ((recv_num + rnum) > MAX_RETRIVAL_BUFFER_LEN ? (MAX_RETRIVAL_BUFFER_LEN - recv_num) : rnum);
                memcpy(&recv_buf[recv_num], temp_buf, rnum);
                recv_num += rnum;
            }
        } else {
            if (recv_num > 0) {
                break;
            }
        }
	}

    LOGD("[%d] bytes read.", recv_num);

    jbyteArray ret_buf = env->NewByteArray(recv_num);
    env->SetByteArrayRegion(ret_buf, 0, recv_num, (jbyte *)recv_buf);
//    env->ReleaseByteArrayElements(ret_buf, recv_buf, JNI_ABORT);

    return ret_buf;

}
/*
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

    struct timeval timeout;
    fd_set set;

    mKeepRunning = true;

    char recv_buf[MAX_RETRIVAL_BUFFER_LEN];
    memset(recv_buf, 0x00, MAX_RETRIVAL_BUFFER_LEN);
    char temp_buf[MAX_TEMP_BUFFER_LEN];

    int recv_num = 0;

    while(mKeepRunning) {
        FD_ZERO(&set);
        FD_SET(fd, &set);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;        // 等待30ms，如果30ms没有数据即按超时处理，此时，如果缓冲区有数据，视为数据接收完成，如果没有数据视为没有数据传送

        // 无论O_NONBLOCK,O_NDELAY设于不设，这里都会等待到超时，如果中间没有数据发生
        select(fd+1, &set, NULL, NULL, &timeout);

        if(FD_ISSET(fd, &set)) {
            // 阻断模式会等到MAX_TEMP_BUFFER_LEN字节全部接收完成，非阻断模式则会在接收到现有数据后立即返回
            int rnum = read(fd, temp_buf, MAX_TEMP_BUFFER_LEN);

            if(rnum > 0) {
                rnum = ((recv_num + rnum) > MAX_RETRIVAL_BUFFER_LEN ? (MAX_RETRIVAL_BUFFER_LEN - recv_num) : rnum);
                memcpy(&recv_buf[recv_num], temp_buf, rnum);
                recv_num += rnum;
            }
        } else {
            if (recv_num > 0) {
                LOGD("[%d] bytes read.", recv_num);

                jbyteArray ret_buf = env->NewByteArray(recv_num);    // 不知道为什么，这里设800，接收大数据没有问题，881就崩溃。中间的没有测，意义不大了
                env->SetByteArrayRegion(ret_buf, 0, recv_num, (jbyte *) recv_buf);
                env->CallVoidMethod(object, method, ret_buf);
                env->DeleteLocalRef(ret_buf);

                recv_num = 0;
            }
        }
    }

    LOGD("Quit reading process. (fd = %d)", fd);

    env->DeleteLocalRef(SerialPortClass);

    return 0;

}
*/
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
    // 2021-11-25 修改读数据方式，取消从JNI向JAVA回写的方式，改为直接返回
    // 2021-11-25 select的超时设置为30ms，取消30000次计数，一定要设置O_NONBLOCK，否则read函数会一直等待MAX_RETRIVAL_BUFFER_LEN字节接收完成
    // 2021-11-24 为了PCCommand的按着Stream方式打开，原来的串口通讯方式恢复原样
    // 2021-10-29 1.0.54 修改为FileDescriptor版本的open和close。因此，本库中仅有open和close函数是实际有效的函数，其他函数将不会被使用到
    LOGI("SerialPort.so 1.0.58 Loaded.");

    return JNI_VERSION_1_4;     //这里很重要，必须返回版本，否则加载会失败。
}

#ifdef __cplusplus
}
#endif
