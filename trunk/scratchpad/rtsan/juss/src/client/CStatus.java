/*
 * Created on May 25, 2005
 *
 */
package client;

import java.util.ArrayList;
import java.util.Iterator;

/**
 * @author Henri Kuuste
 *
 */
public class CStatus {
	public static CStatus instance = null;
	
	public static CStatus getInstance() {
		if(instance == null)
			instance = new CStatus();
		return instance;
	}

	private ArrayList listeners;
	
	public CStatus() {
		listeners = new ArrayList();
	}
	
	public void addListener(IStatusListener l) {
		listeners.add(l);
	}
	
	public void removeListener(IStatusListener l) {
		listeners.remove(l);
	}
	
	public void setStatus(String status) {
		Iterator it = listeners.iterator();
		while(it.hasNext()) {
			((IStatusListener)it.next()).setStatus(status);
		}
	}
}
