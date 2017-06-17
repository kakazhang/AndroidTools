LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Client

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -std=gnu++11

LOCAL_SRC_FILES := \
	client.cpp

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils

ifeq ($(TARGET_PRODUCT),aosp_grouper)
	LOCAL_SHARED_LIBRARIES += libstlport
        LOCAL_C_INCLUDES += bionic external/stlport/stlport
endif

include $(BUILD_EXECUTABLE)

