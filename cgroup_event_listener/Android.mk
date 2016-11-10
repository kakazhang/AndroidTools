
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := CGroupListener

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	cgroup_listener.c

include $(BUILD_EXECUTABLE)
