LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
# LOCAL_CFLAGS := -std=c99
LOCAL_CFLAGS := -D SINGLE_THREADED
LOCAL_MODULE := libsmartcard
LOCAL_MODULE_TAGS := optional
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
# LOCAL_SHARED_LIBRARIES := libutils \
        libcutils

LOCAL_SRC_FILES := com_smartcard.c \
                    drivers/sc_gpio_driver.c \
                    drivers/sc_gpio_adapter.c \
                    drivers/sc_i2c_driver.c \
                    drivers/hp_smart_card_gpio.c \
                    drivers/hp_smart_card_i2c.c \
                    drivers/i2c_tools/smfuncs.c \
                    drivers/i2c_tools/smbus.c \
                    infrastructure/assert/hp_assert.c \
                    infrastructure/debug_log/hp_debug_log.c \
                    infrastructure/debug_log/hp_utils.c \
                    os/os_posix/hp_os_services.c \
                    os/os_posix/msgq.c \
                    src/crypto/crypto_aes.c \
                    src/crypto/crypto_cmac.c \
                    src/hp_host_smart_card/hp_host_smart_card_chip.c \
                    src/hp_host_smart_card/hp_host_smart_card.c \
                    src/hp_host_smart_card/hp_host_smart_card_xpt_i2c.c \
                    src/hp_smart_card/hp_smart_card.c \
                    src/hp_smart_card/hw_smart_card.c \
                    src/hp_smart_card/hw_smart_card_proto.c \
                    src/hp_smart_card/hw_smart_card_xpt_i2c.c \
                    src/hp_smart_card/lib_crc.c \
                    src/sc_common_mem_access.c \
                    src/sc_ink_mem_access.c \
                    src/sc_supply_mem_access.c \
                    src/level_memory_access.c


LOCAL_C_INCLUDES += $(LOCAL_PATH)/ifc \
                    $(LOCAL_PATH)/drivers/i2c_tools \
                    $(LOCAL_PATH)/internal_ifc \
                    $(LOCAL_PATH)/infrastructure/internal_ifc \
                    $(LOCAL_PATH)/os/internal_ifc \
                    $(LOCAL_PATH)/os/os_raspberrypi \
                    $(LOCAL_PATH)/product/tij25 \
                    $(LOCAL_PATH)/src/crypto \
                    $(LOCAL_PATH)/src/hp_host_smart_card \
                    $(LOCAL_PATH)/src/hp_smart_card \
                    $(LOCAL_PATH)/src/internal_ifc

# LOCAL_C_INCLUDES += system/core/include/cutils

include $(BUILD_SHARED_LIBRARY)
