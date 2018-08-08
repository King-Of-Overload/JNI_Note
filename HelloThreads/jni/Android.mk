LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := HelloThreads
LOCAL_SRC_FILES := alan_zjut_hellothreads_MainActivity.cpp

include $(BUILD_SHARED_LIBRARY)
