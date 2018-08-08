LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := hello
LOCAL_SRC_FILES := hello.c

# 生成代理类的包
MY_SWIG_PACKAGE := alan.zjut.simplehellojni.swig
#接口文件
MY_SWIG_INTERFACES := Unix.i

# 类型
MY_SWIG_TYPE := c

include $(LOCAL_PATH)/my-swig-generate.mk

include $(BUILD_SHARED_LIBRARY)
