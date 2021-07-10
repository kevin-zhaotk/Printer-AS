/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

typedef enum
{
    REFLASH_OK              = 0,
    REFLASH_ERROR           = 1,
    REFLASH_INVALID_FILE    = 2,
} ReflashResult_t;

ReflashResult_t validate_srec(const char *fw_file_name, uint32_t *first_address, size_t *total_size);
ReflashResult_t micro_fw_reflash(int32_t instance, const char *fw_file_name, bool verify, bool reset);


