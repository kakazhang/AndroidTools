
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := poweranalysis

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := poweranalysis.c

include $(BUILD_EXECUTABLE)
