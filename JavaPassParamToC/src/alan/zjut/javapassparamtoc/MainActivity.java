package alan.zjut.javapassparamtoc;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends Activity {
	
	static {
		System.loadLibrary("passparam");
	}
	private JNI jni;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		jni = new JNI();
	}

	public void passInt(View v) {
		int result = jni.add(3, 5);
		Toast.makeText(this, result + "", Toast.LENGTH_SHORT).show();
	}

	public void passString(View v) {
		String result = jni.sayHelloInC("abcde");
		Toast.makeText(this, result, Toast.LENGTH_SHORT).show();
	}

	public void passArr(View v) {
		int[] array = new int[] {1, 2, 3, 4, 5};
		int[] result = jni.arrElementsIncrease(array);
		for(int i: result) {
			System.out.println("i = " + i);
		}
	}
}
