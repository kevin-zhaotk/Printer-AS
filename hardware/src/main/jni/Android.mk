LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libHardware_jni
LOCAL_MODULE_TAGS := optional
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
# LOCAL_SHARED_LIBRARIES := libutils \
        libcutils
LOCAL_SRC_FILES := com_industry_printer_UsbSerial.c \
					com_industry_printer_RFIDOperation.c \
					com_industry_printer_GPIOOperation.c \
					com_industry_printer_RTCDevice.c \
					com_industry_printer_HardwareJni.c
					 
include $(BUILD_SHARED_LIBRARY)
