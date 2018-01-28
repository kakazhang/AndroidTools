LOCAL_PATH := $(call my-dir)
#### test attribute
include $(CLEAR_VARS)

LOCAL_MODULE := TestAttr

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    testAttr.cpp

LOCAL_SHARED_LIBRARIES += \
    libutils

include $(BUILD_EXECUTABLE)

#### test smart pointer
include $(CLEAR_VARS)

LOCAL_MODULE := TestSP

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    testSP.cpp

LOCAL_SHARED_LIBRARIES += \
    libutils

include $(BUILD_EXECUTABLE)

##### test json file parser
include $(CLEAR_VARS)

LOCAL_MODULE := TestJson

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    testJsonParser.cpp

LOCAL_C_INCLUDES += \
    external/jsoncpp/include/

LOCAL_STATIC_LIBRARIES += \
    libjsoncpp

include $(BUILD_EXECUTABLE)

###### test host id
include $(CLEAR_VARS)

LOCAL_MODULE := TestHostID

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    testHostID.cpp

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

#### test AMessage
LOCAL_MODULE := TestAMessage

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    testAMessage.cpp

LOCAL_C_INCLUDES += \
    frameworks/av/include/

LOCAL_SHARED_LIBRARIES += \
    libbinder \
    libutils \
    libcutils \
    libstagefright \
    libstagefright_foundation

include $(BUILD_EXECUTABLE)

#### test socket
include $(CLEAR_VARS)

LOCAL_MODULE := TestSocket

LOCAL_CFLAGS += -DDebugTcpLinkHandler -DDebugLink -DDebugLinkEvent

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += \
    libbinder \
    libutils \
    libcutils \
    libstagefright \
    libstagefright_foundation

LOCAL_SRC_FILES += \
    testSocket.cpp \
    TcpLinkHandler.cpp \
    Link.cpp \
    Listener.cpp \
    LinkEvent.cpp \
    common.cpp

include $(BUILD_EXECUTABLE)
