LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := TrafficStatistics

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
    traffic_statistics.c

include $(BUILD_EXECUTABLE)
