package alan.zjut.echosocket;

import android.os.Bundle;
import android.widget.EditText;

/**
 * socket客户端
 * @author Salu
 *
 */
public class EchoClientActivity extends AbstractEchoActivity {
	private EditText ipEdit;//IP地址
	
	private EditText messageEdit;//消息

	public EchoClientActivity() {
		super(R.layout.activity_echo_client);
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		ipEdit = (EditText) findViewById(R.id.ip_edit);
		messageEdit = (EditText) findViewById(R.id.message_edit);
	}

	@Override
	protected void onStartButtonClicked() {
		String ip = ipEdit.getText().toString();
		Integer port = getPort();
		String message = messageEdit.getText().toString();
		if((0 != ip.length() && (port != null) && (0 != message.length()))) {
			ClientTask clientTask = new ClientTask(ip, port, message);
			clientTask.start();
		}
	}
	
	//启动TCP客户端
	private native void nativeStartTcpClient(String ip, int port, String message) throws Exception;
	
	
	private class ClientTask extends AbstractEchoTask{
		private final String ip;
		private final int port;
		private final String message;
		
		public ClientTask(String ip, int port, String message) {
			this.ip = ip;
			this.port = port;
			this.message = message;
		}

		@Override
		protected void onBackground() {
			logMessage("开启客户端");
			try {
				nativeStartTcpClient(ip, port, message);
			}catch (Throwable e) {
				logMessage(e.getMessage());
			}
			logMessage("客户端终止");
		}
		
	}
}
