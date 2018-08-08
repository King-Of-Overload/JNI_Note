package alan.zjut.stovejnidemo;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ProgressBar;

public class MainActivity extends Activity {
	private ProgressBar progressBar;
	static {
		System.loadLibrary("Stove");
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		progressBar = (ProgressBar) findViewById(R.id.pb_pressure);
		progressBar.setMax(100);
	}
	
	public void start(View v) {
		new Thread() {
			public void run() {
				startMonitor();
			};
		}.start();
	}
	
	public void stop(View v) {
		stopMonitor();
	}
	
	public void setPressure(int pressure) {
		progressBar.setProgress(pressure);
	}
	
	public native void startMonitor();
	
	public native void stopMonitor();
	

}
