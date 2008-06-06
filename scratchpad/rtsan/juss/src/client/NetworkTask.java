/*
 * Created on May 25, 2005
 *
 */
package client;

import java.util.HashMap;

/**
 * @author Henri Kuuste
 *
 */
public abstract class NetworkTask implements Runnable {
	private String name;
	private HashMap data;
	
	public NetworkTask(String _name) {
		name = _name;
		data = new HashMap();
	}
	
	public void run() {
		CStatus.getInstance().setStatus("Network task: " + toString());
		runSupport();
		CStatus.getInstance().setStatus("");
	}

	public abstract void runSupport();
	
	public String getName() {
		return name;
	}
	
	public void setName(String _name) {
		name = _name;
	}
	
	public String toString() {
		return name;
	}
	
	public void setData(String name, Object _data) {
		data.put(name, _data);
	}
	
	public Object getData(String name) {
		return data.get(name);
	}
}
