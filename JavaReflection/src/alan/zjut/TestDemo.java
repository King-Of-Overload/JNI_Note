package alan.zjut;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class TestDemo {

	public static void main(String[] args) {
		//获取字节码
		Class clazz = Demo.class;
		//获取方法
		try {
			Method method = clazz.getMethod("hello", new Class[] {String.class});
			//创建对象
			Object object = clazz.newInstance();
			//通过对象调用方法
			method.invoke(object, new Object[] {"hello"});
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InstantiationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
	}

}
