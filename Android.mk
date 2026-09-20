LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := veg_mod
LOCAL_SRC_FILES := main.c
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)
