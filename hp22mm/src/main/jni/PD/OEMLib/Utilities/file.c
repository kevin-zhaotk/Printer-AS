/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "max_common_types.h"
#include "debug_log.h"

/* 
 * Open file for reading
 * Returns handle to opened file on success. 
 * Returns NULL on failure
 */
void *file_open(const char *file_name) {
    if(NULL == file_name) {
        LOGE("file_name NULL");
        return NULL;
    }
    
    FILE *fptr = NULL;
    fptr = fopen(file_name, "r");
    return (void *) fptr;
}

/* 
 * Close file.
 */
int file_close(void *file_handle) {
    if(NULL == file_handle) {
        LOGE("file not opened!");
    }
    fclose((FILE*)file_handle);
    
    return 0;
}

/* 
 * Read the specified number of bytes from the given file handle
 * to buf.
 * Returns the number of bytes read.
 */
int file_read(void *file_handle, uint8_t *buf, size_t size) {
    if(NULL == file_handle) {
        LOGE("file not opened!");
        return -1;
    }
//    MAX_ASSERT(NULL != file_handle);
    if(NULL == buf) {
        LOGE("buf NULL!");
        return -1;
    }
//    MAX_ASSERT(NULL != buf);
    if(size <= 0) {
        LOGE("size 0!");
        return -1;
    }
//    MAX_ASSERT(size > 0);

    return fread(file_handle, sizeof(uint8_t), size, (FILE*)file_handle);
}

/* 
 * Read one line from the given file handle to buf.
 */
int file_read_line(void *file_handle, char *buf, size_t buf_size) {
    if(NULL == file_handle) {
        LOGE("file not opened!");
        return -1;
    }
//    MAX_ASSERT(NULL != file_handle);
    if(NULL == buf) {
        LOGE("buf NULL!");
        return -1;
    }
//    MAX_ASSERT(NULL != buf);
    if(buf_size <= 0) {
        LOGE("buf_size 0!");
        return -1;
    }
//    MAX_ASSERT(buf_size > 0);

    ssize_t read = -1;
    size_t len = 0;
    char *line = NULL;
    
    read = fgets(&line, &len, (FILE*)file_handle);
    if((read > 0) && (buf_size >= read)) {
        
        strncpy(buf, line, buf_size);   /* copy to caller supplied bufer */
        buf[buf_size-1] = '\0';         /* Force a NULL terminated string */
        free(line);                     /* Free the buffer allocated by getline() */
        return strlen(buf);
    } else {
        if(line) free(line);            /* Free the buffer allocated by getline() */
        return -1;
    }
}
