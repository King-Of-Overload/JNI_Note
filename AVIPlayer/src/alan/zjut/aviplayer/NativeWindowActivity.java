package alan.zjut.aviplayer;

import java.util.concurrent.atomic.AtomicBoolean;

import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;

public class NativeWindowActivity extends AbstractPlayerActivity {
	//正在播放
	private AtomicBoolean isPlaying = new AtomicBoolean();
	private SurfaceHolder surfaceHolder;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_native_window);
		SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surface_view);
		surfaceHolder = surfaceView.getHolder();
		surfaceHolder.addCallback(surfaceHolderCallback);
	}
	
	/**
	 * surface holder callback
	 */
	private final Callback surfaceHolderCallback = new Callback() {
		
		@Override
		public void surfaceDestroyed(SurfaceHolder arg0) {
			isPlaying.set(false);
		}
		
		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			isPlaying.set(true);
			//在一个单独线程中启动渲染器
			new Thread(renderer).start();
		}
		
		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
			
		}
	};
	
	
	/**
	 * 渲染线程通过一个bitmap将AVI渲染到surface上
	 */
	private final Runnable renderer = new Runnable() {
		
		@Override
		public void run() {
			//获得surface实例
			Surface surface = surfaceHolder.getSurface();
			//初始化原生window
			init(avi, surface);
			//计算延迟
			long frameDelay = (long)(1000 / getFrameRate(avi));
			//播放时开始渲染
			while(isPlaying.get()) {
				//渲染至surface
				render(avi, surface);
				//等待下一帧
				try {
					Thread.sleep(frameDelay);
				} catch (Exception e) {
					break;
				}
			}
		}
	};
	
	
	//初始化原生window
	private native static void init(long avi, Surface surface);
	
	//将给定的AVI文件帧渲染到给定的surface上
	private native static boolean render(long avi, Surface surface);
}
