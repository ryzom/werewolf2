/*
 * Created on May 25, 2005
 *
 */
package client;

import java.util.ArrayList;
import java.util.Iterator;

import org.apache.log4j.Logger;
import org.eclipse.swt.custom.BusyIndicator;

import client.gui.MainWindow;
import client.gui.SWTThread;

/**
 * @author Henri Kuuste
 *
 */
public class NetworkThread implements Runnable {
	private static NetworkThread instance;
	private static final Logger log = Logger.getLogger("juss.client.NetworkTask");
	private static Thread runner = null;

	public static NetworkThread getInstance() {
		if(instance == null) {
			instance = 	new NetworkThread();
		}
		return instance;
	}

	private ArrayList tasks;
	private ArrayList listeners;
	
	private NetworkThread() {
		tasks = new ArrayList();
		listeners = new ArrayList();
	}
	
	public void run() {
		while(true) {
			if(tasks.size() > 0) {
				NetworkTask currentTask = (NetworkTask)tasks.get(0);
				Thread subThread = new Thread(currentTask, "NetworkSubThread");
				subThread.run();
				while(subThread.isAlive()) {}
				tasks.remove(0);
				Iterator it = listeners.iterator();
				while(it.hasNext()) {
					((INetworkThreadListener)it.next()).networkTaskFinished(currentTask);
				}
				subThread = null;
			} else {
				log.debug("Stopping network thread");
				runner = null;
				break;
			}
		}
	}
	
	public void addTask(NetworkTask task) {
		tasks.add(task);
		if(runner == null) {
			log.debug("Starting network thread");
			runner = new Thread(this, "NetworkThread");
			runner.start();
		}
	}
	
	public void removeTask(NetworkTask task) {
		tasks.remove(task);
	}
	
	public void addListener(INetworkThreadListener l) {
		listeners.add(l);
	}
	
	public void removeListener(INetworkThreadListener l) {
		listeners.remove(l);
	}
}
