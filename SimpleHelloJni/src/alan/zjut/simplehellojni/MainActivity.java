package alan.zjut.simplehellojni;


import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends Activity {
	
	static {
		System.loadLibrary("hello");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
	}
	
	
	public void callC(View v) {
		Toast.makeText(this, helloFromC(), Toast.LENGTH_SHORT).show();
	}
	
	public void getAppID(View v) {
		//Toast.makeText(this, String.valueOf(Unix.getuid()), Toast.LENGTH_SHORT).show();
	}
	
	
	public native String helloFromC();
}
