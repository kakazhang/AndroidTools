
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := powersaver.grouper

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	power.c

LOCAL_SHARED_LIBRARIES += \
	libhardware

LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/hw

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := powersaver

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	main.c

LOCAL_SHARED_LIBRARIES += \
	libhardware 

#LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/hw

include $(BUILD_EXECUTABLE)
