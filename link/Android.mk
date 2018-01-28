LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Link

LOCAL_MODULE_TAGS := optional

#-DDebugLinkEvent

LOCAL_CFLAGS += \
    -DDebugTcpServerLinkHandler \
    -DDebugTcpLinkHandler \
    -DDebugLink \
    -DDUMPJSON \

LOCAL_SRC_FILES := \
      src/main.cpp \
      src/Listener.cpp \
      src/LinkManager.cpp \
      src/Link.cpp \
      src/LinkEvent.cpp \
      src/UartLinkHandler.cpp \
      src/TcpServerLinkHandler.cpp \
      src/TcpLinkHandler.cpp \
      src/UsbObserver.cpp \
      src/LinkFactory.cpp \
      src/CommandListener.cpp

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/inc \
    frameworks/av/include/ \
    external/jsoncpp/include/ \

LOCAL_STATIC_LIBRARIES += \
    libjsoncpp

LOCAL_SHARED_LIBRARIES += \
    libbinder \
    libutils \
    libcutils \
    libstagefright \
    libstagefright_foundation \
    libhardware_legacy

include $(BUILD_EXECUTABLE)
