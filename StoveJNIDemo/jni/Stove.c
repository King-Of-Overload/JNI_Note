#include <jni.h>
#include <stdlib.h>
#include <android/log.h>
#include <unistd.h>
#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,  __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

int getPressure() {
	return rand() % 101;
}

int flag = 1;
JNIEXPORT void JNICALL Java_alan_zjut_stovejnidemo_MainActivity_startMonitor(
		JNIEnv *env, jobject thiz) {
	jclass clazz = (**env).FindClass(env,
			"alan/zjut/stovejnidemo/MainActivity");
	jmethodID methodID = (**env).GetMethodID(env, clazz, "setPressure", "(I)V");
	flag = 1;
	while(flag){
		sleep(1);//Linux中 1就是一秒钟
		LOGD("执行一次");
		(**env).CallVoidMethod(env, thiz, methodID, getPressure());
	}
}

JNIEXPORT void JNICALL Java_alan_zjut_stovejnidemo_MainActivity_stopMonitor(
		JNIEnv *env, jobject thiz) {
	flag = 0;
}
