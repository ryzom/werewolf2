/*
 * Created on May 3, 2005
 *
 */package client.gui;

import org.eclipse.swt.widgets.Display;

import client.JussClient;

/**
 * @author Henri Kuuste
 * 
 */
public class Initializer {
	private String[] args;

	public Initializer(String[] _args) {
		try {
			SWTThread.createInstance(this);
		} catch (SWTThreadAlreadyInstanciatedException e) {
			// printStackTrace( e );
		}
	}

	public void run() {
		try {
			SWTThread swt = SWTThread.getInstance();

			Display display = swt.getDisplay();
			ImageRepository.loadImagesForSplashScreen(display);
			// SplashWindow.create(display, this);
			ImageRepository.loadImages(display);
			JussClient.init();
			new MainWindow(this);
		} catch (Throwable e) {
			// printStackTrace(e);
		}
	}

	public void stopIt() {
		JussClient.shutdown();
		SWTThread.getInstance().terminate();
	}

	public static void main(String args[]) {
		new Initializer(args);
	}

}
