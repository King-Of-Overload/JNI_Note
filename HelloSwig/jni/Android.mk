LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := hello
LOCAL_SRC_FILES := hello.c

#启用RTTI 运行时类型识别
LOCAL_CPP_FEATURES += rtti

MY_SWIG_PACKAGE := alan.zjut.helloswig.swig
MY_SWIG_INTERFACES := Unix.i
MY_SWIG_TYPE := cxx

include $(LOCAL_PATH)/my-swig-generate.mk

include $(BUILD_SHARED_LIBRARY)
