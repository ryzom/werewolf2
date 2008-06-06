/*
 * Created on May 3, 2005
 *
 */
package client.gui;

import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.widgets.Display;

/**
 * @author Henri Kuuste
 * 
 */
public class SWTThread {

	private static SWTThread instance;

	public static SWTThread getInstance() {
		return instance;
	}

	public static void createInstance(Initializer app)
			throws SWTThreadAlreadyInstanciatedException {
		if (instance != null) {
			throw new SWTThreadAlreadyInstanciatedException();
		}

		new SWTThread(app);

	}

	Display display;

	private boolean terminated;

	private Thread runner;

	private SWTThread(final Initializer app) {

		instance = this;

		display = new Display();

		Display.setAppName("JUSS");

		runner = new Thread(new JUSSRunnable() {
			public void runSupport() {
				app.run();
			}
		}, "Main Thread");
		runner.start();

		while (!display.isDisposed() && !terminated) {
			try {
				if (!display.readAndDispatch())
					display.sleep();
			} catch (Exception e) {
				// printStackTrace(e) peaks siin olema vms
			}
		}

		if (!terminated) {
			app.stopIt();
		}

		display.dispose();
	}
	

	public void terminate() {
		terminated = true;
	}

	public Display getDisplay() {
		return display;
	}
	
	public void asyncExecBusy(final Runnable r) {
		display.asyncExec(new Runnable() {
			public void run() {
				BusyIndicator.showWhile(display, r);
			}
		});
	}
}
