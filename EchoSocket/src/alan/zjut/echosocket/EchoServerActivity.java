package alan.zjut.echosocket;

import android.os.Bundle;

public class EchoServerActivity extends AbstractEchoActivity {

	public EchoServerActivity() {
		super(R.layout.activity_echo_server);
	}

	@Override
	protected void onStartButtonClicked() {
		Integer port = getPort();//获取输入的端口号
		if(port != null) {
			//启动服务器任务
			ServerTask serverTask = new ServerTask(port);
			serverTask.start();
		}
	}
	

	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}
	
	//根据给定的端口号启动TCP服务器
	private native void nativeStartTcpServer(int port) throws Exception;
	
	
	//根据端口号启动UDP服务器
	private native void nativeStartUdpServer(int port) throws Exception;
	
	//执行生成签名的命令是： javap -s 类的全路径    bin/classses
	//服务器端的任务
	private class ServerTask extends AbstractEchoTask{
		private final int port;
		public ServerTask(int port) {
			this.port = port;
		}
		@Override
		protected void onBackground() {
			logMessage("服务器开启");//scroll打印消息
			try {
				nativeStartTcpServer(port);//调用原生启动tcp服务器
			} catch (Exception e) {
				logMessage(e.getMessage());//打印错误
			}
			logMessage("服务器停止");
		}
		
	}

}
