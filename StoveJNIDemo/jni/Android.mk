LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Stove
LOCAL_SRC_FILES := Stove.c

LOCAL_LDLIBS += -llog 

include $(BUILD_SHARED_LIBRARY)
