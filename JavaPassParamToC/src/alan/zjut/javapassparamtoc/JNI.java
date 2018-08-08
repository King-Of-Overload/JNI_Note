package alan.zjut.javapassparamtoc;

public class JNI {
	//pass int param
	public native int add(int x, int y);
	
	//pass String param
	public native String sayHelloInC(String s);
	
	//pass int array param
	public native int[] arrElementsIncrease(int[] intArray);
	
	public native void createDirectBuffer();
	
	    //假设在java类中存在如下两个字符串
		private String instanceField = "小缘";
		private static String staticField = "幼儿缘";
}
