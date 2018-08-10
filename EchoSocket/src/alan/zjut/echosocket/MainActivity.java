package alan.zjut.echosocket;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

public abstract class MainActivity extends Activity implements OnClickListener {
	private EditText portEdit;//端口号
	
	private Button startButton;//服务按钮
	
	private ScrollView logScroll;
	
	private TextView logView;
	
	protected abstract void onStartButtonClicked();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
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
	
	
	
	
	
}
