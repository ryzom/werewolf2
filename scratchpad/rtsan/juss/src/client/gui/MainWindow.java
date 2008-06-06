/*
 * Created on May 2, 2005
 *
 */
package client.gui;

import org.apache.log4j.Logger;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.custom.CLabel;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.ShellAdapter;
import org.eclipse.swt.events.ShellEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;

import shared.CConfig;
import shared.FileManager;
import shared.SearchCriteria;
import client.CConnection;
import client.CStatus;
import client.GuiNewProjectFrm;
import client.IStatusListener;
import client.JussClient;

/**
 * @author Henri Kuuste
 * 
 */
public class MainWindow extends JUSSRunnable implements IconBarEnabler, IStatusListener {
	private static MainWindow window;

	private Initializer initializer;

	private Display display;

	private Shell mainWindow;

	private MainMenu mainMenu;

	private IconBar iconBar;

	private CTabFolder folder;

	private CTabItem browserTab;

	private CTabItem transfersTab;

	private CTabItem searchTab;

	private CTabItem consoleTab;

	private Composite statusArea;

	private CLabel statusText;

	private ConsoleView console;
	
	private static final Logger log = Logger.getLogger("juss.client.gui.MainWindow");

	public MainWindow(Initializer _initializer) {
		initializer = _initializer;
		display = SWTThread.getInstance().getDisplay();
		window = this;
		display.syncExec(this);
		CStatus.getInstance().addListener(this);
	}

	public void runSupport() {
		try {
			mainWindow = new Shell(display, SWT.RESIZE | SWT.BORDER | SWT.CLOSE
					| SWT.MAX | SWT.MIN);
			mainWindow.setText("Java Unified Sharing System");
			mainWindow.setImages(new Image[] {
					ImageRepository.getImage("juss16"),
					ImageRepository.getImage("juss32"),
					ImageRepository.getImage("juss64"),
					ImageRepository.getImage("juss128") });


			mainMenu = new MainMenu(this);
			mainMenu.buildMenu();

			FormLayout mainLayout = new FormLayout();
			FormData formData;

			mainLayout.marginHeight = 0;
			mainLayout.marginWidth = 0;
			mainLayout.spacing = 0;
			mainWindow.setLayout(mainLayout);

			Label separator = new Label(mainWindow, SWT.SEPARATOR
					| SWT.HORIZONTAL);
			formData = new FormData();
			formData.top = new FormAttachment(0, 0);
			formData.left = new FormAttachment(0, 0);
			formData.right = new FormAttachment(100, 0);
			// 3.0
			separator.setLayoutData(formData);

			this.iconBar = new IconBar(mainWindow);
			this.iconBar.setCurrentEnabler(this);

			formData = new FormData();
			formData.top = new FormAttachment(separator);
			formData.left = new FormAttachment(0, 0);
			formData.right = new FormAttachment(100, 0);
			// 3.0
			this.iconBar.setLayoutData(formData);

			separator = new Label(mainWindow, SWT.SEPARATOR | SWT.HORIZONTAL);

			formData = new FormData();
			formData.top = new FormAttachment(iconBar.getBanner());
			formData.left = new FormAttachment(0, 0);
			formData.right = new FormAttachment(100, 0);
			separator.setLayoutData(formData);

			Composite statusBar = new Composite(mainWindow, SWT.SHADOW_IN);
			formData = new FormData();
			formData.bottom = new FormAttachment(100, 0);
			formData.left = new FormAttachment(0, 0);
			formData.right = new FormAttachment(100, 0);
			statusBar.setLayoutData(formData);

			folder = new CTabFolder(mainWindow, SWT.NONE);
//			folder.setSimple(false);
//			folder.setFont(new Font(display, "Times New Roman", 12, SWT.BOLD));
			try {
				folder.setSelectionBackground(new Color[] {
						display.getSystemColor(SWT.COLOR_LIST_BACKGROUND),
						display.getSystemColor(SWT.COLOR_LIST_BACKGROUND),
						folder.getBackground() }, new int[] { 10, 90 }, true);
			} catch (NoSuchMethodError e) {
				/** < SWT 3.0M8 * */
				folder.setSelectionBackground(new Color[] { display
						.getSystemColor(SWT.COLOR_LIST_BACKGROUND) },
						new int[0]);
			}
			folder.setSelectionForeground(display
					.getSystemColor(SWT.COLOR_LIST_FOREGROUND));
			formData = new FormData();
			formData.top = new FormAttachment(separator);
			formData.left = new FormAttachment(0, 0);
			formData.right = new FormAttachment(100, 0);
			formData.bottom = new FormAttachment(statusBar);
			folder.setLayoutData(formData);
			createTabs();

			GridLayout layout_status = new GridLayout();
			layout_status.numColumns = 1;
			layout_status.horizontalSpacing = 1;
			layout_status.verticalSpacing = 0;
			layout_status.marginHeight = 0;
			layout_status.marginWidth = 0;
			statusBar.setLayout(layout_status);

			GridData gridData;

			gridData = new GridData(GridData.FILL_HORIZONTAL);
			statusText = new CLabel(statusBar, SWT.SHADOW_IN);
			statusText.setText("Test status");
			statusText.setLayoutData(gridData);

			mainWindow.addShellListener(new ShellAdapter() {
				public void shellClosed(ShellEvent event) {
					event.doit = dispose();
				}
			});

			loadConf();
			mainWindow.open();
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	public boolean dispose() {
		if (!getConfirmation("Confirm exit", "Do you really want to quit?"))
			return false;

		CConfig conf = JussClient.getConf();
		Point size = mainWindow.getSize();
		conf.setValue("GUI/WindowSizeX", size.x);
		conf.setValue("GUI/WindowSizeY", size.y);
		Point location = mainWindow.getLocation();
		conf.setValue("GUI/WindowPositionX", location.x);
		conf.setValue("GUI/WindowPositionY", location.y);
		conf.save();
		((BrowserView) browserTab.getData()).saveConf();
		initializer.stopIt();

		if (!mainWindow.isDisposed()) {
			mainWindow.dispose();
		}
		CStatus.getInstance().removeListener(this);

		return true;
	}
	
	private void loadConf() {
		CConfig conf = JussClient.getConf();
		Integer x = conf.getIntValue("GUI/WindowSizeX");
		Integer y = conf.getIntValue("GUI/WindowSizeY");
		if (x != null && y != null)
			mainWindow.setSize(x.intValue(), y.intValue());
		x = conf.getIntValue("GUI/WindowPositionX");
		y = conf.getIntValue("GUI/WindowPositionY");
		if (x != null && y != null)
			mainWindow.setLocation(x.intValue(), y.intValue());
	}

	private void createTabs() {
		browserTab = new CTabItem(folder, SWT.NULL);
		browserTab.setText("Browser");
		browserTab.setImage(ImageRepository.getImage("tab_browser"));
		browserTab.setToolTipText("File Browser");
		setView(new BrowserView(), browserTab);
		
		transfersTab = new CTabItem(folder, SWT.NULL);
		transfersTab.setText("Transfers");
		transfersTab.setImage(ImageRepository.getImage("tab_transfers"));
		transfersTab.setToolTipText("Transfers");
		setView(new TransferManagerView(), transfersTab);

		searchTab = new CTabItem(folder, SWT.NULL);
		searchTab.setText("Search");
		searchTab.setImage(ImageRepository.getImage("tab_search"));
		searchTab.setToolTipText("Search");
		setView(new SearchView(), searchTab);

		consoleTab = new CTabItem(folder, SWT.NULL);
		consoleTab.setText("Event Log");
		consoleTab.setImage(ImageRepository.getImage("tab_console"));
		consoleTab.setToolTipText("Event Log");
		setView(getConsole(), consoleTab);

		folder.setSelection(browserTab);
	}

	public ConsoleView getConsole() {
		if (console == null)
			console = new ConsoleView();
		return console;
	}

	public boolean isEnabled(String itemKey) {
		if (itemKey.equals("open"))
			return true;
		if (itemKey.equals("new"))
			return true;
		if (itemKey.equals("refresh"))
			return true;
		return false;
	}

	public boolean isSelected(String itemKey) {
		return false;
	}

	public void itemActivated(String itemKey) {
		if (itemKey.equals("open")) {
			doOpen();
			return;
		}
		if (itemKey.equals("new")) {
			doNew();
			return;
		}
		if (itemKey.equals("refresh")) {
			doRefresh();
			return;
		}
	}

	public void infoBox(String title, String message) {
		MessageBox mb = new MessageBox(mainWindow, SWT.ICON_INFORMATION
				| SWT.OK);
		mb.setText(title);
		mb.setMessage(message);
		mb.open();
	}

	public void errorBox(String title, String message) {
		MessageBox mb = new MessageBox(mainWindow, SWT.ICON_ERROR | SWT.OK);
		mb.setText(title);
		mb.setMessage(message);
		mb.open();
	}

	public boolean getConfirmation(String title, String message) {
		MessageBox mb = new MessageBox(mainWindow, SWT.ICON_WARNING | SWT.YES
				| SWT.NO);
		mb.setText(title);
		mb.setMessage(message);
		if (mb.open() == SWT.YES)
			return true;
		return false;
	}

	public Shell getShell() {
		return mainWindow;
	}

	public static MainWindow getWindow() {
		return window;
	}

	public MainMenu getMenu() {
		return mainMenu;
	}

	public IView getView(CTabItem item) {
		return (IView) item.getData();
	}

	public void setView(IView view, CTabItem item) {
		view.initialize(folder);
		item.setControl(view.getComposite());
		item.setData(view);
		item.addDisposeListener(new DisposeListener() {
			public void widgetDisposed(DisposeEvent e) {
				((IView) (e.widget.getData())).delete();
			}
		});
	}

	public void doSearch(final String str) {
		Runnable task = new Runnable() {
			public void run() {
				SearchCriteria c = ((SearchView)searchTab.getData()).getSearchCriteria(str);
				SearchTab tab = new SearchTab(c);
				folder.setSelection(searchTab);
			}
		};
		BusyIndicator.showWhile(display, task);
	}

	public void doOpen() {
		DirectoryDialog directoryDialog = new DirectoryDialog(mainWindow);
		directoryDialog.setText("Select Project Path...");
		final String location = directoryDialog.open();
		if (location != null) {
			Runnable task = new Runnable() {
				public void run() {
					String conName = JussClient.openConnection(location);
					log.debug("Opening connection " + conName);
					addConnection(location, conName);
				}
			};
			BusyIndicator.showWhile(display, task);
		}
	}

	public void doNew() {
		Rectangle r1 = mainWindow.getBounds();
		// this is bad style
		// TODO: FIX IT
		int sX = r1.x + (r1.width / 2) - (347 / 2);
		int sY = r1.y + (r1.height / 2) - (291 / 2);
		final String[] settings = GuiNewProjectFrm.init(sX, sY, mainWindow);

		if (settings[0] == null)
			return;
		Runnable task = new Runnable() {
			public void run() {
				String name = JussClient.newConnection(settings[0], settings[1], settings[2]);
				addConnection(settings[1], name);
			}
		};
		BusyIndicator.showWhile(display, task);
	}

	public void addConnection(String path, String conName) {
		path = FileManager.fixSlashes(path);
		if (conName == null) {
			errorBox("Error while opening connection",
					"Selected path not valid");
			return;
		}
		CConnection con = JussClient.getConnection(path);
		if (con.isValid())
			log.info("Opened connection " + conName);
		else {
			errorBox("Error",
					"Error while opening connection! Please try again.");
			return;
		}

		mainMenu.addRecent(path);
		mainMenu.updateRecent();
		((BrowserView) getView(browserTab)).addConnection(con);
	}
	
	public CConnection[] getConnections() {
		return ((BrowserView) browserTab.getData()).getConnections();
	}

	public void switchToBrowser() {
		folder.setSelection(browserTab);
	}

	public void openFolder(final Long[] ids, final CConnection con) {
		switchToBrowser();
		Runnable task = new Runnable() {
			public void run() {
				((BrowserView) browserTab.getData()).openFolder(ids, con);
			}
		};
		BusyIndicator.showWhile(display, task);
	}
	
	public void doRefresh() {
		((BrowserView) browserTab.getData()).refreshTree();
	}

	public void doLocalRefresh() {
		((BrowserView) browserTab.getData()).refreshFileListing();
	}
	
	public void positionCenter(Shell s) {
		Point p = s.getSize();
		Rectangle r1 = mainWindow.getBounds();
		int sX = r1.x + (r1.width / 2) - (p.x / 2);
		int sY = r1.y + (r1.height / 2) - (p.y / 2);
		s.setLocation(sX, sY);
	}
	
	public void setStatus(final String status) {
		display.syncExec(new Runnable() {
			public void run() {
				if(statusText.isDisposed())
					return;
				statusText.setText(status);
				mainWindow.update();
			}
		});
		Thread.yield();	
	}
	
}
