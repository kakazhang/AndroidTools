LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := TopIo

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := topio.c

include $(BUILD_EXECUTABLE)
