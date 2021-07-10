/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

/** \file file.h
 * Wrapper over the file system on OEM environment.
 */


#ifndef __FILE_H__
#define __FILE_H__

#include "max_common_types.h"

/** 
 * Open file for reading. 
 * Returns Handle to opened file on success. NULL on failure
 * @param file_name Name of the file to open for reading.
 */
void *file_open(const char *file_name);

/**
 * Close file.
 * @param file_handle Handle to file to be closed.
 */
int file_close(void *file_handle);

/**
 * Read the specified number of bytes from the given file handle
 * to buf. Returns the number of bytes read.
 * @param file_handle   File handle to read from.
 * @param buf           Buffer where data read from file is copied. Memory to be allocated by caller.
 * @param size          Size of buf in bytes.
 */
int file_read(void *file_handle, uint8_t *buf, size_t size);

/** 
 * Read one line from the given file handle to buf.
 * @param file_handle   File handle to read from.
 * @param buf           Buffer where data read from file is copied. Memory to be allocated by caller.
 * @param size          Size of buf in bytes.
 */
int file_read_line(void *file_handle, char *buf, size_t size);

#endif
