#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <android/log.h>
#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
/**
 * 将一个jstring转换成一个C语言的char* 类型
 */
char* JString2CStr(JNIEnv *env, jstring jstr) {
	char *result = NULL;
	jclass classtring = (**env).FindClass(env, "java/lang/String");
	jstring strencode = (**env).NewStringUTF(env, "UTF-8");
	jmethodID mid = (**env).GetMethodID(env, classtring, "getBytes",
			"(Ljava/lang/String;)[B");
	jbyteArray barr = (jbyteArray) (**env).CallObjectMethod(env, jstr, mid,
			strencode);
	jsize alen = (**env).GetArrayLength(env, barr);
	jbyte *ba = (**env).GetByteArrayElements(env, barr, JNI_FALSE);
	if (alen > 0) {
		result = (char *) malloc(alen + 1); //  \0
		memcpy(result, ba, alen);
		result[alen] = 0;
	}
	(**env).ReleaseByteArrayElements(env, barr, ba, 0);
	return result;
}

JNIEXPORT jint JNICALL Java_alan_zjut_javapassparamtoc_JNI_add(JNIEnv *env,
		jobject thiz, jint x, jint y) {
	return x + y;
}

JNIEXPORT jstring JNICALL Java_alan_zjut_javapassparamtoc_JNI_sayHelloInC(
		JNIEnv *env, jobject thiz, jstring jstr) {
	char *cstr = JString2CStr(env, jstr);
	//获取字符串长度
	int length = strlen(cstr);
	for (int i = 0; i < length; i++) {
		*(cstr + i) += 1; //为每个元素加1
	}

	return (**env).NewStringUTF(env, cstr);
}



JNIEXPORT jintArray JNICALL Java_alan_zjut_javapassparamtoc_JNI_arrElementsIncrease(
		JNIEnv *env, jobject thiz, jintArray jArray) {
	//获取数组长度
	int length = (**env).GetArrayLength(env, jArray);
	jintArray array = (**env).NewIntArray(env, length);
	LOGD("length=%d", length);
	//获取数组的首地址
	jboolean isCopy = 0; //java布尔在C中的表示，改参数在某些JVM中表示是否创建副本标志
	int *p = (*env)->GetIntArrayElements(env, jArray, JNI_FALSE); //Android中不会创建副本
	LOGD("首位元素: %d", *p);
	for (int i = 0; i < length; i++) {
		*(p + i) += 5;

		LOGD("修改后的:  %d", *(p + i));
	}
	LOGD("遍历之后p的地址: %p", p);
	(**env).SetIntArrayRegion(env, array, 0, length, p);
	return array;
}



JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_createDirectBuffer(JNIEnv *env,
		jobject thiz) {
	//基于给定的字节数组创建字节缓冲区
	unsigned char *buffer = (unsigned char*)malloc(1024);
	jobject directBuffer = (**env).NewDirectByteBuffer(env, buffer, 1024);
	if(directBuffer == NULL){
		free(buffer);
	}
}

JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_getDirectBuffer(JNIEnv *env,
		jobject thiz, jstring directBuffer) {

	//通过java字节缓冲区获取原生字节数组
	unsigned char *buffer;
	buffer = (unsigned char *)(**env).GetDirectBufferAddress(env, directBuffer);
}

//获取域ID
JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_getFieldID(JNIEnv *env,
		jobject thiz) {
	//用对象引用获得类
	jclass clazz = (**env).GetObjectClass(env, thiz);
	//获取实例域的域ID, 最后一个参数是域类型描述符
	jfieldID instanceFieldID;
	instanceFieldID = (**env).GetFieldID(env, clazz, "instanceField",
			"Ljava/lang/String;");
	//获取静态域的域ID
	jfieldID staticFieldID;
	staticFieldID = (**env).GetStaticFieldID(env, clazz, "staticField",
			"Ljava/lang/String;");
	//为提高性能 可以缓存域ID
}

//获取域
JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_getField(JNIEnv *env,
		jobject thiz) {
	//用对象引用获得类
	jclass clazz = (**env).GetObjectClass(env, thiz);

}

//捕获异常
JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_getException(JNIEnv *env,
		jobject thiz) {
	jmethodID throwingMethodID;

	//获取class与methodID  .....
	(**env).CallVoidMethod(env, thiz, throwingMethodID);
	jthrowable exception;
	exception = (**env).ExceptionOccurred(env);
	if(JNI_FALSE != exception){
		//发生异常
		(**env).ExceptionClear(env);
		//接下来处理异常
	}
}

//抛出异常
JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_throwException(JNIEnv *env,
		jobject thiz) {

	jclass clazz = (**env).FindClass(env, "java/lang/NullPointerException");
	if(JNI_FALSE != clazz){
		(**env).ThrowNew(env, clazz, "发生了异常");
	}
}

//局部引用
JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_localRef(JNIEnv *env,
		jobject thiz) {

	jclass clazz = (**env).FindClass(env,
			"java/lang/NullPointerException");
	(**env).DeleteLocalRef(env, clazz);
}

//检测弱全局引用有效性
JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_weakRefIsActive(JNIEnv *env,
		jobject thiz) {

	jclass weakRefClass = (**env).NewWeakGlobalRef(env, thiz);
	if(JNI_FALSE == (**env).IsSameObject(env, weakRefClass, NULL)){
		//仍然有效
	}else{
		//若全局引用已经被GC，无效
	}
}


//线程同步
JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_sync(JNIEnv *env,
		jobject thiz, jobject obj) {

	if(JNI_OK == (**env).MonitorEnter(env, obj)){
		//错误处理
	}
	//在此执行同步线程安全代码
	if(JNI_OK == (**env).MonitorExit(env, obj)){
		//错误处理
	}
}


//原生线程附着JVM
JNIEXPORT void JNICALL Java_alan_zjut_javapassparamtoc_JNI_nativeAttachJava(JNIEnv *env,
		jobject thiz, jobject obj) {
	JavaVM *cachedJVM;
	//将当前线程附着到虚拟机
	(**cachedJVM).AttachCurrentThread(cachedJVM, &env, NULL);
	//在此JNIEnv可以实现 线程与Java的通信
	//使用完后分离虚拟机
	(**cachedJVM).DetachCurrentThread(cachedJVM);

}
