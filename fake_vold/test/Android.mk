
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := losetup.c

LOCAL_MODULE := losetup

LOCAL_MODULE_TAGS := optional


LOCAL_SHARED_LIBRARIES += \
	      libcutils \
		  libutils \
		  libcrypto

include $(BUILD_EXECUTABLE)
