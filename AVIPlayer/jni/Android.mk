LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := AVIPlayer
LOCAL_SRC_FILES := \
          Common.cpp \
          alan_zjut_aviplayer_AbstractPlayerActivity.cpp \
          alan_zjut_aviplayer_BitmapPlayerActivity.cpp \
          alan_zjut_aviplayer_OpenGLPlayerActivity.cpp \
          alan_zjut_aviplayer_NativeWindowActivity.cpp

# 使用AVILib静态库
LOCAL_STATIC_LIBRARIES += avilib_static
# 使用 jni 图形库
LOCAL_LDLIBS += -ljnigraphics
LOCAL_LDLIBS += -lGLESv1_CM
#启用GL ext原型
LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES
#连接Android库
LOCAL_LDLIBS += -landroid

include $(BUILD_SHARED_LIBRARY)

#引入AVILib模块
$(call import-module, transcode-1.1.5/avilib)
