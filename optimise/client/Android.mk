LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Client

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -std=gnu++11

LOCAL_SRC_FILES := \
	client.cpp

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libstlport

LOCAL_C_INCLUDES += bionic external/stlport/stlport

include $(BUILD_EXECUTABLE)

