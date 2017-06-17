LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Optimise

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -std=gnu++11
LOCAL_CFLAGS += -W

LOCAL_SRC_FILES := \
	main.cpp \
    Commands.cpp

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libbinder

#add debug flag
ifeq ($(TARGET_PRODUCT),aosp_bullhead)
    LOCAL_CFLAGS += -DAOSP_BULLHEAD
else
    LOCAL_SHARED_LIBRARIES += libstlport
    LOCAL_C_INCLUDES += bionic external/stlport/stlport
endif

include $(BUILD_EXECUTABLE)

