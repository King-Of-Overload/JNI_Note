package alan.zjut.aviplayer;

import java.io.FileReader;
import java.util.concurrent.atomic.AtomicBoolean;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Bundle;

public class OpenGLPlayerActivity extends AbstractPlayerActivity {
	private final AtomicBoolean isPlaying = new AtomicBoolean();//正在播放
	//原生渲染器
	private long instance;
	
	private GLSurfaceView glSurfaceView;
	

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_open_glplayer);
		glSurfaceView = (GLSurfaceView) findViewById(R.id.gl_surace_view);
		glSurfaceView.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);
		//设置渲染器
		glSurfaceView.setRenderer(renderer);
		//设置为请求时渲染帧模式
		glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
	}
	
	@Override
	protected void onStart() {
		super.onStart();
		//初始化渲染器
		instance = init(avi);
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		glSurfaceView.onResume();
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		glSurfaceView.onPause();
	}
	
	@Override
	protected void onStop() {
		super.onStop();
		//释放原生渲染器
		free(instance);
		instance = 0;
	}
	
	/**
	 * Open gl渲染方法
	 */
	private final Renderer renderer = new Renderer() {
		
		@Override
		public void onSurfaceCreated(GL10 g1, EGLConfig config) {
			//初始化opengl surface 
			initSurface(instance, avi);
			//surface准备好后开始播放
			isPlaying.set(true);
			//启动播放器
			new Thread(player).start();
		}
		
		@Override
		public void onSurfaceChanged(GL10 arg0, int arg1, int arg2) {
			
		}
		
		@Override
		public void onDrawFrame(GL10 g1) {
			//渲染下一帧
			if(!render(instance, avi)) {
				isPlaying.set(false);
			}
		}
	};
	
	//根据帧速请求渲染
	private final Runnable player = new Runnable() {
		
		@Override
		public void run() {
			//使用帧速计算延迟: 帧延迟 = 1000 / 每秒帧数
			long frameDelay = (long)(1000 / getFrameRate(avi));
			//播放时开始渲染
			while(isPlaying.get()) {
				//请求渲染
				glSurfaceView.requestRender();
				//等待下一帧
				try {
					Thread.sleep(frameDelay);
				} catch (Exception e) {
					break;
				}
			}
		}
	};
	
	//初始化原生渲染器
	private native static long init(long avi);
	
	//初始化opengl surface
	private native static void initSurface(long instance, long avi);
	
	//用给定文件进行帧渲染
	private native static boolean render(long instance, long avi);
	//释放原生渲染器
	private native static void free(long instance);
}
