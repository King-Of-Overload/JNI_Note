LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := passparam
LOCAL_SRC_FILES := passparam.c

#加载liblog.so
LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)
