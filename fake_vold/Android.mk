
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	SocketListener.cpp \
	NetlinkListener.cpp \
	NetlinkHandler.cpp \
	NetlinkManager.cpp \
	VolumeManager.cpp \
	Volume.cpp \
	DirectVolume.cpp \
	Loop.cpp \
	main.cpp

LOCAL_MODULE := fake_vold

LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES += \
        system/core/fs_mgr/include \
		external/openssl/include \
		system/core/logwrapper/include
    
LOCAL_STATIC_LIBRARIES += \
        libfs_mgr \
		liblogwrap

LOCAL_SHARED_LIBRARIES += \
	      libcutils \
		  libutils \
		  libcrypto

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_EXECUTABLE)
