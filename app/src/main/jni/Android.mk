LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS +=  -llog -ldl

LOCAL_MODULE     := NativeGraphicJni

include $(BUILD_SHARED_LIBRARY)
