LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Optimise

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -std=gnu++11

LOCAL_SRC_FILES := \
	main.cpp \
        Commands.cpp

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libbinder \
	libstlport

LOCAL_C_INCLUDES += bionic external/stlport/stlport

include $(BUILD_EXECUTABLE)

