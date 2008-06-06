
package client.gui;

/**
 * @author Henri Kuuste
 *
 */

public abstract class 
JUSSRunnable implements Runnable {
	public void run() {
		try{
			runSupport();
		}catch( Throwable e ){
//			TODO: print stack here
		}
	}
	
	public abstract void runSupport();
}
