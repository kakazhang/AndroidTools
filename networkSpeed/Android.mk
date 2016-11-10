
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := NetSpeed

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := speed.c

include $(BUILD_EXECUTABLE)
