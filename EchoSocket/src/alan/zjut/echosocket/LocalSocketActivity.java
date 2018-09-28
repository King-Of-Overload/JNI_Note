package alan.zjut.echosocket;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.net.LocalSocketAddress.Namespace;
import android.widget.EditText;

public class LocalSocketActivity extends AbstractEchoActivity {
	private EditText messageEdit;
	private EditText portEdit;

	public LocalSocketActivity() {
		super(R.layout.activity_echo_local);
		messageEdit = (EditText) findViewById(R.id.message_edit);
		portEdit = (EditText) findViewById(R.id.port_edit);
	}

	@Override
	protected void onStartButtonClicked() {
		String name = portEdit.getText().toString();
		String message = messageEdit.getText().toString();
		if((name.length() > 0) && (message.length() > 0)) {
			String socketName;
			//如果是文件系统socket，准备文件目录，否则直接赋值
			if(isFilesystemSocket(name)) {
				File file = new File(getFilesDir(), name);
				socketName = file.getAbsolutePath();
			}else {
				socketName = name;
			}
			ServerTask serverTask = new ServerTask(socketName);
			serverTask.start();
			ClientTask clientTask = new ClientTask(socketName, message);
			clientTask.start();
		}
	}
	
	
	/**
	 * 检查名称是否是filesystem socket
	 */
	private boolean isFilesystemSocket(String name) {
		return name.startsWith("/");
	}
	
	/**
	 * 启动绑定到给定名称的本地 UNIX 文件系统socket服务器
	 * @param name socket名称
	 */
	private native void nativeStartLocalServer(String name) throws Exception;
	
	
	/**
	 * 启动本地UNIX socket客户端(java)
	 * @param name
	 * @param message
	 * @throws Exception
	 */
	private void startLocalClient(String name, String message) throws Exception{
		//构造一个本地socket
		LocalSocket clientSocket = new LocalSocket();
		try {
		//设置socket名称空间
		LocalSocketAddress.Namespace namespace;
		if(isFilesystemSocket(name)) {
			namespace = Namespace.FILESYSTEM;
		}else {
			namespace = Namespace.ABSTRACT;
		}
		//构造本地socket地址
		LocalSocketAddress address = new LocalSocketAddress(name, namespace);
		//连接到本地socket
		logMessage("连接到" + name);
		clientSocket.connect(address);
		logMessage("已连接");
		//以字节形式获取我们需要发送的消息
		byte[] messageBytes = message.getBytes();
		//发送消息字节到socket
		logMessage("发送给socket...");
		OutputStream outputStream = clientSocket.getOutputStream();
		outputStream.write(messageBytes);
		logMessage(String.format("发送了 %d 字节： %s", messageBytes.length, message));
		//从socket中接收消息返回
		logMessage("正在从socket中接收返回消息...");
		InputStream inputStream = clientSocket.getInputStream();
		int readSize = inputStream.read(messageBytes);
		String receivedMessage = new String(messageBytes, 0, readSize);
		logMessage(String.format("接收了 %d 个字节: %s", readSize, receivedMessage));
		//关闭流
		outputStream.close();
		inputStream.close();
		}finally {
			clientSocket.close();
		}
	}
	
	///服务端任务(jni)
	private class ServerTask extends AbstractEchoTask{
		private final String name;
		
		public ServerTask(String name) {
			this.name = name;
		}

		@Override
		protected void onBackground() {
			logMessage("开启本地服务器");
			try {
				nativeStartLocalServer(name);
			} catch (Exception e) {
				logMessage(e.getMessage());
			}
			logMessage("服务器停止.");
		}
		
	}
	
	//客户端任务
	private class ClientTask extends Thread{
		private final String name;
		//发送消息的文本
		private final String message;
		
		public ClientTask(String name, String message) {
			this.name = name;
			this.message = message;
		}
		
		@Override
		public void run() {
			logMessage("开启客户端");
			try {
				startLocalClient(name, message);
			} catch (Exception e) {
				logMessage(e.getMessage());
			}
			logMessage("客户端停止");
		}
	}

}
