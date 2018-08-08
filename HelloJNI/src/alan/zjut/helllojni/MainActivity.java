package alan.zjut.helllojni;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
	}
	
	public void jniHelloClick(View v) {
		//加载so文件
		System.loadLibrary("hello");
		//该处原生实现
		Toast.makeText(this, helloInC(), Toast.LENGTH_SHORT).show();
	}
	
	//通过native关键字， 声明一个本地方法， 本地方法不用实现， 需要用jni调用c的代码实现
	public native String helloInC();

}
