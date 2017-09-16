LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Handler

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    main.cpp \
    Handler.cpp

LOCAL_SHARED_LIBRARIES += \
    libcutils \
    libutils \
    libbinder

include $(BUILD_EXECUTABLE)
