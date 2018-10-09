#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/sched.h>   //wake_up_process()    
#include <linux/kthread.h> //kthread_create()¡¢kthread_run()  
#include <linux/err.h> //IS_ERR()¡¢PTR_ERR()  
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <mach/platform.h>
#include <mach/gpio.h>
#include <asm/uaccess.h>

#define TRUE	1
#define FALSE	0

// #define DEBUG
#ifdef DEBUG
#define	PRINT(args...)	printk(KERN_ALERT args)
#else
#define	PRINT(args...)	do{}while(0)

#endif

/**/
// #define PAOMADENG_TEST
//#define GPIO_SPEED_TEST

#define _PRODUCT_HP_
#ifndef _PRODUCT_HP_
#define _PRODUCT_32_
#endif

/* 
* The  Data Buffer Size in Driver
* For 304 Point Header: Can Print 128000 columns
* For 128 Point Header: Can Print 327680 columns
* For 32 Point Header: Can Print 327680*4 columns
* Suppose That is Big Enough For Our Project
*/
#define TOTAL_BUFFER_SIZE 5*1024*1024

/* Write 32KB Data To FPGA Once */
#define SINGLE_WRITE_SET	24*2
#define LENGTH_PURGE_512	512
#define SINGLE_WRITE_32K	32*1024
#define SINGLE_WRITE_16K	16*1024


/*
* size = buffer.length * bold
* data = buffer[i/]
*/
struct data_buffer{
	char *buffer;
	int size;		/* total size of buffer data (the real data length * bold) */
	int pos;		/* current buffer position */
	int bold;		/* bold number: 1~9 */
	int bytes;		/* bytes per column of the buffer */
	bool isActive;	/* */
	struct data_buffer *prev;
	struct data_buffer *next;
};
static struct data_buffer kFpgaData;
static struct data_buffer gFpgaSettings;
static struct data_buffer kFpgaPurge;


struct fpga_pipe{
	char *data;
	unsigned long size;
	wait_queue_head_t outq;
};
struct fpga_pipe *fpga_devp;

MODULE_LICENSE("GPL");

#define ADDR_GPIOE_DATA		(SW_VA_PORTC_IO_BASE + 0XA0)
#define ADDR_GPIOG_DATA		(SW_VA_PORTC_IO_BASE + 0XE8)
#define ADDR_GPIOH_DATA		(SW_VA_PORTC_IO_BASE + 0X10C)
#define ADDR_GPIOI_DATA		(SW_VA_PORTC_IO_BASE + 0X130)

#define	BIT_SETTING_PG1		1
#define	BIT_SETTING_PG2		2


//ioctl cmd
enum{
	FPGA_CMD_SETTING	=0x01,
	FPGA_CMD_SENDDATA	=0x02,
	FPGA_CMD_SYNCDATA	=0x03,
	FPGA_CMD_STARTPRINT	=0x04,
	FPGA_CMD_STOPPRINT	=0x05,
	FPGA_CMD_CLEAN		=0x06,
};

enum{
	FPGA_STATE_OUTPUT	=0x00,
	FPGA_STATE_SETTING	=0x01,
	FPGA_STATE_RESERVE	=0x02,
	FPGA_STATE_CLEAN	=0x03,
	FPGA_STATE_KNOWN	=0x04,
	FPGA_STATE_PURGE	=0x05,
};


spinlock_t fpga_data_lock;
static bool kFpgaWritable=FALSE;
static struct semaphore gSempTransfer;
static int gType;//Êý¾ÝÀàÐÍ£¬ÉèÖÃ»ò´òÓ¡Êý¾Ý

static u32 handle1 = 0;
static u32 handle2 = 0;


static struct gpio gpio_array[]={
		//data bus 16bits
		{GPIOI(4), 	GPIOF_DIR_OUT, NULL},
		{GPIOI(5), 	GPIOF_DIR_OUT, NULL},
		{GPIOI(6), 	GPIOF_DIR_OUT, NULL},
		{GPIOI(7), 	GPIOF_DIR_OUT, NULL},
		{GPIOI(8), 	GPIOF_DIR_OUT, NULL},
		{GPIOI(9), 	GPIOF_DIR_OUT, NULL},
		{GPIOH(26), GPIOF_DIR_OUT, NULL},
		{GPIOH(27), GPIOF_DIR_OUT, NULL},
		{GPIOE(4), 	GPIOF_DIR_OUT, NULL},
		{GPIOE(5), 	GPIOF_DIR_OUT, NULL},
		{GPIOE(6), 	GPIOF_DIR_OUT, NULL},
		{GPIOE(7), 	GPIOF_DIR_OUT, NULL},
		{GPIOE(8), 	GPIOF_DIR_OUT, NULL},
		{GPIOE(9), 	GPIOF_DIR_OUT, NULL},
		{GPIOE(10), GPIOF_DIR_OUT, NULL},
		{GPIOE(11), GPIOF_DIR_OUT, NULL},

		//CS signal
		{GPIOG(3), 	GPIOF_DIR_OUT, NULL},
		//WR signal
		{GPIOG(0), 	GPIOF_DIR_OUT, NULL},

		//state signal
		{GPIOG(1),	GPIOF_DIR_OUT, NULL},
		{GPIOG(2),	GPIOF_DIR_OUT, NULL},

#if defined _PRODUCT_HP_
		//state - used for buzzle right now		
		// {GPIOG(4),	GPIOF_DIR_OUT, NULL},
		// {GPIOG(5),	GPIOF_DIR_IN, NULL},
#elif defined _PRODUCT_32_
		{GPIOG(4),	GPIOF_DIR_IN, NULL},
		{GPIOG(5),	GPIOF_DIR_IN, NULL},
#endif
	};

/*
* ÖÐ¶Ï´¦Àíº¯Êý
*/
void handler_first(void* arg);
void handler_segment(void* arg);

static int writeData(struct fpga_pipe *dev, struct data_buffer *data, int state, int length);



static void tWrite(struct device *dev,  
          struct device_attribute *attr,  
          __user const char *buf, size_t count){
	struct data_buffer data;
	data.buffer = (char*)vmalloc(count);
	data.size = count;
	data.pos = 0;
	down(&gSempTransfer);
	writeData(fpga_devp, &data, FPGA_STATE_OUTPUT, 2);
	up(&gSempTransfer);
}


static DEVICE_ATTR(tWrite, S_IRUGO | S_IWUSR, NULL, tWrite);

static struct attribute *fpga_sysfs_entries[] = {
	&dev_attr_tWrite.attr,
	NULL
};


static struct attribute_group fpga_attribute_group = {
	.name = NULL,		/* put in device directory */
	.attrs = fpga_sysfs_entries,
};


/*******************************
* FPGA
*******************************/


static void writeBit(unsigned long addr, int pin, char data, int bit)
{
	unsigned long regdata=0;
	regdata = readl(addr);

	if (data&(0x01<<bit))
	{
		regdata = regdata | (0x01 << pin);
	}
	else {
		regdata = regdata & (~(0x01 << pin));
	}
	writel(regdata, addr);
}

static void setState(int state)
{
	PRINT("===>setState: %d\n", state);
	switch(state)
	{
		case FPGA_STATE_OUTPUT:
			writeBit(ADDR_GPIOG_DATA, BIT_SETTING_PG1, FPGA_STATE_OUTPUT,	1);
			writeBit(ADDR_GPIOG_DATA, BIT_SETTING_PG2, FPGA_STATE_OUTPUT,	0);
			break;
		case FPGA_STATE_SETTING:
			writeBit(ADDR_GPIOG_DATA, BIT_SETTING_PG1, FPGA_STATE_SETTING,	1);
			writeBit(ADDR_GPIOG_DATA, BIT_SETTING_PG2, FPGA_STATE_SETTING,	0);
			break;
		case FPGA_STATE_RESERVE:
			writeBit(ADDR_GPIOG_DATA, BIT_SETTING_PG1, FPGA_STATE_RESERVE,	1);
			writeBit(ADDR_GPIOG_DATA, BIT_SETTING_PG2, FPGA_STATE_RESERVE,	0);
			break;
		case FPGA_STATE_CLEAN:
			writeBit(ADDR_GPIOG_DATA, BIT_SETTING_PG1, FPGA_STATE_CLEAN,	1);
			writeBit(ADDR_GPIOG_DATA, BIT_SETTING_PG2, FPGA_STATE_CLEAN,	0);
			break;
		default:
			break;
	}
}

static void setDataType(int type){
	if(type >= FPGA_STATE_OUTPUT && type <= FPGA_STATE_PURGE)
	{
		gType = type;
		// setState(type);
	}
}


static int writeData(struct fpga_pipe *dev, struct data_buffer *data, int state, int length)
{
	int size=0;
	int i=0;
	int position=0;
	int realPos = 1;
	u32 time=jiffies;
	unsigned long regdata=0; 
	// PRINT( "===>fpga_gpio write line:%d, size=%d\n",__LINE__, data->size);

	if(data->buffer == NULL){
		return -1;
	}
	//struct fpga_pipe *dev = filep->private_data;
	
	setState(state);
	
	//CS signal to low
	regdata = readl(ADDR_GPIOG_DATA);
	regdata = regdata & ~(0x08);
	writel(regdata,ADDR_GPIOG_DATA);
	udelay(1);
	
	//set WR signal to low
	regdata = readl(ADDR_GPIOG_DATA);
	regdata = regdata & ~(0x01);
	writel(regdata,ADDR_GPIOG_DATA);

	PRINT( "===>write size=%d, pos=%d\n", data->size, data->pos);
	
#if defined _PRODUCT_HP_
	if((data->size - data->pos) >= length)
	{
		size = length;
	}
	else
	{
		size = data->size - data->pos;
	}
#elif defined _PRODUCT_32_
	size = data->size;
#endif
	PRINT( "===>write real size=%d\n", size);
	PRINT("--->start position=%d,  realpos=%d\n", data->pos, (position/(data->bold*data->bytes) * data->bytes) + (position%data->bytes));

	for(i=0; i< size/2; i++)
	{
		// PRINT( "===>data[%d]=0x%x, data[%d]=0x%x\n", (data->pos + 2*i), data->buffer[data->pos + 2*i], (data->pos + 2*i+1), data->buffer[data->pos + 2*i+1]);
		if(gType == FPGA_STATE_OUTPUT) {
			position = data->pos + 2*i;
			realPos = (position/(data->bold*data->bytes) * data->bytes) + (position%data->bytes);
		} else {
			realPos = data->pos + 2*i;
		}
		
		regdata = readl(ADDR_GPIOI_DATA);
		regdata &= 0xfffffc0f;
		regdata |= ((data->buffer[realPos]<<4)&0x03f0);
		writel(regdata, ADDR_GPIOI_DATA);

		regdata = readl(ADDR_GPIOH_DATA);
		regdata &= 0xf3ffffff;
		regdata |= ((data->buffer[realPos]<<20)&0x0c000000);
		writel(regdata, ADDR_GPIOH_DATA);

		realPos = realPos + 1;
		regdata = readl(ADDR_GPIOE_DATA);
		regdata &= 0xfffff00f;
		regdata |= ((data->buffer[realPos] << 4) & 0x0ff0);
		writel(regdata, ADDR_GPIOE_DATA);

		//set WR signal high
		regdata = readl(ADDR_GPIOG_DATA);
		regdata = regdata | 0x01;
		writel(regdata,ADDR_GPIOG_DATA);
		//set WR signal low	
		regdata = readl(ADDR_GPIOG_DATA);
		regdata = regdata & ~(0x01);
		writel(regdata,ADDR_GPIOG_DATA);
	
	}
	udelay(1);
	//CS signal to high
	regdata = readl(ADDR_GPIOG_DATA);
	regdata = regdata | 0x08;
	writel(regdata,ADDR_GPIOG_DATA);
#if defined _PRODUCT_HP_
	data->pos += size;
	//PRINT( "===>write pos=%d, size=%d\n", data->pos, data->size);
	if(data->pos >= data->size-1)
	{
		kFpgaWritable = TRUE;
		PRINT( "===>writeable true\n");
		wake_up(&dev->outq);
	}
#elif defined _PRODUCT_32_
	kFpgaWritable = TRUE;
	wake_up(&dev->outq);
#endif
	PRINT("--->position=%d,  realpos=%d\n", position, realPos);

	return size;
}


struct task_struct *kStatePollThd = NULL;
static bool gFpgaStateRunning = FALSE;
	
static int fpga_state_poll_thread(void * pArg)
{
	struct file *fp = pArg;
	for(;gFpgaStateRunning == TRUE;)
	{
		set_current_state(TASK_UNINTERRUPTIBLE);
		if(kthread_should_stop())
			break;
		unsigned long regData = readl(ADDR_GPIOG_DATA);
		/*
		* GPIOG4=1&&GPIOG5=1
		*/
		if((regData & 0x30) == 0x30)
		{
			PRINT( "===>poll thread\n");
			down(&gSempTransfer);
			writeData(fpga_devp, &kFpgaData, FPGA_STATE_OUTPUT, SINGLE_WRITE_16K);
			up(&gSempTransfer);
			do {
				if(kthread_should_stop())
					break;
				regData = readl(ADDR_GPIOG_DATA);
				if((regData & 0x30) == 0x30) {
					mdelay(5);
				} else {
					break;
				}
				
			}while(gFpgaStateRunning == TRUE);
		}
		else
		{
			schedule_timeout(1);
		}
	}
}

static void startPollThread(struct file *filep) 
{
	kStatePollThd = kthread_create(fpga_state_poll_thread, filep, "fpga-poll"); 
	if(IS_ERR(kStatePollThd))
	{
		PRINT( "*****start fpga state poll thread err");
	}
	gFpgaStateRunning = TRUE;
	wake_up_process(kStatePollThd);
}

static void stopPollThread()
{
	if(kStatePollThd == NULL)
	{
		return ;
	}
	kthread_stop(kStatePollThd);
	gFpgaStateRunning = FALSE;
	kStatePollThd = NULL;
}


static int startFirst(void * pArg)
{
	handler_first(NULL);
}


/*
*
*/
static void startPrint() {
	// struct task_struct *kthread = kthread_create(startFirst, filep, "fpga-poll"); 
	// wake_up_process(kthread);
	handle1 = sw_gpio_irq_request(GPIOH(14),TRIG_EDGE_POSITIVE, (peint_handle)handler_first,"GPIOH14");
	handle2 = sw_gpio_irq_request(GPIOH(15),TRIG_EDGE_POSITIVE, (peint_handle)handler_segment,"GPIOH15");
	PRINT( "===>handle1=0x%x, handle2=0x%x\n", handle1, handle2);
	setState(FPGA_STATE_OUTPUT);
}

static void stop() {
	sw_gpio_irq_free(handle1);
	sw_gpio_irq_free(handle2);
	handle1 = -1;
	handle2 = -1;
}

int fpga_gpio_open(struct inode *inode, struct file *filep)
{
	PRINT( "===>fpga_gpio open\n");
	struct fpga_pipe *dev = fpga_devp;
	filep->private_data = dev;
	return 0;
}

ssize_t fpga_gpio_read(struct file *filep, char __user *buff, size_t count, loff_t *offset)
{
	PRINT( "===>fpga_gpio read\n");
	return 0;
}

/*
* ÏòFPGAÐ´ÈëÊý¾Ý
* count: Ð´ÈëÊý¾ÝµÄË«×Ö½ÚÊý
* µ¥´ÎÐ´ÈëÊý¾ÝµÄ×î´ó³¤¶ÈÎª2M
* Èç¹ûÊý¾Ý³¤¶È³¬¹ý2M,ÔòÍ¨¹ýÑ­»·ÊµÏÖ
* PG0  
	¡°WRÐÅºÅ¡±£¬ Æ½Ê±ÎªµÍ£¬ Ã¿´ÎÐ´Êý¾ÝÊ±Îª¸ß¡£ÆµÂÊÓëÊý¾ÝÍ¬²½£¬¿ÉÒÔÓÐÎó²î¡£ 

*Êý¾ÝÔÚ´ËÐÅºÅ ÉÏÉýÑØËÍ¡£ 

* PG3
*	¡°CRÐÅºÅ¡±£¬  Æ½Ê±ÎªµÍ£¬ Ã¿´ÎÐ´ÊýÎª¸ß¡£ 
*ÓÃÓÚÃ¿×éÊý¾ÝµÄ ±£»¤£¬  ±ÈÈçÐ´100 ¸ö 16bit¡£CR ÒªÕû¸ö±§×Å100 ¸öWR¡£
*/
ssize_t fpga_gpio_write(struct file *filep, const char __user *buff, size_t count, loff_t *offset)
{
	size_t i=0, size=-1; 
	if(kFpgaData.buffer == NULL)
	{
		/* Init The Data Buffer*/
		memset(&kFpgaData, 0, sizeof(kFpgaData));
		kFpgaData.buffer = vmalloc(TOTAL_BUFFER_SIZE);
		if(kFpgaData.buffer == NULL)
		{
			printk(KERN_ERR "fpga vmalloc failed");
			return -1;
		}	
	}
	if(gFpgaSettings.buffer == NULL)
	{
		/* Init The Data Buffer*/
		memset(&gFpgaSettings, 0, sizeof(gFpgaSettings));
		gFpgaSettings.buffer = vmalloc(24*4);
		if(gFpgaSettings.buffer == NULL)
		{
			printk(KERN_ERR "fpga setting vmalloc failed");
			return -1;
		}
	}
	if(kFpgaPurge.buffer == NULL)
	{
		/* Init The Data Buffer*/
		memset(&kFpgaPurge, 0, sizeof(kFpgaPurge));
		kFpgaPurge.buffer = vmalloc(LENGTH_PURGE_512);
		if(kFpgaPurge.buffer == NULL)
		{
			printk(KERN_ERR "fpga purge vmalloc failed");
			return -1;
		}
	}

	if(gType == FPGA_STATE_SETTING) //Settings
	{
		down(&gSempTransfer);
		memset(gFpgaSettings.buffer, 0, sizeof(gFpgaSettings.buffer));
		copy_from_user(gFpgaSettings.buffer, buff, count);
		gFpgaSettings.size = count;
		gFpgaSettings.pos = 0;
		PRINT( "===>Settings\n");
		writeData(fpga_devp, &gFpgaSettings, gType, SINGLE_WRITE_SET);
		PRINT( "===>Settings write ok\n");
		up(&gSempTransfer);
		
		return count;
	}
	else if(gType == FPGA_STATE_CLEAN)
	{
		down(&gSempTransfer);
		setState(gType);
		up(&gSempTransfer);
	}	
	else if(gType == FPGA_STATE_OUTPUT)//printing data
	{
		down(&gSempTransfer);
		memset(kFpgaData.buffer, 0, sizeof(kFpgaData.buffer));
		copy_from_user(kFpgaData.buffer, buff, count);
		PRINT("===>write data: bold=%d, %d\n", gFpgaSettings.buffer[30], gFpgaSettings.buffer[31]);
		if(gFpgaSettings.buffer[15*2] <= 0 || gFpgaSettings.buffer[15*2] > 9) {
			kFpgaData.bold = 1;
		} else {
			kFpgaData.bold = gFpgaSettings.buffer[15*2];
		}
		kFpgaData.size = count * kFpgaData.bold;
		kFpgaData.pos = 0;
		kFpgaData.bytes = gFpgaSettings.buffer[24*2];
		up(&gSempTransfer);
		PRINT("--->bold= %d, size=%d\n", kFpgaData.bold, kFpgaData.size);
		PRINT("--->bytes=%d\n", kFpgaData.bytes);
		kFpgaWritable = FALSE;
	}
	else if (gType == FPGA_STATE_PURGE) 
	{
		down(&gSempTransfer);
		memset(kFpgaPurge.buffer, 0, sizeof(kFpgaPurge.buffer));
		copy_from_user(kFpgaPurge.buffer, buff, count);
		kFpgaPurge.size = count;
		kFpgaPurge.bold = 1;
		kFpgaPurge.pos = 0;
		kFpgaPurge.bytes = gFpgaSettings.buffer[24];
		PRINT( "===>purge size=%d\n", kFpgaPurge.size);
		writeData(fpga_devp, &kFpgaPurge, FPGA_STATE_OUTPUT, count);
		up(&gSempTransfer);
	}
	else {
		PRINT( "--->Fuck!!! you should set the data type first\n");
	}
	

#ifdef PAOMADENG_TEST
	down(&gSempTransfer);
	writeData(fpga_devp, &kFpgaData, FPGA_STATE_OUTPUT);
	up(&gSempTransfer);
#endif
	return count;
}

/*
* Steps:
* 1. invoke write method writing data to driver buffer, data is ready
* 2. invoke ioctl method for starting print
* 3. Triger the interrupt_handler after starting 
*/	
void fpga_gpio_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
		case FPGA_CMD_SETTING:
			setDataType(arg);
			break;
		case FPGA_CMD_SENDDATA:
			break;
		case FPGA_CMD_SYNCDATA:
			break;
		case FPGA_CMD_STARTPRINT:
			kFpgaWritable = FALSE;
			gFpgaStateRunning = TRUE;
#if defined _PRODUCT_HP_
			startPrint();
#elif defined _PRODUCT_32_
			startPollThread(filep);
#endif
			break;
		case FPGA_CMD_STOPPRINT:
#if defined _PRODUCT_HP_
			stop();
#elif defined _PRODUCT_32_
			stopPollThread();
#endif
			kFpgaWritable = FALSE;
			gFpgaStateRunning = FALSE;
			break;
		case FPGA_CMD_CLEAN:
			setState(FPGA_STATE_CLEAN);
		default:
			break;
	}
	PRINT( "--->ioctl ok\n");
}

static unsigned int fpga_gpio_poll(struct file *filp,poll_table *wait)
{
	struct fpga_pipe *dev =filp->private_data;
	unsigned int mask =0;
	/*poll_table */
	poll_wait(filp,&dev->outq,wait);
	/*·µ»ØÑÚÂë*/
	
	if (kFpgaWritable == TRUE)
		mask = POLLOUT |POLLWRNORM;/*Éè±¸¿É¶Á*/
	return mask;
}


struct file_operations fpga_fops={
	.owner = THIS_MODULE,
	.open = fpga_gpio_open,
	.read = fpga_gpio_read,
	.write = fpga_gpio_write,
	.unlocked_ioctl = fpga_gpio_ioctl,
	.poll = fpga_gpio_poll,
};

struct miscdevice fpga_gpio_dev={
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fpga-gpio",
	.fops = &fpga_fops,
	.mode = 0x766,
};


static int fpga_gpio_probe(struct platform_device* dev)
{
	int result;
	PRINT( "===> fpga_gpio probe, %d\n",sizeof(char));
	misc_register(&fpga_gpio_dev);

	/*init wait queue*/
	fpga_devp = kmalloc(sizeof(struct fpga_pipe), GFP_KERNEL);
	if(!fpga_devp)
	{
		result = -ENOMEM;
		goto fail_probe;
	}
	init_waitqueue_head(&fpga_devp->outq);
	int error = gpio_request_array(&gpio_array,ARRAY_SIZE(gpio_array));
	if(error != 0)
	{
		PRINT( "===> fpga_gpio probe request=%d\n",error);
		result = -ENODEV;
		goto fail_probe;
	}
	/* set CS=1 after boot */
	__gpio_set_value(GPIOG(3),1);
	/* Init The Data Buffer*/
	memset(&kFpgaData, 0, sizeof(kFpgaData));

	/*malloc buffers list*/
	kFpgaData.buffer = (char *)vmalloc(TOTAL_BUFFER_SIZE * sizeof(char));
	
	if(kFpgaData.buffer == NULL)
	{
		printk(KERN_ERR "fpga vmalloc failed");
		goto fail_probe;
	}
	

	/* Init The Data Buffer*/
	memset(&gFpgaSettings, 0, sizeof(gFpgaSettings));
	
	gFpgaSettings.buffer = (char*)vmalloc(24*4);
	if(gFpgaSettings.buffer == NULL)
	{
		printk(KERN_ERR "fpga setting vmalloc failed");
		goto settings_alloc_fail;
	}

	/* Init The Purge Buffer*/
	memset(&kFpgaPurge, 0, sizeof(kFpgaPurge));
	
	kFpgaPurge.buffer = (char*)vmalloc(LENGTH_PURGE_512);
	if(kFpgaPurge.buffer == NULL)
	{
		printk(KERN_ERR "fpga setting vmalloc failed");
		goto settings_alloc_fail;
	}
	/**/
	spin_lock_init(&fpga_data_lock);
	sema_init(&gSempTransfer, 1);
	gType = FPGA_STATE_KNOWN;
	setState(FPGA_STATE_CLEAN);

#if defined _PRODUCT_HP_
	/* Irq request, Irq enable when start print instead of probe stage */
	// handle1 = sw_gpio_irq_request(GPIOH(14),TRIG_EDGE_POSITIVE, (peint_handle)handler_first,"GPIOH14");
	// handle2 = sw_gpio_irq_request(GPIOH(15),TRIG_EDGE_POSITIVE, (peint_handle)handler_segment,"GPIOH15");
#endif
	/*buzzle - high level work*/
	PRINT( "===>handle1=0x%x, handle2=0x%x\n", handle1, handle2);
	return 0;
settings_alloc_fail:
	vfree(kFpgaData.buffer);
	kFpgaData.buffer = NULL;
fail_probe:
	misc_deregister(&fpga_gpio_dev);
	return result;
}

void handler_first(void* arg)
{
 	if(gFpgaStateRunning == FALSE)
 	{
 		PRINT( "11 print not start\n");
 		return;
 	}
	// spin_lock(&fpga_data_lock);
	// unsigned long regdata = readl(ADDR_GPIOH_DATA);
	PRINT( "===>PH14 interrupt\n");
	// if(regdata & 0x0C000 == 0x0C000)
	// {
	down(&gSempTransfer);
	writeData(fpga_devp, &kFpgaData, FPGA_STATE_OUTPUT, SINGLE_WRITE_16K);
	up(&gSempTransfer);
	// }
	// spin_unlock(&fpga_data_lock);
}

//static int isFirst32K = 0;

void handler_segment(void* arg)
{
	//PRINT( "22\n");
 	if(gFpgaStateRunning == FALSE)
 	{
 		return;
 	}
	if(kFpgaData.pos == 0) {
		return;
	}
/*
	if (isFirst32K==1 && kFpgaData.pos == SINGLE_WRITE_16K * 2 -1) 
	{
		kFpgaPurge.buffer[0] = 0x0ff;
		kFpgaPurge.buffer[1] = 0x0ff;
		kFpgaPurge.buffer[2] = 0x0ff;
		kFpgaPurge.buffer[3] = 0x0ff;
		kFpgaPurge.size = 4;
		kFpgaPurge.pos = 0;
		kFpgaPurge.bold = 1;
		writeData(fpga_devp, &kFpgaPurge, FPGA_STATE_OUTPUT, 4);
		isFirst32K = 1;
		return;
	}*/
	// spin_lock(&fpga_data_lock);
	// unsigned long regdata = readl(ADDR_GPIOH_DATA);
	// PRINT( "===>PH14&PH15=0x%x\n", regdata&0x0C000);
	// if(regdata & 0x0C000 == 0x0C000)
	// {
	PRINT( "===>PH15 interrupt\n");
	down(&gSempTransfer);
	writeData(fpga_devp, &kFpgaData, FPGA_STATE_OUTPUT, SINGLE_WRITE_16K);
	up(&gSempTransfer);
	// }
	// spin_unlock(&fpga_data_lock);
}


static int fpga_gpio_remove(struct platform_device* dev)
{
	struct data_buffer *data=NULL;

	PRINT( "===>fpga_gpio remove\n");
	gpio_free_array(&gpio_array, ARRAY_SIZE(gpio_array));
	sw_gpio_irq_free(handle1);
	sw_gpio_irq_free(handle2);	
	/*release buffers*/
	if(kFpgaData.buffer != NULL)
	{
		vfree(kFpgaData.buffer);
		kFpgaData.buffer = NULL;
	}

	return 0;
}

static int fpga_gpio_suspend(struct platform_device *dev, pm_message_t state)
{
	PRINT( "===>fpga_gpio suspend\n");
	return 0;
}

static int fpga_gpio_resume(struct platform_device* dev)
{
	PRINT( "===>fpga_gpio resume\n");
	return 0;
}

struct platform_device fpga_device={
	.name = "fpga-gpio",
	.id = -1,
};

struct platform_driver fpga_driver={
	.probe = fpga_gpio_probe,
	.remove = fpga_gpio_remove,
	.suspend = fpga_gpio_suspend,
	.resume = fpga_gpio_resume,
	.driver={
		.name = "fpga-gpio",
		.owner = THIS_MODULE,
	},
};

static int __init fpga_init(void)
{
	int ret=0;
	if(platform_device_register(&fpga_device))
	{
		PRINT( "register fpga_gpio device failed\n");
		goto init_fail;
	}
	if(platform_driver_register(&fpga_driver))
	{
		platform_device_unregister(&fpga_device);
	}
#ifdef _PRODUCT_HP_
	//kFpgaData.buffer = vmalloc(TOTAL_BUFFER_SIZE);
	ret = sysfs_create_group(&fpga_device.dev.kobj,
			&fpga_attribute_group);
#endif

	return 0;
init_fail:
	return -1;
}

static int __exit fpga_exit(void)
{
	platform_driver_unregister(&fpga_driver);
	platform_device_unregister(&fpga_device);
	kfree(fpga_devp);
}	


module_init(fpga_init);
module_exit(fpga_exit);

MODULE_AUTHOR("Kevin Zhao");



