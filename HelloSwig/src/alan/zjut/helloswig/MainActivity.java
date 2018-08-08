package alan.zjut.helloswig;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {
	static {
		System.loadLibrary("hello");
		
	}
	private int i = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
	}
	


}
