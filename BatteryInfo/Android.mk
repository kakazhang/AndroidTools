LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := battery_info

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS :=-DKITKAT

LOCAL_CFLAGS += -std=gnu++11

LOCAL_SRC_FILES := \
	main_battery.cpp \
	BatteryManager.cpp \
	SysEvent.cpp \
	SockListener.cpp \
	BatteryListener.cpp \
	BatteryUpdate.cpp \
	WakeLock.cpp

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \
	libbinder \
	libstlport

LOCAL_C_INCLUDES += bionic external/stlport/stlport

include $(BUILD_EXECUTABLE)
