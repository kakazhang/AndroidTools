
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main.cpp \
        ScreenRecorder.cpp

LOCAL_MODULE := myScreenRecorder

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -W

LOCAL_C_INCLUDES := \
	frameworks/av/media/libstagefright \
	frameworks/av/media/libstagefright/include \
 
LOCAL_SHARED_LIBRARIES += \
    libbinder \
    libgui \
    libcutils \
    libutils \
    libmedia \
    libstagefright \
    libstagefright_foundation

include $(BUILD_EXECUTABLE)
