LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := libNativeGraphicJni
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog

LOCAL_SRC_FILES := com_industry_printer_data_NativeGraphicJni.cpp

include $(BUILD_SHARED_LIBRARY)
