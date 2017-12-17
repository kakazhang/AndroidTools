LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tinyBench

LOCAL_MODULE_TAGS := optional 

LOCAL_SRC_FILES := \
    main.c \
    util.c \
    asm-opt.c \
    aarch64-asm.S \
    arm-neon.S

LOCAL_C_INCLUDES += \
     bionic/libc/private

include $(BUILD_EXECUTABLE)
