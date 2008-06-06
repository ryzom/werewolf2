package client.gui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.program.Program;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;

import shared.CConfig;

import client.JussClient;

/**
 * @author Henri Kuuste
 * 
 */
public class MainMenu {

	private Display display;

	private MainWindow mainWindow;

	private Menu menuBar;

	private MenuItem fileItem;

	private Menu fileMenu;

	private Menu recentMenu;
	private List recents;

	public MainMenu(MainWindow mainWindow) {
		this.mainWindow = mainWindow;
		recents = new ArrayList();
		loadRecent();
		this.display = SWTThread.getInstance().getDisplay();
	}

	public void buildMenu() {
		try {
			// The Main Menu
			menuBar = new Menu(mainWindow.getShell(), SWT.BAR);
			mainWindow.getShell().setMenuBar(menuBar);

			// The File Menu
			fileItem = new MenuItem(menuBar, SWT.CASCADE);
			fileItem.setText("File");
			fileMenu = new Menu(mainWindow.getShell(), SWT.DROP_DOWN);
			fileItem.setMenu(fileMenu);

			MenuItem file_new = new MenuItem(fileMenu, SWT.CASCADE);
			file_new.setText("New Connection");

			MenuItem file_open = new MenuItem(fileMenu, SWT.CASCADE);
			file_open.setText("Open Connection");

			new MenuItem(fileMenu, SWT.SEPARATOR);

			MenuItem file_recent = new MenuItem(fileMenu, SWT.CASCADE);
			file_recent.setText("Recent");

			recentMenu = new Menu(mainWindow.getShell(), SWT.DROP_DOWN);
			file_recent.setMenu(recentMenu);

			new MenuItem(fileMenu, SWT.SEPARATOR);

			MenuItem file_exit = new MenuItem(fileMenu, SWT.NULL);
			file_exit.setText("Exit");

			file_exit.addListener(SWT.Selection, new Listener() {

				public void handleEvent(Event event) {
					mainWindow.dispose();
				}
			});

			updateRecent();

			file_new.addListener(SWT.Selection, new Listener() {
				public void handleEvent(Event e) {
					mainWindow.doNew();
				}
			});

			file_open.addListener(SWT.Selection, new Listener() {
				public void handleEvent(Event e) {
					mainWindow.doOpen();
				}
			});

			// ******** Options menu ***************

			MenuItem optionsItem = new MenuItem(menuBar, SWT.CASCADE);
			optionsItem.setText("Options");
			Menu optionsMenu = new Menu(mainWindow.getShell(), SWT.DROP_DOWN);
			optionsItem.setMenu(optionsMenu);

			MenuItem options_nothing = new MenuItem(optionsMenu, SWT.NULL);
			options_nothing.setText("Nothing here");
			options_nothing.setEnabled(false);

			// ******** Help menu ***************
			MenuItem helpItem = new MenuItem(menuBar, SWT.CASCADE);
			helpItem.setText("Help");
			Menu helpMenu = new Menu(mainWindow.getShell(), SWT.DROP_DOWN);
			helpItem.setMenu(helpMenu);

			MenuItem help_help = new MenuItem(helpMenu, SWT.NULL);
			help_help.setText("Help");
			help_help.addListener(SWT.Selection, new Listener() {
				public void handleEvent(Event e) {
					Program.launch("http://www.google.com");
				}
			});

			MenuItem help_about = new MenuItem(helpMenu, SWT.NULL);
			help_about.setText("About");
			help_about.addListener(SWT.Selection, new Listener() {
				public void handleEvent(Event e) {
					mainWindow.infoBox("About", "About JUSS....");
				}
			});
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	public void updateRecent() {
		MenuItem[] items = recentMenu.getItems();
		for(int i = 0; i < items.length; i++) {
			items[i].dispose();
		}
		Listener l = new Listener() {
			public void handleEvent(Event e) {
				final String path = ((MenuItem)e.widget).getText();
				Runnable task = new Runnable() {
					public void run() {
						String conName = JussClient.openConnection(path);
						mainWindow.addConnection(path, conName);
					}
				};
				BusyIndicator.showWhile(display, task);
			}
		};
		CConfig conf = JussClient.getConf();
		for (int i = 0; i < 5 && i < recents.size(); i++) {
			conf.setValue("recents/recent"+i, (String)recents.get(i));
			MenuItem recent_item = new MenuItem(recentMenu, SWT.NULL);
			recent_item.setText((String)recents.get(i));
			recent_item.addListener(SWT.Selection, l);
		}
		conf.save();
	}
	
	public void loadRecent() {
		CConfig conf = JussClient.getConf();
		for (int i = 0; i < 5; i++) {
			String recent = conf.getStringValue("recents/recent"+i);
			if(recent == null)
				break;
			recents.add(recent);
		}
	}
	
	public void addRecent(String path) {
		if(recents.contains(path))
			recents.remove(path);
		recents.add(0, path);
	}

}
