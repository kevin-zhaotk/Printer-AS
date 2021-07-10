LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
# LOCAL_CFLAGS := -std=c99
LOCAL_CFLAGS := -D SINGLE_THREADED
LOCAL_MODULE := libhp22mm
LOCAL_MODULE_TAGS := optional
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
# LOCAL_SHARED_LIBRARIES := libutils \
        libcutils

LOCAL_SRC_FILES := hp22mm.c \
                   common.c \
                   IDS/lib/ids.c \
                   PD/MaxLib/Common/common_service.c \
                   PD/MaxLib/Common/fw_reflash.c \
                   PD/MaxLib/IDS/IDS.c \
                   PD/MaxLib/IDS/ids_service.c \
                   PD/MaxLib/PrintHead/frame.c \
                   PD/MaxLib/PrintHead/print_head_driver.c \
                   PD/MaxLib/PrintHead/service.c \
                   PD/MaxLib/Utilities/crc.c \
                   PD/MaxLib/Utilities/debug_log.c \
                   PD/MaxLib/Utilities/srec.c \
                   PD/OEMLib/oem_lib.c \
                   PD/OEMLib/HAL/UART/uart.c \
                   PD/OEMLib/Utilities/file.c \
                   PD/OEMLib/Utilities/lock.c \
                   PD/OEMLib/Utilities/oem_time.c
#                   PD/OEMLib/Utilities/stack_trace.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/IDS/lib \
                    $(LOCAL_PATH)/PD/MaxLib/Common \
                    $(LOCAL_PATH)/PD/MaxLib/IDS \
                    $(LOCAL_PATH)/PD/MaxLib/Ifc \
                    $(LOCAL_PATH)/PD/MaxLib/PrintHead \
                    $(LOCAL_PATH)/PD/MaxLib/Utilities \
                    $(LOCAL_PATH)/PD/OEMLib/Ifc

# LOCAL_C_INCLUDES += system/core/include/cutils

include $(BUILD_SHARED_LIBRARY)
