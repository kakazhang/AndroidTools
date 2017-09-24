
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Recorder

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
    main.cpp \
    recorder.cpp

LOCAL_SHARED_LIBRARIES += \
    libcutils \
    libutils \
    libbinder \
    libmedia 

include $(BUILD_EXECUTABLE)
