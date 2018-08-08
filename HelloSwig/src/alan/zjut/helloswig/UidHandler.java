package alan.zjut.helloswig;

import alan.zjut.helloswig.swig.AsyncUidProvider;
import android.widget.TextView;

public class UidHandler extends AsyncUidProvider {
	private TextView textView;
	
	public UidHandler(TextView textView) {
		this.textView = textView;
	}
	
	@Override
	public void onUid(long uid) {
		textView.setText("UID:" + uid);
	}
}
