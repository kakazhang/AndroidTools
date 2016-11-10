
LOCAL_PATH := $(call my-dir)

#include $(CLEAR_VARS)

#LOCAL_MODULE := Filter

#LOCAL_MODULE_TAGS := optional

#LOCAL_CFLAGS :=-DDebug

#LOCAL_CFLAGS += -std=gnu++11

#LOCAL_SRC_FILES := \
	main.cpp \
	Filter.cpp \
	GetProcessFiles.cpp

#LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libbinder \
	libstlport

#LOCAL_C_INCLUDES += bionic external/stlport/stlport

#include $(BUILD_EXECUTABLE)
include $(CLEAR_VARS)

LOCAL_MODULE := libfiles

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS :=-DDebug

LOCAL_CFLAGS += -std=gnu++11

LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk

LOCAL_SRC_FILES := \
	Filter.cpp \
	GetProcessFiles.cpp

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libbinder \
	libstlport

LOCAL_C_INCLUDES += bionic external/stlport/stlport

include $(BUILD_SHARED_LIBRARY)

#build service
include $(CLEAR_VARS)

LOCAL_MODULE := libfileservice

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -std=gnu++11

LOCAL_SRC_FILES := \
	main.cpp \
	IFileService.cpp \
	FileService.cpp

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libbinder \
	libstlport \
	libfiles

LOCAL_C_INCLUDES += bionic external/stlport/stlport

include $(BUILD_SHARED_LIBRARY)

#build service
include $(CLEAR_VARS)

LOCAL_MODULE := FileService

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -std=gnu++11

LOCAL_SRC_FILES := \
	main.cpp \
	IFileService.cpp \
	FileService.cpp

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libbinder \
	libstlport \
	libfiles

LOCAL_C_INCLUDES += bionic external/stlport/stlport

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := TestFileService

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -std=gnu++11

LOCAL_SRC_FILES := \
	test.cpp \
	IFileService.cpp

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libbinder \
	libstlport

LOCAL_C_INCLUDES += bionic external/stlport/stlport

include $(BUILD_EXECUTABLE)

