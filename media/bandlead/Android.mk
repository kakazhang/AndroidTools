
LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := myBandlead

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    main.cpp \
    Bandlead.cpp

LOCAL_SHARED_LIBRARIES += \
    libbinder \
    libcutils \
    libgui \
    libutils \

include $(BUILD_EXECUTABLE)
