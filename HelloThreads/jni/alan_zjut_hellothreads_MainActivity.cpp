#include <jni.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "alan_zjut_hellothreads_MainActivity.h"

//缓存的方法ID
static jmethodID gOnNativeMessage = NULL;

//Java虚拟机接口指针
static JavaVM *gVm = NULL;

//对象的全局引用,MainActivity
static jobject gObj = NULL;

//原生worker线程参数
struct NativeWorkerArgs{
	jint id;
	jint iterations;
};

//互斥实例
static pthread_mutex_t mutex;

/**
 * 共享库加载时系统自动调用调用
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved){
	gVm = vm;
	return JNI_VERSION_1_4;
}

/*
 * 初始化原生函数
 * Class:     alan_zjut_hellothreads_MainActivity
 * Method:    nativeInit
 * Signature: ()V
 */
void Java_alan_zjut_hellothreads_MainActivity_nativeInit
  (JNIEnv *env, jobject thiz){
	//初始化互斥锁
	if(0 != pthread_mutex_init(&mutex, NULL)){
		jclass exceptionClazz = (*env).FindClass("java/lang/RuntimeException");
		(*env).ThrowNew(exceptionClazz, "无法初始化互斥锁");
		goto exit;
	}
	//全局对象的缓存
	if(NULL == gObj){
		gObj = (*env).NewGlobalRef(thiz);
		if(NULL == gObj){
			goto exit;
		}
	}
	//方法ID是否缓存
	if(NULL == gOnNativeMessage){
		//获取对象类
		jclass clazz = (*env).GetObjectClass(thiz);
		//为回调方法获取方法ID
		gOnNativeMessage = (*env).GetMethodID(clazz, "onNativeMessage", "(Ljava/lang/String;)V");
		//方法是否找到
		if(NULL == gOnNativeMessage){
			//没有找到，需要获取异常类
			jclass exceptionClazz = (*env).FindClass("java/lang/RuntimeException");
			//抛出异常
			(*env).ThrowNew(exceptionClazz, "无法找到消息回调方法");
		}
	}
	exit:
	   return;
}

/*
 * 释放空间操作
 * Class:     alan_zjut_hellothreads_MainActivity
 * Method:    nativeFree
 * Signature: ()V
 */
void Java_alan_zjut_hellothreads_MainActivity_nativeFree
  (JNIEnv *env, jobject thiz){
	if(NULL != gObj){
		//清除全局引用
		(*env).DeleteGlobalRef(gObj);
		gObj = NULL;
	}
	//销毁互斥锁
	if(0 != pthread_mutex_destroy(&mutex)){//发生异常
		jclass exceptionClazz = (*env).FindClass("java/lang/RuntimeException");
		(*env).ThrowNew(exceptionClazz, "无法销毁互斥锁");
	}
}

/**
 * 进行JVM附着操作
 */
static void* nativeWorkerThread(void *args){
	JNIEnv *env = NULL;
	//将当前线程附着到虚拟机上,并且获得JNIEnv接口指针
	if(0 == gVm -> AttachCurrentThread(&env, NULL)){
		//获取原生worker线程参数
		NativeWorkerArgs *nativeWorkerArgs = (NativeWorkerArgs*)args;
		//在线程上下文运行原生worker
		Java_alan_zjut_hellothreads_MainActivity_nativeWorker(env, gObj,
				nativeWorkerArgs->id, nativeWorkerArgs->iterations);
		//释放原生worker线程参数
		delete nativeWorkerArgs;
		//从Java虚拟机分离当前线程
		(*gVm).DetachCurrentThread();
	}
	return (void *) 1;
}

/*
 * Class:     alan_zjut_hellothreads_MainActivity
 * Method:    nativeWorker
 * Signature: (II)V
 */
void Java_alan_zjut_hellothreads_MainActivity_nativeWorker
  (JNIEnv *env, jobject thiz, jint id, jint iterations){
	//锁定互斥锁
	if(0 != pthread_mutex_lock(&mutex)){
		jclass exceptionClazz = (*env).FindClass("java/lang/RuntimeException");
		(*env).ThrowNew(exceptionClazz, "无法锁定互斥锁");
		goto exit;
	}
	//循环给定的迭代数
	for(jint i = 0; i < iterations; i++){
		//准备消息
		char message[26];
		sprintf(message, "子线程 id %d: 迭代号 %d", id, i);
		//创建消息
		jstring messageString = (*env).NewStringUTF(message);
		//调用原生方法
		(*env).CallVoidMethod(thiz, gOnNativeMessage, messageString);
		//检查是否产生异常
		if(NULL != (*env).ExceptionOccurred()){
			break;
		}
		//模拟长时间操作,睡眠1秒
		sleep(1);
	}
	//解锁互斥锁
	if(0 != pthread_mutex_unlock(&mutex)){
		jclass exceptionClazz = (*env).FindClass("java/lang/RuntimeException");
		(*env).ThrowNew(exceptionClazz, "无法解锁互斥锁");
	}
	exit:
	    return;
}


/*
 * 创建线程
 * Class:     alan_zjut_hellothreads_MainActivity
 * Method:    posixThreads
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_alan_zjut_hellothreads_MainActivity_posixThreads
  (JNIEnv *env, jobject thiz, jint threads, jint iterations){
	//线程句柄集合
	pthread_t *handles = new pthread_t[threads];
	//为每一个worker创建一个POSIX线程
	for(jint i = 0; i < threads; i++){
		//封装参数
		NativeWorkerArgs *args = new NativeWorkerArgs();
		(*args).id = i;
		(*args).iterations = iterations;
		//线程句柄
		pthread_t thread;
		//创建一个新线程,添加任务
		int result = pthread_create(&handles[i], NULL, nativeWorkerThread, (void*)args);
		if(0 != result){//出现异常
			//获取异常类
			jclass exceptionClazz = (*env).FindClass("java/lang/RuntimeException");
			//抛出异常
			(*env).ThrowNew(exceptionClazz, "无法创建原生线程");
			goto exit;
		}
	}

	//等待线程终止
	for(jint i = 0; i < threads; i++){
		void *result = NULL;
		//连接每一个线程句柄
		if(0 != pthread_join(handles[i], &result)){//发生异常
			jclass exceptionClazz = (*env).FindClass("java/lang/RuntimeException");
			//抛出异常
			(*env).ThrowNew(exceptionClazz, "无法连接线程");
		}else{//连接正常
			char message[26];
			sprintf(message, "Worker %d returned %d", i, result);
			//来自C字符串的Message
			jstring messageString = (*env).NewStringUTF(message);
			//调用原生消息方法
			(*env).CallVoidMethod(gObj, gOnNativeMessage, messageString);
			//检查是否产生异常
			if(NULL != (*env).ExceptionOccurred()){
				goto exit;
			}
		}
	}

	exit:
	    return;
}
