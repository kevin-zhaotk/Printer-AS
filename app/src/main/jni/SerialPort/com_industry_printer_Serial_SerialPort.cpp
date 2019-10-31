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

using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif

int server_connection();

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
 * Method:    OpenSerialPort
 * Signature: (Ljava/lang/String;I)Ljava/io/FileDescriptor;
 */
JNIEXPORT jobject JNICALL Java_com_industry_printer_Serial_SerialPort_OpenSerialPort
  (JNIEnv *env, jobject object, jstring path, jint baudrate)
{
    speed_t speed;
    jobject mFileDescriptor;
    int fd;

    LOGD("init native Check arguments");
    /* Check arguments */
    {
        speed = getBaudrate(baudrate);
        if (speed == -1) {
            /* TODO: throw an exception */
            LOGE("Invalid baudrate");
            return NULL;
        }
    }

    LOGD("init native Opening device!");
    /* Opening device */
    {
        jboolean iscopy;
        const char *path_utf = env->GetStringUTFChars(path, &iscopy);
        LOGD("Opening serial port %s", path_utf);
        fd = open(path_utf, O_RDWR);
//        fd = open(path_utf, O_RDWR | O_DIRECT | O_SYNC);
//        fd = open(path_utf, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
        LOGD("open() fd = %d", fd);
        env->ReleaseStringUTFChars(path, path_utf);
        if (fd == -1) {
            /* Throw an exception */
            LOGE("Cannot open port %d",baudrate);
            /* TODO: throw an exception */
            return NULL;
        }
    }

    LOGD("init native Configure device!");
    /* Configure device */
    {
        struct termios cfg;
        if (tcgetattr(fd, &cfg)) {
            LOGE("Configure device tcgetattr() failed 1");
            close(fd);
            return NULL;
        }

        cfmakeraw(&cfg);
        cfsetispeed(&cfg, speed);
        cfsetospeed(&cfg, speed);

        if (tcsetattr(fd, TCSANOW, &cfg)) {
            LOGE("Configure device tcsetattr() failed 2");
            close(fd);
            /* TODO: throw an exception */
            return NULL;
        }
    }

    /* Create a corresponding file descriptor */
    {
    	/* Call Function @ JAVA from JNI
        jclass dpclazz  = env->GetObjectClass(object);
        jmethodID method1 = env->GetMethodID(dpclazz,"CommandReceived","(Ljava/lang/String;)V");
        env->CallVoidMethod(object, method1);
		*/

        jclass cFileDescriptor = env->FindClass("java/io/FileDescriptor");
        jmethodID iFileDescriptor = env->GetMethodID(cFileDescriptor,"<init>", "()V");
        jfieldID descriptorID = env->GetFieldID(cFileDescriptor,"descriptor", "I");
        mFileDescriptor = env->NewObject(cFileDescriptor,iFileDescriptor);
        env->SetIntField(mFileDescriptor, descriptorID, (jint) fd);
    }

//    server_connection();

    return mFileDescriptor;
}

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    CloseSerialPort
* Signature: ()I
*/
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_CloseSerialPort
  (JNIEnv *env, jobject object)
{
    jclass SerialPortClass = env->GetObjectClass(object);
//	jclass SerialPortClass = thiz;
    jclass FileDescriptorClass = env->FindClass("java/io/FileDescriptor");

    jfieldID mFdID = env->GetFieldID(SerialPortClass, "mFd", "Ljava/io/FileDescriptor;");
    jfieldID descriptorID = env->GetFieldID(FileDescriptorClass, "descriptor", "I");

    jobject mFd = env->GetObjectField(object, mFdID);
    jint descriptor = env->GetIntField(mFd, descriptorID);

    LOGD("close(fd = %d)", descriptor);
    close(descriptor);
    return 1;

}

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    ReadSerialPort
* Signature: ()I
*/
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_ReadSerialPort
  (JNIEnv *env, jobject object)
{
    jclass SerialPortClass = env->GetObjectClass(object);
//	jclass SerialPortClass = thiz;
    jclass FileDescriptorClass = env->FindClass("java/io/FileDescriptor");

    jfieldID mFdID = env->GetFieldID(SerialPortClass, "mFd", "Ljava/io/FileDescriptor;");
    jfieldID descriptorID = env->GetFieldID(FileDescriptorClass, "descriptor", "I");

    jobject mFd = env->GetObjectField(object, mFdID);
    jint descriptor = env->GetIntField(mFd, descriptorID);

	char cmd[256];
	memset(cmd, 0x00, 256);
	bool icmd = false;

	while(1) {
		char buf[256];
		memset(buf, 0x00, 256);

//		LOGD("Receiving from %d", descriptor);
		int rnum = read(descriptor, buf, 256);

		if(rnum > 0) {
//			LOGD("%d - %s", rnum, buf);
			for(int i=0; i<strlen(buf); i++) {
//				LOGD("%s", cmd);
				if(buf[i] == ']') {
					cmd[strlen(cmd)] = buf[i];
//					LOGD("%s", cmd);
        	        jclass dpclazz  = env->GetObjectClass(object);
        	        jstring retString = env->NewStringUTF(cmd);
        	        jmethodID method1 = env->GetMethodID(dpclazz,"CommandReceived","(Ljava/lang/String;)V");
        	        env->CallVoidMethod(object, method1, retString);
					memset(cmd, 0x00, 256);
					icmd = false;
				} else if(buf[i] == '[') {
					cmd[strlen(cmd)] = buf[i];
					icmd = true;
				} else if(icmd) {
					cmd[strlen(cmd)] = buf[i];
				}
			}
		}
	}

    return 1;

}

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    WriteSerialPort
* Signature: ([C)I
*/
/*
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_WriteSerialPort
  (JNIEnv *env, jobject object, jcharArray buf)
{
	jchar *sbuf = env->GetCharArrayElements (buf, 0);
	char *ssbuf = new char[env->GetArrayLength(sbuf)];
	strcpy(ssbuf, sbuf);

    jclass SerialPortClass = env->GetObjectClass(object);
//	jclass SerialPortClass = thiz;
    jclass FileDescriptorClass = env->FindClass("java/io/FileDescriptor");

    jfieldID mFdID = env->GetFieldID(SerialPortClass, "mFd", "Ljava/io/FileDescriptor;");
    jfieldID descriptorID = env->GetFieldID(FileDescriptorClass, "descriptor", "I");

    jobject mFd = env->GetObjectField(object, mFdID);
    jint descriptor = env->GetIntField(mFd, descriptorID);

	int rnum = write(descriptor, (char const *)ssbuf, strlen(ssbuf));
	LOGD("[%s] - %d bytes written to port.", ssbuf, rnum);

    env->ReleaseCharArrayElements(buf, sbuf, 0);

    return 1;

}
*/

/*
* Class:     com_industry_printer_Serial_SerialPort
* Method:    GetSectorCount
* Signature: ()I
*/
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_GetSectorCount
  (JNIEnv *env, jobject object)
{
	int fdd = open("/storage/external_storage/sdb4", O_RDWR);
    int64_t size;

    if(fdd == -1) {
        LOGD("open failed.");
    } else {
        LOGD("open success.");
    }

    unsigned char *buf;
    buf = (unsigned char*)malloc(156);
    memset(buf, 0x00, 256);
    size_t w = write(fdd, buf, 256);
    if(w == -1) {
        LOGD("write failed.");
    }

    if (ioctl(fdd, BLKGETSIZE, &size) >= 0) {
        LOGD("Size: %lld.", size);
    }else{
        LOGD("ioctl failed.");
    }
    return 0;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		return -1;
	}
	assert(env != NULL);

//	if (!registerNatives(env)) {//ע��
//		return -1;
//	}
	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

	return result;
}

/*
int ret = 0;

JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_ReadSD
 (JNIEnv *env, jobject obj,  jstring sdname, jint sectorstart, jcharArray buffer) {
    size_t sector_size = 0;
    int col;
    unsigned short *array;
    unsigned char *buf;
    int i;

    // jstring to char
    char* rtn = NULL;
    jclass clsstring = (*env)->FindClass(env,"java/lang/String");
    jstring strencode = (*env)->NewStringUTF(env,"utf-8");
    jmethodID mid = (*env)->GetMethodID(env,clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)(*env)->CallObjectMethod(env,sdname, mid, strencode);
    jsize alen = (*env)->GetArrayLength(env,barr);
    jbyte* ba = (*env)->GetByteArrayElements(env,barr, JNI_FALSE);
    if (alen > 0) {
    	rtn = (char*)malloc(alen + 1);
    	memcpy(rtn, ba, alen);
    	rtn[alen] = 0;
    }

    (*env)->ReleaseByteArrayElements(env,barr, ba, 0);

    int fd = open(rtn,O_RDONLY);

    if (fd == -1) {
        // printf("Open \"sdname \" failed. %s\n",strerror(errno));
        return errno;
    }

    // jchar * buf = (*env)->GetCharArrayElements(env,buffer,0);

    ioctl(fd, BLKSSZGET, &sector_size);
    // buffer = (unsigned char*)malloc(sector_size);
    buf = (unsigned char*)malloc(sector_size);

    lseek(fd,sectorstart * sector_size ,SEEK_SET);

    // ret = read(fd,buffer,(unsigned int)sector_size);
    ret = read(fd,buf,(unsigned int)sector_size);
    printf("%d",ret);
    printf("\n");

    // jcharArray iarr = (*env)->NewCharArray(env, sector_size);

    (*env)->SetByteArrayRegion(env,buffer, 0, sector_size, buf);

    if (ret < 0) {
        printf("Read sectors failed. %s\n",strerror(errno));
        return errno;
    }

    close(fd);
    return ret;
}
*/

int server_connection() {
	int client_sockfd;
	int len;

	struct sockaddr_in remote_addr; //服务器端网络地址结构体
	char buf[BUFSIZ];  //数据传送的缓冲区

	memset(&remote_addr,0,sizeof(remote_addr)); //数据初始化--清零
	remote_addr.sin_family = AF_INET; //设置为IP通信
	remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//服务器IP地址
	remote_addr.sin_port = htons(14797); //服务器端口号

	/*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
	if((client_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		LOGD("socket error %s. \n", strerror(errno));
		return 1;
	}

	LOGD("connected to server\n");
	/*将套接字绑定到服务器的网络地址上*/
	if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr)) < 0) {
		LOGD("connect error\n");
		return 1;
	}

	len = recv(client_sockfd,buf,BUFSIZ,0);//接收服务器端信息
	buf[len]='\0';

	close(client_sockfd);//关闭套接字
	return 0;
}


#ifdef __cplusplus
}
#endif
