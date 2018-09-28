extern "C"{
#include <avilib.h>
}

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <malloc.h>

#include "Common.h"
#include "alan_zjut_aviplayer_OpenGLPlayerActivity.h"


//设置原生渲染器结构体
struct Instance{
	char *buffer;//视频内存地址
	GLuint texture;

	Instance():
		buffer(0),
		texture(0){}
};

//初始化方法生成渲染器
JNIEXPORT jlong JNICALL Java_alan_zjut_aviplayer_OpenGLPlayerActivity_init
  (JNIEnv *env, jclass clazz, jlong avi){
	Instance *instance = 0;
	long frameSize = AVI_frame_size((avi_t*)avi, 0);
	if(0 >= frameSize){
		ThrowException(env, "java/io/RuntimeException", "无法获得frame大小");
		goto exit;
	}
	instance = new Instance();
	if(0 == instance){
		ThrowException(env, "java/io/RuntimeException", "无法分配渲染器");
		goto exit;
	}
	instance -> buffer = (char*)malloc(frameSize);
	if(0 == instance -> buffer){
		ThrowException(env, "java/io/RuntimeException", "无法分配渲染器的buffer");
		delete instance;
		instance = 0;
	}
	exit:
	return (jlong)instance;
}

//初始化opengl surface生成纹理
JNIEXPORT void JNICALL Java_alan_zjut_aviplayer_OpenGLPlayerActivity_initSurface
  (JNIEnv *env, jclass clazz, jlong inst, jlong avi){
	Instance *instance = (Instance*)inst;//获取渲染器
	//启用纹理
	glEnable(GL_TEXTURE_2D);
	//生成纹理对象
	glGenTextures(1, &(instance ->texture));
	//绑定到生成的纹理上
	glBindTexture(GL_TEXTURE_2D, instance -> texture);
	//获取宽高
	int frameWidth = AVI_video_width((avi_t*)avi);
	int frameHeigth = AVI_video_height((avi_t*)avi);
	//剪切纹理矩形
	GLint rect[] = {0, frameHeigth, frameWidth, -frameHeigth};
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, rect);

	//填充颜色
	glColor4f(1.0, 1.0, 1.0, 1.0);
	//生成一个空的纹理
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeigth,
			0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0);
}

/**
 * 渲染方法
 */
JNIEXPORT jboolean JNICALL Java_alan_zjut_aviplayer_OpenGLPlayerActivity_render
  (JNIEnv *env, jclass clazz, jlong inst, jlong avi){
	Instance *instance = (Instance*)inst;
	jboolean isFrameRead = JNI_FALSE;
	int keyFrame = 0;
	//将AVI帧字节读至bitmap
	long frameSize = AVI_read_frame((avi_t*)avi, instance ->buffer, &keyFrame);
	//检查帧是否读了
	if(0 >= frameSize){
		goto exit;
	}
	//读帧
	isFrameRead = JNI_TRUE;
	//使用新帧更新纹理
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, AVI_video_width((avi_t*)avi),
			AVI_video_height((avi_t*)avi), GL_RGB, GL_UNSIGNED_SHORT_5_6_5, instance ->buffer);
	//绘制纹理
	glDrawTexiOES(0, 0, 0, AVI_video_width((avi_t*)avi), AVI_video_height((avi_t*)avi)); // @suppress("Function cannot be resolved")
	exit:
	return isFrameRead;
}

/**
 * 释放渲染器
 */
JNIEXPORT void JNICALL Java_alan_zjut_aviplayer_OpenGLPlayerActivity_free
  (JNIEnv *env, jclass clazz, jlong inst){
	Instance *instance = (Instance*)inst;
	if(0 != instance){
		free(instance ->buffer);
		delete instance;
	}
}



