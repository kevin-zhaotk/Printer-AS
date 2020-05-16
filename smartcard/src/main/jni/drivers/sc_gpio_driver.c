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
        2020.3.20 取消ext-gpio的引用，改为通过shell进行访问
*********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "common_log.h"
#include "internal_ifc/sc_gpio_driver.h"

/*
*/
static int ioctlValue(int port) {
    int gpio_ioctl_value = 0;

    switch (port) {
        case GPIO_PORT_PG5:
            gpio_ioctl_value = GPIO_PIN_PG5_ENABLE;
            break;
        case GPIO_PORT_PG6:
            gpio_ioctl_value = GPIO_PIN_PG6_ENABLE;
            break;
        case GPIO_PORT_PG7:
            gpio_ioctl_value = GPIO_PIN_PG7_ENABLE;
            break;
        case GPIO_PORT_PG8:
            gpio_ioctl_value = GPIO_PIN_PG8_ENABLE;
            break;
        case GPIO_PORT_PG9:
            gpio_ioctl_value = GPIO_PIN_PG9_ENABLE;
            break;
    }

    return gpio_ioctl_value;
}

/*********************************************************************************
    SP_GPIO_DRIVER_set_value
----------------------------------------------------------------------------------
    @描述
        给GPIO端口设置值。
    @参数
        port: GPIO端口
        value: 设置值
    @返回值
        SC_GPIO_DRIVER_SUCCESS(0): 关闭成功
        SC_GPIO_DRIVER_FAIL(-1)：失败。错误信息保存在errno
**********************************************************************************/

int SP_GPIO_DRIVER_set_value(int port, uint8_t value) {
//    LOGI(">>> SP_GPIO_DRIVER_set_value[%d] to port[%d]", value, port);

    int fd = SC_GPIO_DRIVER_open(GPIO_DEVICE);

    if(fd == SC_GPIO_DRIVER_FAIL) {
        return SC_GPIO_DRIVER_FAIL;
    }

    int ret = SC_GPIO_DRIVER_ioctl(fd, GPIO_DEVICE_IOCTL_WRITE, ioctlValue(port) + (value & 0x01));

    SC_GPIO_DRIVER_close(fd);

    return ret;
/*
    FILE *fp;
    char s[50]="";
    char s1[50]="";

    if((fp = fopen("/sys/class/gpio/export", "w")) == NULL) {
        LOGE(">>> SP_GPIO_DRIVER_set_value error when open port [/sys/class/gpio/export]! [%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    fprintf(fp, "%d", port);
    fclose(fp);

    sprintf(s, "/sys/class/gpio/gpio%d/direction", port);

    if((fp = fopen(s, "wb+")) == NULL) {
        LOGE(">>> SP_GPIO_DRIVER_set_value error when set direction! [%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }
    fprintf(fp, "out");
    fclose(fp);

    sprintf(s1, "/sys/class/gpio/gpio%d/value", port);

    if((fp = fopen(s1, "wb+")) == NULL) {
        LOGE(">>> SP_GPIO_DRIVER_set_value error when set value! [%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    fprintf(fp, "%d", value);
    fclose(fp);
//    LOGD(">>> SP_GPIO_DRIVER_set_value set [%d] to port [%d], done!", value, port);

    return SC_GPIO_DRIVER_SUCCESS;
*/
}

/*********************************************************************************
    SP_GPIO_DRIVER_get_value
----------------------------------------------------------------------------------
    @描述
        从GPIO端口读取值。
    @参数
        port: GPIO端口
    @返回值
        value: 读取的值
        SC_GPIO_DRIVER_FAIL(-1)：失败。错误信息保存在errno
**********************************************************************************/

int SP_GPIO_DRIVER_get_value(int port) {
//    LOGI(">>> SP_GPIO_DRIVER_get_value from port [%d]", port);

    int fd = SC_GPIO_DRIVER_open(GPIO_DEVICE);

    if(fd == SC_GPIO_DRIVER_FAIL) {
        return SC_GPIO_DRIVER_FAIL;
    }

    int read = SC_GPIO_DRIVER_read(fd, 0);

    SC_GPIO_DRIVER_close(fd);

    return read;
/*
    uint8_t value;

    FILE *fp;
    char s[50]="";
    char s1[50]="";

    if((fp = fopen("/sys/class/gpio/export", "w")) == NULL) {
        LOGE(">>> SP_GPIO_DRIVER_get_value error when open port [/sys/class/gpio/export]! [%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    fprintf(fp, "%d", port);
    fclose(fp);

    sprintf(s, "/sys/class/gpio/gpio%d/direction", port);

    if((fp = fopen(s, "wb+")) == NULL) {
        LOGE(">>> SP_GPIO_DRIVER_get_value error when set direction! [%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }
    fprintf(fp, "in");
    fclose(fp);

    sprintf(s1, "/sys/class/gpio/gpio%d/value", port);

    if((fp = fopen(s1, "wb+")) == NULL) {
        LOGE(">>> SP_GPIO_DRIVER_get_value error when read value! [%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    size_t ret = fread(&value, sizeof(value), 1, fp);

    value &= 0x01;

//    LOGD(">>> SP_GPIO_DRIVER_get_value: %d", value);
    return (ret > 0 ? value : SC_GPIO_DRIVER_FAIL);
*/
}

/*********************************************************************************
    SC_GPIO_DRIVER_open(取消)
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
//    LOGI(">>> SC_GPIO_DRIVER_open: %s", dev);

    if(dev == NULL)	{
        LOGE(">>> SC_GPIO_DRIVER_open: [null device]");
        return SC_GPIO_DRIVER_FAIL;
    }

    int fd = open(dev, O_RDWR);

    if(fd == SC_GPIO_DRIVER_FAIL) {
        LOGE(">>> SC_GPIO_DRIVER_open: Error[%s]", strerror(errno));
    } else {
//        LOGD(">>> SC_GPIO_DRIVER_open: opened [%s] as [%d] ", dev, fd);
    }

    return fd;
}

/*********************************************************************************
    SC_GPIO_DRIVER_write（取消）
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
/*
int SC_GPIO_DRIVER_write(int fd, char *data_buf, int count) {
    char buf[1024];
    toHexString(data_buf, buf, count, ',');
    LOGI(">>> SC_GPIO_DRIVER_write: Write [%s](%d bytes)", buf, count);

    if(fd <= 0) {
        LOGE("Error: SC_GPIO_DRIVER_write - invalid device");
        return SC_GPIO_DRIVER_FAIL;
    }

    int ret = write(fd, data_buf, count);
    if(ret == SC_GPIO_DRIVER_FAIL) {
        LOGE("Error: SC_GPIO_DRIVER_write - %s", strerror(errno));
    } else {
//        LOGD("[%d] bytes written", ret);
    }

    return ret;
}
*/
/*********************************************************************************
    SC_GPIO_DRIVER_ioctl（取消）
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
//    LOGI(">>> SC_GPIO_DRIVER_ioctl: Command: 0x%02X; Value: [0x%04X]", cmd, arg1);

    if(fd < 0) {
        LOGE(">>> SC_GPIO_DRIVER_ioctl: Error[%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    uint8_t ret = ioctl(fd, cmd, arg1);
//	if(ret == EINVAL) {
    if(ret != SC_GPIO_DRIVER_SUCCESS) {
        LOGE(">>> SC_GPIO_DRIVER_ioctl: %s(Return value: 0x%02X)", strerror(errno), ret);
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
    SC_GPIO_DRIVER_read（取消）
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
//    LOGI(">>> SC_GPIO_DRIVER_read: Pin: %d", pin);

    if(fd <= 0) {
        LOGE(">>> SC_GPIO_DRIVER_read: Error[%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    uint8_t buf = 0;

    size_t ret = read(fd, &buf, sizeof(buf));
    if(ret == SC_GPIO_DRIVER_FAIL) {
        LOGE(">>> SC_GPIO_DRIVER_read: Error[%s]", strerror(errno));
    } else {
//        LOGD(">>> SC_GPIO_DRIVER_read: [0x%X] - %d bytes read", buf, ret);
    }

    return buf;
}

/*********************************************************************************
    SC_GPIO_DRIVER_close（取消）
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
//    LOGI(">>> SC_GPIO_DRIVER_close: %d", fd);

    if(fd <= 0) {
        LOGE(">>> SC_GPIO_DRIVER_close: Error[%s]", strerror(errno));
        return SC_GPIO_DRIVER_FAIL;
    }

    int ret = close(fd);
    if(ret != SC_GPIO_DRIVER_SUCCESS) {
        LOGE(">>> SC_GPIO_DRIVER_close: Error[%s]", strerror(errno));
    } else {
//        LOGD(">>> SC_GPIO_DRIVER_close: [%d] closed.", fd);
    }

    return ret;
}

