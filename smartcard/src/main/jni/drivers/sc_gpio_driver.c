/*********************************************************************************
    sc_gpio_driver.c

    @功能
        实现对GPIO端口的控制和状态读取。本驱动的前提是使用赵工的ext-gpio驱动
    @创建
        2019.10.15
    @作者
        H.M. Wang
    @版权
        Copyrights 2019 - 2029, Allrights reserved.
    @地点
        北京
    @变更履历
        2019.10.15 创建 Version 1.0
*********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "common_log.h"
#include "internal_ifc/sc_gpio_driver.h"

/*********************************************************************************
    SC_GPIO_DRIVER_open
----------------------------------------------------------------------------------
    @描述
        打开一个GPIO端口。
    @参数
        dev: GPIO驱动设备命
    @返回值
        >0: 文件描述符
        SC_GPIO_DRIVER_FAIL(-1)：失败
**********************************************************************************/

int SC_GPIO_DRIVER_open(char *dev) {
    LOGI(">>> SC_GPIO_DRIVER_open: %s", dev);

    if(dev == NULL)	{
        LOGE(">>> SC_GPIO_DRIVER_open: [null device]");
        return SC_GPIO_DRIVER_FAIL;
    }

    int fd = open(dev, O_RDWR);

    if(fd == SC_GPIO_DRIVER_FAIL) {
        LOGE(">>> SC_GPIO_DRIVER_open: Error[%s]", strerror(errno));
    } else {
        LOGD(">>> SC_GPIO_DRIVER_open: opened [%s] as [%d] ", dev, fd);
    }

    return fd;
}

/*********************************************************************************
    SC_GPIO_DRIVER_write
----------------------------------------------------------------------------------
    @描述
        向一个GPIO端口写数据。
    @参数
        fd: GPIO端口的文件描述符
        data_buf: 向GPIO端口写入的数据
        count: 数据长度
    @返回值
        >0: 写入数据的长度
        SC_GPIO_DRIVER_FAIL(-1)：失败
**********************************************************************************/

int SC_GPIO_DRIVER_write(int fd, char *data_buf, int count) {
    char buffer[1024];
    memset(buffer, 0x00, 1024);
    char s[16];
    for(int i=0; i<count; i++) {
        memset(s, 0x00, 16);
        if(i == 0) {
            sprintf(s, "%02X", data_buf[i]);
        } else {
            sprintf(s, " %02X", data_buf[i]);
        }
        strcat(buffer, s);
    }
    LOGI(">>> SC_GPIO_DRIVER_write: %s[%d] -> %d", buffer, count, fd);

    if(fd <= 0) {
        LOGE("Error: SC_GPIO_DRIVER_write - invalid device");
        return SC_GPIO_DRIVER_FAIL;
    }

    int ret = write(fd, data_buf, count);
    if(ret == SC_GPIO_DRIVER_FAIL) {
        LOGE("Error: SC_GPIO_DRIVER_write - %s", strerror(errno));
    } else {
        LOGD("[%d] bytes written", ret);
    }

    return ret;
}

/*********************************************************************************
    SC_GPIO_DRIVER_ioctl
----------------------------------------------------------------------------------
    @描述
        向一个GPIO端口发送输入输出控制字。
    @参数
        fd: GPIO端口的文件描述符
        cmd: 控制字名称
        arg1: 控制字值
    @返回值
        SC_GPIO_DRIVER_SUCCESS(0): 设置成功
        SC_GPIO_DRIVER_FAIL(<=-1)：失败，及失败原因
**********************************************************************************/

int SC_GPIO_DRIVER_ioctl(int fd, int cmd, long arg1) {
    LOGI(">>> SC_GPIO_DRIVER_ioctl: 0x%02X[0x%04X] -> %d", cmd, arg1, fd);

    if(fd <= 0) {
        LOGE(">>> SC_GPIO_DRIVER_ioctl: Error[%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    uint8_t ret = ioctl(fd, cmd, arg1);
//	if(ret == EINVAL) {
    if(ret != SC_GPIO_DRIVER_SUCCESS) {
        LOGE(">>> SC_GPIO_DRIVER_ioctl: Error[%s](Retrun value:%x)", strerror(errno), ret);
    } else {
        LOGD(">>> SC_GPIO_DRIVER_ioctl: return value: [0x%02X]", ret);
    }

    return ret;
}

/*
int SC_GPIO_DRIVER_poll(int fd) {
	LOGI(">>> SC_GPIO_DRIVER_poll: %d", fd);

	if(fd <= 0) {
		LOGE("Error: SC_GPIO_DRIVER_poll - invalid device");
		return -1;
	}

	int maxfd=0;
	struct timeval timeout;
	fd_set fds;

	FD_ZERO(&fds);
    FD_SET(fd, &fds);
    maxfd = fd+1;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100;

    int ret = select(maxfd, NULL, &fds, NULL, &timeout);
	LOGD("select return value: [0x%08X]", ret);

    return ret;
}
*/

/*********************************************************************************
    SC_GPIO_DRIVER_read ------------------------------- 该函数需要待赵工将驱动写好后，根据其驱动进行再次修改
----------------------------------------------------------------------------------
    @描述
        从一个GPIO端口读取数值。
    @参数
        fd: GPIO端口的文件描述符
        pin: GPIO pin
    @返回值
        >0: 读取的数值
        SC_GPIO_DRIVER_FAIL(-1)：失败
**********************************************************************************/

int SC_GPIO_DRIVER_read(int fd, uint8_t pin) {
    LOGI(">>> SC_GPIO_DRIVER_read: %d, %d", fd, pin);

    if(fd <= 0) {
        LOGE(">>> SC_GPIO_DRIVER_read: Error[%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    uint8_t buf = 0;

    size_t ret = read(fd, &buf, sizeof(buf));
    if(ret == SC_GPIO_DRIVER_FAIL) {
        LOGE(">>> SC_GPIO_DRIVER_read: Error[%s]", strerror(errno));
    } else {
        LOGD(">>> SC_GPIO_DRIVER_read: [%d(0x%X)] - %d bytes read", buf, buf, ret);
    }

    return buf;
}

/*********************************************************************************
    SC_GPIO_DRIVER_close
----------------------------------------------------------------------------------
    @描述
        关闭一个GPIO端口。
    @参数
        fd: GPIO端口的文件描述符
    @返回值
        SC_GPIO_DRIVER_SUCCESS(0): 关闭成功
        SC_GPIO_DRIVER_FAIL(-1)：失败。错误信息保存在errno
**********************************************************************************/

int SC_GPIO_DRIVER_close(int fd) {
    LOGI(">>> SC_GPIO_DRIVER_close: %d", fd);

    if(fd <= 0) {
        LOGE(">>> SC_GPIO_DRIVER_close: Error[%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    int ret = close(fd);
    if(ret != SC_GPIO_DRIVER_SUCCESS) {
        LOGE(">>> SC_GPIO_DRIVER_close: Error[%s]", strerror(errno));
    } else {
        LOGD(">>> SC_GPIO_DRIVER_close: [%d] closed.", fd);
    }

    return ret;
}

/*********************************************************************************
    SP_GPIO_DRIVER_set_value
----------------------------------------------------------------------------------
    @描述
        向一个GPIO端口设置值。
    @参数
        port: GPIO端口号；
        value: 值；0：低电平；1：高电平
    @返回值
        SC_GPIO_DRIVER_SUCCESS(0): 关闭成功
        SC_GPIO_DRIVER_FAIL(-1)：失败。错误信息保存在errno
**********************************************************************************/

int SP_GPIO_DRIVER_set_value(int port, int value) {
    LOGI(">>> SP_GPIO_DRIVER_set_value to port [%d]: %d", port, value);

    FILE *fp;
    char buffer[10];
    char s[50]="";
    char s1[50]="";

/*    system("su");
    if(chmod("/sys/class/gpio/export", 777) == -1) {
        LOGE(">>> Error: SP_GPIO_DRIVER_set_value chmod. %s", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }
*/
    if((fp = fopen("/sys/class/gpio/export", "w")) == NULL) {
        LOGE(">>> Error: SP_GPIO_DRIVER_set_value export. %s", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    fprintf(fp, "%d", port);
    fclose(fp);

    sprintf(s, "/sys/class/gpio/gpio%d/direction", port);

    if((fp = fopen(s, "rb+")) == NULL) {
        LOGE(">>> Error: SP_GPIO_DRIVER_set_value port [%d] direction. %s", port, strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }
    fprintf(fp, "out");
    fclose(fp);

    sprintf(s1, "/sys/class/gpio/gpio%d/value", port);
    if((fp = fopen(s1, "rb+")) == NULL) {
        LOGE(">>> Error: SP_GPIO_DRIVER_set_value port [%d] value. %s", port, strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    fprintf(fp, "%d", value);
    fclose(fp);

    if((fp = fopen("/sys/class/gpio/unexport", "w")) == NULL) {
        LOGE(">>> Error: SP_GPIO_DRIVER_set_value unexport. %s", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    fprintf(fp, "%d", port);
    fclose(fp);

    LOGD("SP_GPIO_DRIVER_set_value [%d] to port [%d]", value, port);

    return SC_GPIO_DRIVER_SUCCESS;
}

/*********************************************************************************
    SP_GPIO_DRIVER_get_value
----------------------------------------------------------------------------------
    @描述
        向一个GPIO端口设置值。
    @参数
        port: GPIO端口号；
    @返回值
        value: 值；0：低电平；1：高电平
        SC_GPIO_DRIVER_FAIL(-1)：失败。错误信息保存在errno
**********************************************************************************/

int SP_GPIO_DRIVER_get_value(int port) {
    LOGI(">>> SP_GPIO_DRIVER_get_value from port [%d]", port);

    int value;

    FILE *fp;
    char buffer[10];
    char s[50]="";
    char s1[50]="";

    if((fp = fopen("/sys/class/gpio/export", "w")) == NULL) {
        LOGE(">>> Error: SP_GPIO_DRIVER_get_value export. %s", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    fprintf(fp, "%d", port);
    fclose(fp);

    sprintf(s, "/sys/class/gpio/gpio%d/direction", port);
    if((fp = fopen(s, "rb+")) == NULL) {
        LOGE(">>> Error: SP_GPIO_DRIVER_get_value port [%d] direction. %s", port, strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }
    fprintf(fp, "in");
    fclose(fp);

    sprintf(s1, "/sys/class/gpio/gpio%d/value", port);
    if((fp = fopen(s1, "rb+")) == NULL) {
        LOGE(">>> Error: SP_GPIO_DRIVER_get_value port [%d] value. %s", port, strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }


    int8_t buf = 0;
    size_t ret = read(fp, &buf, sizeof(buf));

    if((fp = fopen("/sys/class/gpio/unexport", "w")) == NULL) {
        LOGE(">>> Error: SP_GPIO_DRIVER_get_value unexport. %s", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    fprintf(fp, "%d", port);
    fclose(fp);

    LOGD("SP_GPIO_DRIVER_get_value: %d", buf);
    return (ret == 0 ? SC_GPIO_DRIVER_FAIL : buf);
}

