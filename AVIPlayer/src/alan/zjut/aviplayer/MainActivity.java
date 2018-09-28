package alan.zjut.aviplayer;

import java.io.File;
import java.lang.ref.Reference;
import java.lang.ref.WeakReference;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;

public class MainActivity extends Activity implements OnClickListener{
	private EditText fileNameEdit;
	//player类型单选组
	private RadioGroup playerRadioGroup;
	
	private Button playButton;
	private WeakReference<MainActivity> reference;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		reference = new WeakReference<MainActivity>(this);
		fileNameEdit = (EditText) findViewById(R.id.file_name_edit);
		playerRadioGroup = (RadioGroup) findViewById(R.id.player_radio_group);
		playButton = (Button) findViewById(R.id.play_button);
		playButton.setOnClickListener(this);
	}

	@Override
	public void onClick(View view) {
		switch (view.getId()) {
		case R.id.play_button:
			onPlayButtonClick();
			break;
		}
	}
	
	/**
	 * 按钮单击事件
	 */
	private void onPlayButtonClick() {
		Intent intent;
		//获得选择的单选按钮的id
		int radioId = playerRadioGroup.getCheckedRadioButtonId();
		//基于id选择activity
		switch (radioId) {
		case R.id.bitmap_player_radio:{
			intent = new Intent(reference.get(), BitmapPlayerActivity.class);
			break;
		}
		case R.id.open_gl_player_radio:{
			intent = new Intent(reference.get(), OpenGLPlayerActivity.class);
			break;
		}
		case R.id.native_window_player_radio:{
			intent = new Intent(reference.get(), NativeWindowActivity.class);
			break;
		}
		default:
			throw new UnsupportedOperationException("radioId=" + radioId);
		}
		File file = new File(Environment.getExternalStorageDirectory(), fileNameEdit.getText().toString());
		//将AVI文件的名字作为extra内容
		intent.putExtra(AbstractPlayerActivity.EXTRA_FILE_NAME, file.getAbsolutePath());
		//启动player activity
		startActivity(intent);
	}


}
