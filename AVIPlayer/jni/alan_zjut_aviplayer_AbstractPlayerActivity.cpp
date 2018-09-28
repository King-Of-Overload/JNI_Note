extern "C"{
#include <avilib.h>
}

#include "Common.h"
#include "alan_zjut_aviplayer_AbstractPlayerActivity.h"
#include "alan_zjut_aviplayer_BitmapPlayerActivity.h"


/**
 * 打开avi文件并返回文件描述符
 */
JNIEXPORT jlong JNICALL Java_alan_zjut_aviplayer_AbstractPlayerActivity_open
  (JNIEnv *env, jclass clazz, jstring fileName){
	avi_t *avi = 0;
	//获取文件名字赋给C字符串
	const char *cFileName = (*env).GetStringUTFChars(fileName, 0);
	if(0 == cFileName){goto exit;}
	//打开avi文件
	avi = AVI_open_input_file(cFileName, 1);
	//释放文件名字
	(*env).ReleaseStringUTFChars(fileName, cFileName);
	//如果不能打开则抛出一个异常
	if(0 == avi){
		ThrowException(env, "java/io/IOException", AVI_strerror());
	}
	exit:
	return (jlong)avi;
}

/**
 * 获取视频宽度
 */
JNIEXPORT jint JNICALL Java_alan_zjut_aviplayer_AbstractPlayerActivity_getWidth
  (JNIEnv *env, jclass clazz, jlong avi){
	return AVI_video_width((avi_t*)avi);
}

/**
 * 获取视频高度
 */
JNIEXPORT jint JNICALL Java_alan_zjut_aviplayer_AbstractPlayerActivity_getHeight
  (JNIEnv *env, jclass clazz, jlong avi){
	return AVI_video_height((avi_t*)avi);
}

/**
 * 获取视频帧速
 */
JNIEXPORT jdouble JNICALL Java_alan_zjut_aviplayer_AbstractPlayerActivity_getFrameRate
  (JNIEnv *env, jclass clazz, jlong avi){
	return AVI_frame_rate((avi_t*)avi);
}

/**
 * 关闭文件描述符
 */
JNIEXPORT void JNICALL Java_alan_zjut_aviplayer_AbstractPlayerActivity_close
  (JNIEnv *env, jclass clazz, jlong avi){
	AVI_close((avi_t*)avi);
}




