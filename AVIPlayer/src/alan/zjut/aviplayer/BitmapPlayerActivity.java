package alan.zjut.aviplayer;

import java.util.concurrent.atomic.AtomicBoolean;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;

public class BitmapPlayerActivity extends AbstractPlayerActivity {
	private final AtomicBoolean isPlaying = new AtomicBoolean();//单一线程运行
	
	private SurfaceHolder surfaceHolder;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_bitmap_player);
		SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surface_view);
		surfaceHolder = surfaceView.getHolder();
		surfaceHolder.addCallback(surfaceHolderCallback);
	}
	
	/**
	 * surfaceholder监听surface事件回调
	 */
	private final Callback surfaceHolderCallback = new Callback() {
		
		@Override
		public void surfaceDestroyed(SurfaceHolder arg0) {
			isPlaying.set(false);
		}
		
		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			//surface准备好后播放
			isPlaying.set(true);
			//在一个单独的线程中渲染
			new Thread(renderer).start();
		}
		
		@Override
		public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
		}
	};
	
	/**
	 * 渲染线程通过一个bitmap将AVI文件中的视频帧渲染到surface上
	 */
	private final Runnable renderer = new Runnable() {
		@Override
		public void run() {
			//创建一个新的bitmap来保存所有的帧
			Bitmap bitmap = Bitmap.createBitmap(getWidth(avi), getHeight(avi), Bitmap.Config.RGB_565);
			//使用帧速来计算延迟
			long frameDelay = (long)(1000 / getFrameRate(avi));
			//播放的时候开始渲染
			while(isPlaying.get()) {
				//将帧渲染到bitmap
				render(avi, bitmap);
				//锁定canvas
				Canvas canvas = surfaceHolder.lockCanvas();
				//将bitmap绘制到canvas
				canvas.drawBitmap(bitmap, 0, 0, null);
				//canvas显示准备
				surfaceHolder.unlockCanvasAndPost(canvas);
				//等待下一帧
				try {
					Thread.sleep(frameDelay);
				}catch (Exception e) {
					break;
				}
			}
		}
	};
	
	//从AVI文件描述符输出到指定的bitmao来渲染帧
	private native static boolean render(long avi, Bitmap bitmap);

}
