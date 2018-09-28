package alan.zjut.aviplayer;

import java.io.IOException;

import android.app.Activity;
import android.app.AlertDialog;

public class AbstractPlayerActivity extends Activity {
	
	//加载so库
	static {
		System.loadLibrary("AVIPlayer");
	}
	//AVI文件名字的extra
	public static final String EXTRA_FILE_NAME = "alan.zjut.aviplayer.EXTRA_FILE_NAME";
	
	//AVI视频文件描述符
	protected long avi = 0;
	
	@Override
	protected void onStart() {
		super.onStart();
		//打开AVI文件
		try {
			String filename = getFileName();
			avi = open(getFileName());
		} catch (Exception e) {
			new AlertDialog.Builder(this)
			.setTitle(R.string.error_alert_title)
			.setMessage(e.getMessage())
			.show();
		}
	}
	
	@Override
	protected void onStop() {
		super.onStop();
		//如果avi处于开启状态
		if(0 != avi) {
			//关闭文件描述符
			close(avi);
			avi = 0;
		}
	}
	
	//获取AVI视频文件的名字
	protected String getFileName() {
		return getIntent().getExtras().getString(EXTRA_FILE_NAME);
	}
	
	//打开指定的AVI文件并返回文件描述符
	protected native static long open(String fileName) throws IOException;
	
	//获得视频宽度
	protected native static int getWidth(long avi);
	
	//获得视频高度
	protected native static int getHeight(long avi);
	
	//获得帧速
	protected native static double getFrameRate(long avi);
	
	//基于给定的文件描述符关闭指定的AVI文件
	protected native static void close(long avi);
	
	
}
