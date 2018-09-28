#include "Common.h"


void ThrowException(JNIEnv *env, const char*className, const char *message){
	//获取异常类
	jclass clazz = (*env).FindClass(className);
	//如果找到异常类
	if(0 != clazz){
		(*env).ThrowNew(clazz, message);
	}
	//释放
	(*env).DeleteLocalRef(clazz);
}


