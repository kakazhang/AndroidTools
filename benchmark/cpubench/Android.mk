LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cpufreq-bench

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    main.c \
    parse.c \
    system.c \
    benchmark.c \
    cpufreq.c \
    sysfs.c

LOCAL_C_INCLUDES := \
    bionic/libc/include

include $(BUILD_EXECUTABLE)
 
