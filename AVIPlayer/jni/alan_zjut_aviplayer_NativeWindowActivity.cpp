extern "C"{
#include <avilib.h>
}

#include <android/native_window_jni.h>
#include <android/native_window.h>

#include "Common.h"
#include "alan_zjut_aviplayer_NativeWindowActivity.h"

/**
 * 初始化原生window
 */
JNIEXPORT void JNICALL Java_alan_zjut_aviplayer_NativeWindowActivity_init
  (JNIEnv *env, jclass clazz, jlong avi, jobject surface){
	//从surface中获得原生window
	ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
	if(0 == nativeWindow){
		ThrowException(env, "java/io/RuntimeException", "无法从surface获得原生window");
		goto exit;
	}
	//设置buffer大小为AVI视频帧的分辨率
	//如果和window大小不一致，buffer会被缩放来匹配
	if(0 > ANativeWindow_setBuffersGeometry(nativeWindow, AVI_video_width((avi_t*)avi),
			AVI_video_height((avi_t*)avi), WINDOW_FORMAT_RGB_565)){
		ThrowException(env, "java/io/RuntimeException", "无法匹配大小");
	}
	//释放原生window
	ANativeWindow_release(nativeWindow);
	nativeWindow = 0;
	exit:
	return;
}

/**
 * 将给定的AVI文件帧渲染到给定的surface上
 */
JNIEXPORT jboolean JNICALL Java_alan_zjut_aviplayer_NativeWindowActivity_render
  (JNIEnv *env, jclass clazz, jlong avi, jobject surface){
	jboolean isFrameRead = JNI_FALSE;
	long frameSize = 0;
	int keyFrame = 0;
	//从surface中获取原生window
	ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
	if(0 == nativeWindow){
		ThrowException(env, "java/io/RuntimeException", "无法获得window");
		goto exit;
	}
	//锁定原生window并访问buffer
	ANativeWindow_Buffer windowBuffer;
	if(0 > ANativeWindow_lock(nativeWindow, &windowBuffer, 0)){
		ThrowException(env, "java/io/RuntimeException", "无法锁定window");
		goto release;
	}
	//将AVI帧比特流读取到原始 缓冲区
	frameSize = AVI_read_frame((avi_t*)avi, (char*)windowBuffer.bits, &keyFrame);
	//检查帧是否被成功读取
	if(0 < frameSize){
		isFrameRead = JNI_TRUE;
	}
	//解锁并输出缓冲区来显示
	if(0 > ANativeWindow_unlockAndPost(nativeWindow)){
		ThrowException(env, "java/io/RuntimeException", "无法解锁window");
		goto release;
	}
	release:
	//释放原生window
	ANativeWindow_release(nativeWindow);
	nativeWindow = 0;

	exit:
	return isFrameRead;
}



