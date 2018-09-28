extern "C"{
#include <avilib.h>
}
#include <jni.h>
#include "alan_zjut_aviplayer_BitmapPlayerActivity.h"
#include <android/bitmap.h>
#include "Common.h"

/**
 * 根据文件描述符输出到指定的bitmap来渲染帧
 */
JNIEXPORT jboolean JNICALL Java_alan_zjut_aviplayer_BitmapPlayerActivity_render
  (JNIEnv *env, jclass clazz, jlong avi, jobject bitmap){
	jboolean isFrameRead = JNI_FALSE;
	char *frameBuffer = 0;//存放raw byte
	long frameSize = 0;
	int keyFrame = 0;
	//锁定bitmap并得到raw byte
	if(0 > AndroidBitmap_lockPixels(env, bitmap, (void**)&frameBuffer)){
		ThrowException(env, "java/io/IOException", "无法锁定bitmap");
		goto exit;
	}
	//将AVI帧byte读取到bitmap中
	frameSize = AVI_read_frame((avi_t*)avi, frameBuffer, &keyFrame);
	//解锁bitmap
	if(0 > AndroidBitmap_unlockPixels(env, bitmap)){
		ThrowException(env, "java/io/IOException", "无法解锁bitmap");
		goto exit;
	}
	//检查帧是否读取成功
	if(0 < frameSize){
		//成功
		isFrameRead = JNI_TRUE;
	}
	exit:
	return isFrameRead;
}




