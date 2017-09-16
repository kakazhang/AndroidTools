LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := DisplayColor

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    main.cpp \
    Show.cpp

LOCAL_SHARED_LIBRARIES += \
    libutils \
    libcutils \
    libbinder \
    libui \
    libgui \

include $(BUILD_EXECUTABLE)
