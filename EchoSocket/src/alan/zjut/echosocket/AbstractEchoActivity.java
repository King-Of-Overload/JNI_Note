package alan.zjut.echosocket;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

public abstract class AbstractEchoActivity extends Activity implements OnClickListener {
	private EditText portEdit;//端口号
	
	private Button startButton;//服务按钮
	
	private ScrollView logScroll;
	
	private TextView logView;
	
	private final int layoutID;
	
	protected abstract void onStartButtonClicked();
	
	static {
		System.loadLibrary("Echo");
	}
	
	public AbstractEchoActivity(int layoutID) {
		this.layoutID = layoutID;
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(layoutID);
		portEdit = (EditText) findViewById(R.id.port_edit);
		logScroll = (ScrollView) findViewById(R.id.log_scroll);
		logView = (TextView) findViewById(R.id.log_view);
		startButton = (Button) findViewById(R.id.startBtn);
		startButton.setOnClickListener(this);
	}

	@Override
	public void onClick(View view) {
		if(view == startButton) {
			onStartButtonClicked();
		}
	}
	
	/**
	 * 获取端口号
	 * @return
	 */
	protected Integer getPort() {
		Integer port;
		port = Integer.valueOf(portEdit.getText().toString());
		return port;
	}
	
	/**
	 * 记录消息
	 * @param message
	 */
	protected void logMessage(final String message) {
		runOnUiThread(new Runnable() {
			
			@Override
			public void run() {
				logMessageDirect(message);
			}
		});
	}
	
	/**
	 * 直接记录给定的消息
	 * @param message
	 */
	private void logMessageDirect(final String message) {
		logView.append(message);
		logView.append("\n");
		logScroll.fullScroll(View.FOCUS_DOWN);
	}
	
	/**
	 * 抽象的异步echo任务
	 * @author Salu
	 *
	 */
	protected abstract class AbstractEchoTask extends Thread{
		//Handler对象
		private final Handler handler;
		
		//构造函数
		public AbstractEchoTask() {
			handler = new Handler();
		}
		
		protected void onPreExecute() {
			startButton.setEnabled(false);
			logView.setText("");
		}
		
		public synchronized void start() {
			onPreExecute();
			super.start();
		}
		
		@Override
		public void run() {
			onBackground();
			handler.post(new Runnable() {
				
				@Override
				public void run() {
					onPostExecute();
				}
			});
		}
		
		//新线程中的背景回调
		protected abstract void onBackground();
		
		//在调用线程后执行回调
		protected void onPostExecute() {
			startButton.setEnabled(true);
		}
	}
	
	
	
	
	
	
}
