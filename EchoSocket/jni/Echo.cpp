/**
 * Socket实现
 */
#include <jni.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include<string.h>

#include <sys/un.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <endian.h>

#include <stddef.h>

#include "alan_zjut_echosocket_EchoServerActivity.h"
#include "alan_zjut_echosocket_EchoClientActivity.h"
#include "alan_zjut_echosocket_LocalSocketActivity.h"

#include <android/log.h>
#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,  __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

//最大日志消息长度
#define MAX_LOG_MESSAGE_LENGTH 256

//最大数据缓冲区大小
#define MAX_BUFFER_SIZE 80

/**
 * 将给定的消息记录到app
 */
static void LogMessage(JNIEnv *env, jobject obj, const char *format, ...) {
	//缓存日志方法id
	static jmethodID methodID = NULL;
	//判断
	if (NULL == methodID) {
		//从对象获取类
		jclass clazz = (*env).GetObjectClass(obj);
		//获取方法id
		methodID = (*env).GetMethodID(clazz, "logMessage",
				"(Ljava/lang/String;)V");
		//释放类引用
		(*env).DeleteLocalRef(clazz);
	}
	//如果方法找到
	if (NULL != methodID) {
		//格式化消息
		char buffer[MAX_LOG_MESSAGE_LENGTH];
		va_list ap;
		va_start(ap, format);
		vsnprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, ap);
		va_end(ap);

		//将缓冲区转换成Java字符串
		jstring message = (*env).NewStringUTF(buffer);

		if (NULL != message) {
			//记录消息
			(*env).CallVoidMethod(obj, methodID, message);
			//释放消息引用
			(*env).DeleteLocalRef(message);
		}
	}
}

/**
 * 用给定的异常类和异常消息抛出新的异常
 */
static void ThrowException(JNIEnv *env, const char *className,
		const char *message) {
	//获取异常类
	jclass clazz = (*env).FindClass(className);
	if (NULL != clazz) {
		//抛出异常
		(*env).ThrowNew(clazz, message);
		(*env).DeleteLocalRef(clazz);
	}
}

/**
 * 用给定的异常类与基于错误号的错误消息抛出新异常
 */
static void ThrowErrnoException(JNIEnv *env, const char *className,
		int errnum) {
	char buffer[MAX_LOG_MESSAGE_LENGTH];
	//获取错误号消息
	if (-1 == strerror_r(errnum, buffer, MAX_LOG_MESSAGE_LENGTH)) {
		strerror_r(errno, buffer, MAX_LOG_MESSAGE_LENGTH);
	}
	//抛出异常
	ThrowException(env, className, buffer);
}

/**
 * 构造新的 TCP socket
 */
static int NewTcpSocket(JNIEnv *env, jobject obj) {
	LogMessage(env, obj, "创建一个新的TCP socket");
	//构造socket
	int tcpSocket = socket(PF_INET, SOCK_STREAM, 0);
	//检查socket创建是否正确
	if (-1 == tcpSocket) {
		//创建失败，抛出,此时全局变量errno被socket函数赋予了错误代码
		ThrowErrnoException(env, "java/io/IOException", errno);
	}

	return tcpSocket;
}

/**
 * 将socket绑定到某一端口与地址
 */
static void BindSocketToPort(JNIEnv *env, jobject obj, int socketDescriptor,
		unsigned short port) {
	struct sockaddr_in address;
	//绑定socket地址
	//将address中当前位置后面的n个字节用ch替换并返回s
	//用是在一段内存块中填充某个给定的值，它是对较大的结构体或数组进行清零操作的一种最快方法
	memset(&address, 0, sizeof(address));
	//设置协议族
	address.sin_family = PF_INET;
	//设置协议地址(绑定所有地址)
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	//将端口转换为网络字节顺序并设置端口号
	address.sin_port = htons(port);
	//绑定socket
	LogMessage(env, obj, "绑定端口号： %hu.", port);
	if (-1
			== bind(socketDescriptor, (struct sockaddr*) &address,
					sizeof(address))) {
		//抛出异常
		ThrowErrnoException(env, "java/io/IOException", errno);
	}
}

/**
 * 获得当前绑定的端口号socket
 */
static unsigned short GetSocketPort(JNIEnv *env, jobject obj,
		int socketDescriptor) {
	unsigned short port = 0;
	struct sockaddr_in address;
	socklen_t addressLength = sizeof(address);
	//获取socket地址
	if (-1
			== getsockname(socketDescriptor, (struct sockaddr*) &address,
					&addressLength)) {
		//抛出错误异常
		ThrowErrnoException(env, "java/io/IOException", errno);
	} else {
		//将端口转换成主机字节顺序
		port = ntohs(address.sin_port);
		LogMessage(env, obj, "绑定到了随机端口 %hu.", port);
	}
	return port;
}

/**
 * 监听socket连接函数
 */
static void ListenOnSocket(JNIEnv *env, jobject obj, int socketDescriptor,
		int backlog) {
	LogMessage(env, obj, "开始监听socket，允许 %d 个连接挂起", backlog);
	//监听给定backlog的scoket
	if (-1 == listen(socketDescriptor, backlog)) {
		//抛出错误
		ThrowErrnoException(env, "java/io/IOException", errno);
	}
}

/**
 * 辅助函数，记录给定地址的IP与端口号
 */
static void LogAddress(JNIEnv *env, jobject obj, const char *message,
		const struct sockaddr_in *address) {
	//将IP地址转换成字符串
	char ip[INET_ADDRSTRLEN];
	if (NULL == inet_ntop(PF_INET, &(address->sin_addr), ip, INET_ADDRSTRLEN)) {
		//抛异常
		ThrowErrnoException(env, "java/io/IOException", errno);
	} else {
		//将端口转换为主机字节顺序
		unsigned short port = ntohs(address->sin_port);
		//纪录地址
		LogMessage(env, obj, "%s %s:%hu.", message, ip, port);
	}
}

/**
 * 在给定的socket上阻塞和等待进来的客户连接
 */
static int AcceptOnSocket(JNIEnv *env, jobject obj, int socketDescriptor) {
	struct sockaddr_in address;
	socklen_t addressLength = sizeof(address);
	//阻塞与等待进来的客户连接并接受
	LogMessage(env, obj, "等待客户端连接...");
	int clientSocket = accept(socketDescriptor, (struct sockaddr*) &address,
			&addressLength);
	if (-1 == clientSocket) {
		//抛异常
		ThrowErrnoException(env, "java/io/IOException", errno);
	} else {
		//纪录地址
		LogAddress(env, obj, "收到连接，来自：", &address);
	}
	return clientSocket;
}

/**
 * 阻塞接收来自socket数据放到缓冲区
 */
static ssize_t ReceiveFromSocket(JNIEnv *env, jobject obj, int socketDescriptor,
		char *buffer, size_t bufferSize) {
	LogMessage(env, obj, "接收socket...");
	//接收socket并将内容写入缓冲区
	ssize_t recvSize = recv(socketDescriptor, buffer, bufferSize - 1, 0);
	if (-1 == recvSize) {
		ThrowErrnoException(env, "java/io/IOException", errno);
	} else {
		//以NULL结尾创建一个缓冲区形成字符串
		buffer[recvSize] = '\0';
		if (recvSize > 0) {
			LogMessage(env, obj, "接收到 %d bytes: %s", recvSize, buffer);
		} else {
			LogMessage(env, obj, "客户端失去连接");
		}
	}
	return recvSize;
}

/**
 * 将数据缓冲区发送到socket
 */
static ssize_t SendToSocket(JNIEnv *env, jobject obj, int socketDescriptor,
		char *buffer, size_t bufferSize) {
	LogMessage(env, obj, "向socket发送数据...");
	ssize_t sentSize = send(socketDescriptor, buffer, bufferSize, 0);
	if (-1 == sentSize) {
		ThrowErrnoException(env, "java/io/IOException", errno);
	} else {
		if (sentSize > 0) {
			LogMessage(env, obj, "发送了 %d bytes: %s", sentSize, buffer);
		} else {
			LogMessage(env, obj, "与客户端失去连接.");
		}
	}
	return sentSize;
}

//原生tcp方法
JNIEXPORT void JNICALL Java_alan_zjut_echosocket_EchoServerActivity_nativeStartTcpServer(
		JNIEnv *env, jobject obj, jint port) {
	//构造新的tcp socket
	int serverSocket = NewTcpSocket(env, obj);
	if (NULL == (*env).ExceptionOccurred()) {
		//将socket绑定到某端口号
		BindSocketToPort(env, obj, serverSocket, (unsigned short) port);
		if (NULL != (*env).ExceptionOccurred()) {
			goto exit;
		}
		//如果请求了随机端口号
		if (0 == port) {
			//获得当前绑定的端口号socket
			GetSocketPort(env, obj, serverSocket);
			if (NULL != (*env).ExceptionOccurred()) {
				goto exit;
			}
		}

		//监听有4个等待连接的backlog的socket
		ListenOnSocket(env, obj, serverSocket, 4);
		if (NULL != (*env).ExceptionOccurred()) {
			goto exit;
		}
		//接受socket的一个客户连接
		int clientSocket = AcceptOnSocket(env, obj, serverSocket);
		if (NULL != (*env).ExceptionOccurred()) {
			goto exit;
		}
		//接收并发送返回的数据
		char buffer[MAX_BUFFER_SIZE];
		ssize_t recvSize;
		ssize_t sentSize;
		while (1) {
			//从socket接收
			recvSize = ReceiveFromSocket(env, obj, clientSocket, buffer,
			MAX_BUFFER_SIZE);
			if ((0 == recvSize) || (NULL != (*env).ExceptionOccurred())) {
				break;
			}

			//发送给socket
			sentSize = SendToSocket(env, obj, clientSocket, buffer,
					(size_t) recvSize);
			if ((0 == sentSize) || (NULL != (*env).ExceptionOccurred())) {
				break;
			}
		}
		//关闭客户端socket
		close(clientSocket);
	}

	exit: if (serverSocket > 0) {
		close(serverSocket);
	}
}

/*****客户端方法******/

/**
 * 连接到指定IP与端口号
 */
static void ConnectToAdress(JNIEnv *env, jobject obj, int socketDescriptor,
		const char *ip, unsigned short port) {
	//连接到IP与端口号
	LogMessage(env, obj, "开始连接到 %s:%uh...", ip, port);
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = PF_INET;	//设置协议族
	//将IP地址转成网络地址
	if (0 == inet_aton(ip, &(address.sin_addr))) {
		//抛异常
		ThrowErrnoException(env, "java/io/IOException", errno);
	} else {
		//将端口号转换为网络字节顺序
		address.sin_port = htons(port);
		//与地址连接
		if (-1
				== connect(socketDescriptor, (const sockaddr*) &address,
						sizeof(address))) {
			ThrowErrnoException(env, "java/io/IOException", errno);
		} else {
			LogMessage(env, obj, "已连接服务端");
		}
	}
}

JNIEXPORT void JNICALL Java_alan_zjut_echosocket_EchoClientActivity_nativeStartTcpClient(
		JNIEnv *env, jobject obj, jstring ip, jint port, jstring message) {
	//构造新的TCP socket
	int clientSocket = NewTcpSocket(env, obj);
	if (NULL == (*env).ExceptionOccurred()) {
		//以C字符串形式获取IP地址
		const char *ipAddress = (*env).GetStringUTFChars(ip, NULL);
		if (NULL == ipAddress) {
			goto exit;
		}
		//连接到IP地址和端口
		ConnectToAdress(env, obj, clientSocket, ipAddress,
				(unsigned short) port);
		//释放ip地址
		(*env).ReleaseStringUTFChars(ip, ipAddress);
		//如果连接不成功
		if (NULL != (*env).ExceptionOccurred()) {
			goto exit;
		}
		//连接成功,以C字符串形式获取消息,发送给服务器
		const char *messageText = (*env).GetStringUTFChars(message, NULL);
		if (NULL == messageText) {
			goto exit;
		}
		//获取消息大小
		jsize messageSize = (*env).GetStringUTFLength(message);
		//发送消息给socket
		SendToSocket(env, obj, clientSocket, (char*) messageText, messageSize);
		//释放消息文本
		(*env).ReleaseStringUTFChars(message, messageText);
		//如果发送未成功
		if (NULL != (*env).ExceptionOccurred()) {
			goto exit;
		}

		char buffer[MAX_BUFFER_SIZE];
		//从socket接收
		ReceiveFromSocket(env, obj, clientSocket, buffer, MAX_BUFFER_SIZE);
	}

	exit: if (clientSocket > -1) {
		close(port);
	}
}

/********本地socket方法********/

/**
 *第一步： 构造一个新的UNIX socket
 */
static int NewLocalSocket(JNIEnv *env, jobject obj) {
	//构造socket
	LogMessage(env, obj, "构造一个新的本地UNIX socket...");
	int localSocket = socket(PF_LOCAL, SOCK_STREAM, 0);
	//检查socket构造是否正确
	if (-1 == localSocket) {
		//抛异常
		ThrowErrnoException(env, "java/io/IOException", errno);
	}
	return localSocket;
}

/**
 * 将socket名与socket绑定
 */
static void BindLocalSocketName(JNIEnv *env, jobject obj, int socketDescriptor,
		const char*name) {
	struct sockaddr_un address;
	//名字长度
	const size_t nameLength = strlen(name);
	//路径长度初始化与名称长度相等
	size_t pathLength = nameLength;
	//如果名字不是以/开头，即它在抽象命名空间里
	bool abstratNamespace = ('/' != name[0]);
	//抽象命名空间要求目录的第一个字是0字节，更新路径长度包括0字节
	if(abstratNamespace){
		pathLength ++;
	}
	//检查路径长度
	if(pathLength > sizeof(address.sun_path)){
		ThrowErrnoException(env, "java/io/IOException", errno);
	}else{
		//初始化清除地址字节
		memset(&address, 0, sizeof(address));
		address.sun_family = PF_LOCAL;
		//socket路径
		char *sunPath = address.sun_path;
		//第一字节必须是0以抽象命名空间
		if(abstratNamespace){
			*sunPath ++ = NULL;
		}
		//追加本地名字
		strcmp(sunPath, name);
		//地址长度
		socklen_t addressLength = (offsetof(struct sockaddr_un, sun_path) + pathLength);
		//如果socket名已经绑定，取消连接
		unlink(address.sun_path);
		//绑定socket
		LogMessage(env, obj, "绑定到local name: %s%s.", (abstratNamespace)? "(null)": "", name);
		//开始绑定操作
		if(-1 == bind(socketDescriptor, (struct sockaddr*)&address, addressLength)){
			//抛异常
			ThrowErrnoException(env, "java/io/IOException", errno);
		}
	}
}

/**
 * 阻塞并等待给定socket上即将到来的客户端连接
 */
static int AcceptOnLocalSocket(JNIEnv *env, jobject obj, int socketDescriptor){
	//阻塞并等待即将到来的客户端连接并接收
	LogMessage(env, obj, "等待客户端连接...");
	int clientSocket = accept(socketDescriptor, NULL, NULL);
	//如果客户端socket无效
	if(-1 == clientSocket){
		ThrowErrnoException(env, "java/io/IOException", errno);
	}
	return clientSocket;
}


//本地socket服务端
JNIEXPORT void JNICALL Java_alan_zjut_echosocket_LocalSocketActivity_nativeStartLocalServer
  (JNIEnv *env, jobject obj, jstring name){
	//构造一个新的UNIX socket
	int serverSocket = NewLocalSocket(env, obj);
	if(NULL == (*env).ExceptionOccurred()){
		//以C字符串形式获取名称
		const char *nameText = (*env).GetStringUTFChars(name, NULL);
		if(NULL == nameText){goto exit;}
		//绑定socket到某一命名空间
		BindLocalSocketName(env, obj, serverSocket, nameText);
		//释放Name文本
		(*env).ReleaseStringUTFChars(name, nameText);
		//如果绑定失败
		if(NULL != (*env).ExceptionOccurred()){goto exit;}
		//监听有4个挂起连接的带backlog的socket
		ListenOnSocket(env, obj, serverSocket, 4);
		//接收一个客户连接
		int clientSocket = AcceptOnLocalSocket(env, obj, serverSocket);
		if(NULL != (*env).ExceptionOccurred()){goto exit;}

		char buffer[MAX_BUFFER_SIZE];
		ssize_t recvSize;
		ssize_t sentSize;
		//接收并发送回消息
		while(1){
			//从socket接收
			recvSize = ReceiveFromSocket(env, obj, clientSocket, buffer, MAX_BUFFER_SIZE);
			if((0 == recvSize) || (NULL != (*env).ExceptionOccurred())){break;}
			//发送给socket
			sentSize = SendToSocket(env, obj, clientSocket, buffer, (size_t)recvSize);
			if((0 == sentSize) || (NULL != (*env).ExceptionOccurred())){break;}
		}
		//关闭客户端
		close(clientSocket);
	}

	exit:
	if(serverSocket > 0){
		close(serverSocket);
	}
}

//void _stl_debug_message(const char * format_str, ...){
//	va_list ap;
//	va_start(ap, format_str);
//	__android_log_vprint(ANDROID_LOG_FATAL, "STLport", format_str, ap);
//	va_end(ap);
//}

