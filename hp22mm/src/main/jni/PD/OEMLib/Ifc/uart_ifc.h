/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

/** \file uart_ifc.h
 * Interface to UART IO layer.
 */
 
#ifndef __UART_IFC_H__
#define __UART_IFC_H__

#include "max_common_types.h"

/**
 * @brief UART library API return values.
 */
typedef enum 
{
    UART_OK                     =  0,   /**< Success */
    UART_ERROR                  = -1,   /**< Error */
    UART_ERROR_TIMEOUT          = -2,   /**< Receive request timed out. */
    UART_ERROR_NO_DEVICE        = -3,   /**< No device found */
    UART_ERROR_NOT_IMPLEMENTED  = -4,   /**< This API is not implemented */
    UART_ERROR_NOT_INITIALIZED  = -5,   /**< UART is not initialized. */
} UartResult_t;

/* Callback function pointer definition */
typedef void (*UartCallbackFunc)(UartResult_t result);

/**
 * This function should be called to initialize the UART library before
 * any other UART functions can be invoked.
 */
UartResult_t uart_lib_init();

/**
 * This function should be called to shut down UART library.
 */
UartResult_t uart_lib_shutdown();

/**
 * Initialize the UART instance. open the device, configure 
 * the UART settings etc.
 * @param instance  Which UART instance to initialize.
 */
UartResult_t uart_init(int32_t instance);

/** 
 * This function will close the underlying uart connection and
 * cleanup the data structures.
 * @param instance  Which UART instance to close.
 */
UartResult_t uart_shutdown(int32_t instance);

UartResult_t uart_lock();

UartResult_t uart_unlock();

/**
 * send data to uart.
 *
 * @param data  Pointer to buffer containing the data. Allocated by caller.
 * @param size  Size of the data to be transmitted in bytes
 */
UartResult_t uart_send(int32_t instance, uint8_t *data, size_t size);

/**
 * Receive data from uart. This is a blocking call. This function will
 * wait until specified number of bytes are received or times out.
 *
 * @param buf           Pointer to buffer where data to be filled. Allocated by caller.
 * @param buf_size      Number of bytes to read.
 * @param recvd-size    Number of bytes received.
 * @param timeout       Time out in milli seconds to wait. A value of "0" means 
 *                      no block. read and return the data available in the uart input 
 *                      fifo. A value of "-1" indicates wait forever.
 */
UartResult_t uart_recv( int32_t         instance, 
                        uint8_t         *buf, 
                        size_t          buf_size, 
                        size_t          *recvd_size,
                        int32_t         timeout_ms);

/**
 * Receive data from uart. This is a non-blocking call and will return 
 * immediately. When the specified number of bytes are received, 
 * caller will be notified through callback function. 
 *
 * @param data      Pointer to buffer where data to be filled. Allocated by caller.
 * @param size      Number of bytes to read.
 * @param timeout   Time out in milli seconds to wait. A value of "0" means do 
 *                  not block. read and return the data available in the UART input
 *                  fifo. A value of "-1" indicates wait forever.
 *
 */
UartResult_t uart_recv_callback(int32_t instance, uint8_t *data, 
								size_t size, int32_t timeout, 
                                UartCallbackFunc *func);


#endif /* __UART_IFC_H__ */
