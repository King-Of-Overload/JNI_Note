package alan.zjut.hellothreads;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity {
	static {
		System.loadLibrary("HelloThreads");
	}
	private EditText threadsEdit;
	
	private EditText iterationsEdit;
	
	private Button startButton;
	
	private TextView logView;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		//初始化原生代码
		nativeInit();
		threadsEdit = (EditText) findViewById(R.id.threads_edit);
		iterationsEdit = (EditText) findViewById(R.id.iterations_edit);
		startButton = (Button) findViewById(R.id.start_button);
		logView = (TextView) findViewById(R.id.log_view);
		startButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				int threads = getNumber(threadsEdit, 0);//获取线程个数
				int iterations = getNumber(iterationsEdit, 0);//获取迭代个数
				if(threads > 0 && iterations > 0) {
					startThreads(threads, iterations);//开启线程
				}
			}
		});
	}
	
	@Override
	protected void onDestroy() {
		//释放原生资源
		nativeFree();
		super.onDestroy();
	}
	
	/**
	 * 原生消息回调
	 * @param message
	 */
	private void onNativeMessage(final String message) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				logView.append(message);
				logView.append("\n");
			}
		});
	}
	
	/**
	 * 以Integer格式获取编辑文本的值
	 * 如果值为empty或计数不能分析，则返回默认值
	 * @param editText
	 * @param defaultValue
	 * @return
	 */
	private static int getNumber(EditText editText, int defaultValue) {
		int value;
		try {
			value = Integer.parseInt(editText.getText().toString());
		} catch (NumberFormatException e) {
			value = defaultValue;
		}
		return value;
	}
	
	/**
	 * 启动给定个数的线程进行迭代
	 * @param threads
	 * @param iterations
	 */
	private void startThreads(int threads, int iterations) {
		//javaThreads(threads, iterations);
		posixThreads(threads, iterations);
	}
	
	/**
	 * 创建Java线程
	 * @param threads 线程数
	 * @param iterations 每个worker的迭代次数
	 */
	private void javaThreads(int threads, final int iterations) {
		//为每一个worker创建一个Java线程
		for (int i = 0; i < threads; i++) {
			final int id = i;
			Thread thread = new Thread() {
				@Override
				public void run() {
					nativeWorker(id, iterations);
				}
			};
			thread.start();
		}
	}
	
	/**
	 * 使用POSIX线程
	 * @param threads 线程数
	 * @param iterations 每个worker的迭代次数
	 */
	private native void posixThreads(int threads, int iterations);
	
	
	//初始化的原生代码,完成原生代码的初始化工作
	private native void nativeInit();
	
	//释放原生资源
	private native void nativeFree();
	
	/**
	 * 原生worker
	 * 用来模拟执行时间较长的任务
	 * @param id worked ID
	 * @param iterations 迭代次数
	 */
	private native void nativeWorker(int id, int iterations);
	
}
