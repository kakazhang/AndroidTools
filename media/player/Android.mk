
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main.cpp \
	Player.cpp

LOCAL_MODULE := myPlayer

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -W

LOCAL_C_INCLUDES += \
        system/core/fs_mgr/include \
		external/openssl/include \
		system/core/logwrapper/include
    
LOCAL_SHARED_LIBRARIES += \
		  libbinder \
	      libcutils \
		  libutils \
          libmedia

include $(BUILD_EXECUTABLE)
