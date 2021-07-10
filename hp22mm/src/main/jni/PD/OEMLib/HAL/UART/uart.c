/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/
#define _BSD_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>         /* Used for UART */
#include <fcntl.h>          /* Used for UART */
#include <termios.h>        /* Used for UART */
#include <sys/select.h>
#include <errno.h>
//#include <wiringPi.h>


#include "uart_ifc.h"           /* UART interface */
#include "max_common_types.h"
#include "debug_log.h"
#include "lock.h"

#define MAX_TX_SIZE         507//270
#define MSG_SIZE            64
#define SPLIT_THRESHOLD     256
#define SPLIT_SIZE          128

#define UART_MUX_SEL_PIN    7       /* Wiring Pi = 7, BCM = 4 */

#ifndef UART_DEVICE_NAME
#define UART_DEVICE_NAME    "/dev/serial0"
#endif

#ifndef UART_BAUD_RATE
/*
 * Supported baud rates by termios library are,
    B0
    B50
    B75
    B110
    B134
    B150
    B200
    B300
    B600
    B1200
    B1800
    B2400
    B4800
    B9600
    B19200
    B38400
    B57600
    B115200
    B230400
*/
#define UART_BAUD_RATE      B115200
#endif

//static const char *UART_MODULE_NAME = "UART";
//#define UART_DEBUG_LOG(level, ...) DEBUG_LOG(UART_MODULE_NAME, level, __VA_ARGS__)

typedef struct
{
    int fs;
} UartHandle_t;

#ifndef NUM_BLUR_INSTANCES
    #error "NUM_BLUR_INSTANCES should be defined in max_config.h"
#endif

#if (NUM_BLUR_INSTANCES < 0) || (NUM_BLUR_INSTANCES > 16)
    #error "NUM_BLUR_INSTANCES should be between 1 and 16"
#endif

//static UartHandle_t uart_handle[NUM_BLUR_INSTANCES];

static UartHandle_t uart_handle;
static bool         is_lib_initialized = false;
static bool         is_uart_initialized = false;

static Lock_t       shared_lock;

/*
 * This function should be called to initialize the UART library before
 * any other UART functions can be invoked.
 */
UartResult_t uart_lib_init()
{
    if(true == is_lib_initialized) return UART_OK;

    /*
    int i = 0;
    for(i = 0; i < NUM_BLUR_INSTANCES; i++)
    {
        uart_handle[i].fs = -1;
    }*/

    // initialize simple uart mutex
    mutex_init(&shared_lock);

    is_lib_initialized = true;

    return UART_OK;
}

UartResult_t uart_lib_shutdown()
{
    if(false == is_lib_initialized) return UART_OK;

    is_lib_initialized = true;

    mutex_destroy(&shared_lock);
    return UART_OK;
}


UartResult_t uart_lock()
{
    // lock simple mutex
    mutex_lock(&shared_lock);
    return UART_OK;
}

UartResult_t uart_unlock()
{
    // unlock simple mutex
    mutex_unlock(&shared_lock);
    return UART_OK;
}



UartResult_t uart_init(int32_t instance)
{
    if(instance <= 0) return UART_ERROR;
    if(instance > NUM_BLUR_INSTANCES) return UART_ERROR;

    // Already initialized, just return.
    //
    // In the dev kit implementation, we only have one physical UART.
    //
    //if(uart_handle[instance-1].fs != -1) return UART_OK;

    UartHandle_t *handle = &uart_handle;

    handle->fs = open(UART_DEVICE_NAME, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
    if (handle->fs == -1)
    {
        /* Failed to open UART */
        char msg[MSG_SIZE];
        snprintf(msg, MSG_SIZE, "uart_init() : Unable to open UART device : %s : ", UART_DEVICE_NAME);
        perror(msg);
        return UART_ERROR_NO_DEVICE;
    }

    /* Configure UART
       The flags are defined in termios.h -
       see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html
     */
    struct termios options;
    tcgetattr(handle->fs, &options);
    options.c_cflag = UART_BAUD_RATE | CS8 | CLOCAL | CREAD;
    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(handle->fs, TCIFLUSH);
    tcsetattr(handle->fs, TCSANOW, &options);

    if(true == is_uart_initialized) return UART_OK;

    /* Initialize the WiringPi */
/*    if(wiringPiSetup() == -1)
    {
        char msg[MSG_SIZE];
        snprintf(msg, MSG_SIZE, "uart_init() : Failed to initialize wiringPi");
        perror(msg);
        return UART_ERROR;
    }
    pinMode(UART_MUX_SEL_PIN, OUTPUT);
*/
    is_uart_initialized = true;

    return UART_OK;
}

/*
 * This function will close the underlying uart connection and
 * cleanup the data structures.
 */
UartResult_t uart_shutdown(int32_t instance)
{
    if(false == is_uart_initialized) return UART_OK;

    is_uart_initialized = false;

    if(instance <= 0) return UART_ERROR;
    if(instance > NUM_BLUR_INSTANCES) return UART_ERROR;
    if(uart_handle.fs == -1) return UART_ERROR;

    //UartHandle_t *handle = &uart_handle[instance-1];
    UartHandle_t *handle = &uart_handle;

    close(handle->fs);
    handle->fs = -1;

    return UART_OK;
}

/* Select the UART based on the instance */
UartResult_t uart_select_mux(int32_t instance)
{
//    if(instance == 2)   digitalWrite (UART_MUX_SEL_PIN, HIGH); /* Select IDS */
//    else                digitalWrite (UART_MUX_SEL_PIN, LOW);  /* Select PD  */

    return UART_OK;
}

/*
 * send data to uart.
 *
 * data - Pointer to buffer containing the data. Allocated by caller.
 * size - Size of the data to be transmited in bytes
 */
UartResult_t uart_send(int32_t instance, uint8_t *data, size_t size)
{
    if(instance <= 0) return UART_ERROR;
    if(instance > NUM_BLUR_INSTANCES) return UART_ERROR;
    if(uart_handle.fs == -1) return UART_ERROR;

    if(NULL == data) return UART_ERROR;
    if(size <= 0) return UART_ERROR;

    UartHandle_t *handle = &uart_handle;

    size_t transmitted = 0;

    if(handle->fs   == -1           ) return UART_ERROR_NOT_INITIALIZED;
    if(data         == NULL         ) return UART_ERROR;
    if(size         < 0             ) return UART_ERROR;
    if(size         > MAX_TX_SIZE   ) return UART_ERROR;

    /* Control the UART MUX on PDG */
    uart_select_mux(instance);

    /* Flush the input buffer incase we have some unwanted data */
    tcflush(handle->fs, TCIFLUSH);

    if( size < SPLIT_THRESHOLD )
    {
        /* send the data */
        transmitted = write(handle->fs, data, size);
        if(transmitted != size)
        {
            LOGD("Bytes transmitted = %d\n", transmitted);
            return UART_ERROR;
        }

    }else{

        //uint8_t * dcpy = data;
        size_t splitsize = (size_t)SPLIT_SIZE;
        int residual_size = (int) size;

        /* send the data */

        while (residual_size >= SPLIT_SIZE)
        {
           LOGD("residual size = %d \n", residual_size);

           transmitted = write(handle->fs, data, SPLIT_SIZE);
           if(transmitted != splitsize)
           {
 //            UART_DEBUG_LOG(DEBUG_LEVEL_DEBUG, "Bytes transmitted = %d\n", transmitted);
                LOGD("Bytes transmitted = %d\n", transmitted);
                return UART_ERROR;
           }
           usleep(50*1000);//50ms delay
          residual_size = residual_size - SPLIT_SIZE;
          data = data + SPLIT_SIZE;
        }

         if (residual_size > 0)
        {
        /* send the data */
         //  dcpy = data + SPLIT_SIZE;
           splitsize = (size_t)residual_size;//(resisize - splitsize) ;
           transmitted = write(handle->fs, data, splitsize);
            if(transmitted != splitsize)
            {
    //            UART_DEBUG_LOG(DEBUG_LEVEL_DEBUG, "Bytes transmitted = %d\n", transmitted);
                LOGD("Bytes transmitted = %d\n", transmitted);
                return UART_ERROR;
            }
        }
    }

    /****************************************************************
     * IMPORTANT NOTE : If the micro controller firmware depends on
     * line break to detect end of frame/packet, we need to uncomment
     * the line below to send the break.
     */
    /* Send break */
    /* tcsendbreak(handle->fs, 0); */
    /****************************************************************/

    return UART_OK;
}

/*
 * Receive data from uart. This is a blocking call. This function will
 * wait until specified number of bytes are received or times out.
 *
 * buf - Pointer to buffer where data to be filled. Allocated by caller.
 * buf_size - number of bytes to read.
 * recvd-size - Number of bytes recieved.
 * timeout - time out in milli seconds to wait. A value of "0" means do
 *           no block. read and return the data available in the uart input
 *           fifo. A value of "-1" indicates wait forever.
 *
 */
UartResult_t uart_recv( int32_t         instance,
                        uint8_t         *buf,
                        size_t          buf_size,
                        size_t          *recvd_size,
                        int32_t         timeout_ms)
{
    if(instance <= 0) return UART_ERROR;
    if(instance > NUM_BLUR_INSTANCES) return UART_ERROR;
    if(uart_handle.fs == -1) return UART_ERROR;

    if(NULL == buf) return UART_ERROR;
    if(NULL == recvd_size) return UART_ERROR;

    UartHandle_t *handle = &uart_handle;

    fd_set          rfds;
    struct timeval  tv;
    int             retval;

    *recvd_size = 0; /* Initalize received size to "0" */

    /* Wait for the data to be ready or till it timeouts */
    FD_ZERO(&rfds);
    FD_SET(handle->fs, &rfds);

    /*
     * Convert the time out in milli seconds to seconds and microseconds
     */
    tv.tv_sec   = 0;
    tv.tv_usec  = 0;
    if(timeout_ms > 1000)
    {
        tv.tv_sec   = timeout_ms / 1000;
        timeout_ms -= timeout_ms / 1000;
    }
    tv.tv_usec = timeout_ms * 1000;


    while(1)
    {
        size_t recieved = 0;

        retval = select(handle->fs + 1, &rfds, NULL, NULL, &tv);
        if(retval == -1)
        {
            /* If the select call was interrupted due to a signal,
             * call select() again.
             */
            if(errno == EINTR)  continue;
            else return UART_ERROR;
        }
        else if (retval)
        {
            /* data is available to read */
            recieved = read(handle->fs, (void *)(buf + *recvd_size), (buf_size - *recvd_size));
            if(recieved < 0)
            {
                if(errno == EINTR) continue;

                return UART_ERROR;
            }

            *recvd_size += recieved;
            LOGD("total no of bytes read = %d\n", *recvd_size);

            if(*recvd_size >= buf_size) return UART_OK;
        }
        else /* = 0 */
        {
            return UART_ERROR_TIMEOUT;
        }
    }

    return UART_ERROR;
}

/*
 * Receive data from uart. This is a non-blocking call and will return
 * immediately. When the specified number of bytes are received,
 * caller will be notified through callback function.
 *
 * data - Pointer to buffer where data to be filled. Allocated by caller.
 * size - number of bytes to read.
 * timeout - time out in milli seconds to wait. A value of "0" means do
 *           not block. read and return the data available in the uart input
 *           fifo. A value of "-1" indicates wait forever.
 *
 */
UartResult_t uart_recv_callback(int32_t instance, uint8_t *data,
                                size_t size, int32_t timeout,
                                UartCallbackFunc *func)
{
    if(instance <= 0) return UART_ERROR;
    if(instance > NUM_BLUR_INSTANCES) return UART_ERROR;
    if(uart_handle.fs == -1) return UART_ERROR;

    return UART_ERROR_NOT_IMPLEMENTED;
}



