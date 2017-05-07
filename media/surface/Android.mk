LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	SFStrategy.cpp \
        Image.cpp \
        main.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	libskia \
        libui \
        libgui

LOCAL_MODULE:= Snapshot

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
