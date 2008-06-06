/*
 * Created on May 3, 2005
 *
 */
package client.gui;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.apache.log4j.Logger;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.TreeAdapter;
import org.eclipse.swt.events.TreeEvent;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.program.Program;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;

import shared.CConfig;
import shared.CMetadata;
import shared.FileManager;
import shared.JussFile;
import shared.JussFileComparator;
import client.CConnection;
import client.CStatus;
import client.GuiImportFileFrm;
import client.INetworkThreadListener;
import client.JussClient;
import client.NetworkTask;
import client.NetworkThread;

/**
 * 
 * @author Henri Kuuste
 */
public class BrowserView implements IView, INetworkThreadListener {

	private Display display;

	private Composite main;

	private Tree tree;

	private Table table;

	private Composite tableComp;

	private MetaView meta;

	private LiveSashForm sash;

	private LiveSashForm rightSide;

	private TableColumn c_name;

	private TableColumn c_size;

	private TableColumn c_version;

	private TableColumn c_modified;

	private int sortBy = JussFileComparator.NAME;

	private JussFileComparator fileComp = null;

	private Menu tableMenu;

	private Menu treeMenu;

	private static final Logger log = Logger
			.getLogger("juss.client.gui.BrowserView");

	public void initialize(Composite composite) {
		try {
			display = composite.getDisplay();
			main = new Composite(composite, SWT.NONE);

			FillLayout layout = new FillLayout();
			main.setLayout(layout);

			sash = new LiveSashForm(main, SWT.HORIZONTAL);
			tree = new Tree(sash, SWT.SINGLE);
			rightSide = new LiveSashForm(sash, SWT.VERTICAL);
			tableComp = new Composite(rightSide, SWT.NONE);
			tableComp.setLayout(new FillLayout());
			table = new Table(tableComp, SWT.MULTI);
			meta = new MetaView();
			meta.initialize(rightSide);

			createDragNDrop();

			createColumns();

			table.setHeaderVisible(true);
			loadConf();

			createTreeRMBMenu();
			createTreeListeners();

			createTableRMBMenu();
			createTableListeners();

			NetworkThread.getInstance().addListener(this);

		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	private void createColumns() {
		c_name = new TableColumn(table, SWT.LEFT);
		c_name.setText("Name");
		c_name.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				if (sortBy == JussFileComparator.NAME && fileComp != null)
					fileComp.reverse();
				else
					sortByName();
				TreeItem[] items = tree.getSelection();
				if (items.length < 1)
					return;
				doFileListing(items[0], false);
			}
		});
		c_size = new TableColumn(table, SWT.RIGHT);
		c_size.setText("Size");
		c_size.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				if (sortBy == JussFileComparator.SIZE && fileComp != null)
					fileComp.reverse();
				else
					sortBySize();
				TreeItem[] items = tree.getSelection();
				if (items.length < 1)
					return;
				doFileListing(items[0], false);
			}
		});
		c_version = new TableColumn(table, SWT.RIGHT);
		c_version.setText("Version");
		c_version.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				if (sortBy == JussFileComparator.VERSION && fileComp != null)
					fileComp.reverse();
				else
					sortByVersion();
				TreeItem[] items = tree.getSelection();
				if (items.length < 1)
					return;
				doFileListing(items[0], false);
			}
		});
		c_modified = new TableColumn(table, SWT.RIGHT);
		c_modified.setText("Modified");
		c_modified.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				if (sortBy == JussFileComparator.MODTIME && fileComp != null)
					fileComp.reverse();
				else
					sortByDate();
				TreeItem[] items = tree.getSelection();
				if (items.length < 1)
					return;
				doFileListing(items[0], false);
			}
		});

	}

	private void createTreeRMBMenu() {
		treeMenu = new Menu(MainWindow.getWindow().getShell(), SWT.POP_UP);
		tree.setMenu(treeMenu);

		treeMenu.addListener(SWT.Show, new Listener() {
			public void handleEvent(Event event) {
				createDynamicTreeMenu();
			}
		});
	}

	private void createDynamicTreeMenu() {
		TreeItem[] it = tree.getSelection();
		MenuItem[] menuItems = treeMenu.getItems();
		for (int i = 0; i < menuItems.length; i++) {
			menuItems[i].dispose();
		}
		createRefreshMenuItem(treeMenu);
		if (it.length > 0) {
			createUploadMenuItem(treeMenu);
			createNewDirectoryMenuItem(treeMenu);
			if (isRoot(it[0])) {
				createLoginMenuItem(treeMenu);
				createCloseConnectionMenuItem(treeMenu);
			}
		}
	}

	private void createTreeListeners() {
		tree.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (e.item instanceof TreeItem)
					doListing((TreeItem) e.item, false, true, false);
			}
		});

		tree.addTreeListener(new TreeAdapter() {
			public void treeCollapsed(TreeEvent e) {
				if (e.item instanceof TreeItem) {
					doTreeCollapse((TreeItem) e.item);
				}
			}

			public void treeExpanded(TreeEvent e) {
				if (e.item instanceof TreeItem) {
					doTreeExpand((TreeItem) e.item);
				}
			}
		});

		tree.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				onTreeMouseUp(e);
			}
		});
	}

	private void createTableRMBMenu() {
		tableMenu = new Menu(MainWindow.getWindow().getShell(), SWT.POP_UP);
		table.setMenu(tableMenu);
		tableMenu.addListener(SWT.Show, new Listener() {
			public void handleEvent(Event event) {
				createDynamicTableMenu();
			}
		});
	}

	private void createDynamicTableMenu() {
		TableItem[] it = table.getSelection();
		MenuItem[] menuItems = tableMenu.getItems();
		for (int i = 0; i < menuItems.length; i++) {
			menuItems[i].dispose();
		}
		if (it.length > 0) {
			boolean modified = false;
			boolean newer = false;
			boolean versions = false;
			boolean files = false;
			CConnection con = getCurrentConnection();
			for (int i = 0; i < it.length; i++) {
				if (modified == true && newer == true)
					break;
				JussFile jf = con.getFile(getId(it[i]));
				if (jf.getExists()) {
					int mod = con.checkModTime(jf);
					if (mod == 1)
						modified = true;
					else if (mod == -1)
						newer = true;
				}
				if (jf.hasVersions()) {
					versions = true;
				}
				if (!jf.isFolder())
					files = true;
			}
			if (files)
				createDownloadMenuItem(tableMenu);
			if (modified)
				createCommitMenuItem(tableMenu);
			if (newer)
				createUpdateMenuItem(tableMenu);
			if (versions)
				createViewVersionsMenuItem(tableMenu);
			createDeleteMenuItem(tableMenu);
		}
		createUploadMenuItem(tableMenu);
		createRefreshMenuItem(tableMenu);
	}

	private void createTableListeners() {
		table.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (e.item instanceof TableItem)
					doMeta((TableItem) e.item);
			}
		});

		table.addMouseListener(new MouseAdapter() {
			public void mouseDoubleClick(MouseEvent e) {
				doFileOpen();
			}
		});
	}

	private void createCloseConnectionMenuItem(Menu m) {
		final MenuItem closeConnection = new MenuItem(m, SWT.PUSH);
		closeConnection.setText("Close");

		closeConnection.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				doCloseConnection();
			}
		});
	}

	private void createLoginMenuItem(Menu m) {
		final MenuItem login = new MenuItem(m, SWT.PUSH);
		login.setText("Login");

		login.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				TreeItem[] items = tree.getSelection();
				if (items.length < 1)
					return;
				doLogin(items[0]);
			}
		});

	}

	private void createNewDirectoryMenuItem(Menu m) {
		final MenuItem newDir = new MenuItem(m, SWT.PUSH);
		newDir.setText("Make new directory");

		newDir.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				makeDirectory();
			}
		});
	}

	private void createRefreshMenuItem(Menu m) {
		final MenuItem refreshProject = new MenuItem(m, SWT.PUSH);
		refreshProject.setText("Refresh");

		refreshProject.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				doListing(true);
			}
		});
	}

	private void createDownloadMenuItem(Menu m) {
		final MenuItem fileDownload = new MenuItem(m, SWT.PUSH);
		fileDownload.setText("Download");

		fileDownload.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				TableItem[] items = table.getSelection();
				for (int i = 0; i < items.length; i++) {
					doDownload(items[i]);
				}
			}
		});
	}

	private void createUploadMenuItem(Menu m) {
		final MenuItem upload = new MenuItem(m, SWT.PUSH);
		upload.setText("Upload");
		upload.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				onUpload();
			}
		});
	}

	private void createCommitMenuItem(Menu m) {
		final MenuItem upload = new MenuItem(m, SWT.PUSH);
		upload.setText("Commit");
		upload.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				onCommit();
			}
		});
	}

	private void createUpdateMenuItem(Menu m) {
		final MenuItem upload = new MenuItem(m, SWT.PUSH);
		upload.setText("Update");
		upload.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				onUpdate();
			}
		});
	}

	private void createViewVersionsMenuItem(Menu m) {
		final MenuItem viewVersions = new MenuItem(m, SWT.PUSH);
		viewVersions.setText("View Versions");
		viewVersions.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				onViewVersions();
			}
		});
	}

	private void createDeleteMenuItem(Menu m) {
		final MenuItem delete = new MenuItem(m, SWT.PUSH);
		delete.setText("Delete");
		delete.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				onDelete();
			}
		});
	}

	private void createDragNDrop() {
		DropTarget target = new DropTarget(tableComp, DND.DROP_MOVE
				| DND.DROP_COPY | DND.DROP_DEFAULT);
		final FileTransfer fileTransfer = FileTransfer.getInstance();
		Transfer[] types = new Transfer[] { fileTransfer };

		target.setTransfer(types);
		target.addDropListener(new DropTargetAdapter() {
			public void dragEnter(DropTargetEvent event) {
				CConnection con = getCurrentConnection();
				if (con == null || !con.isConnected()) {
					event.detail = DND.DROP_NONE;
					return;
				}
				if (event.detail == DND.DROP_DEFAULT) {
					if ((event.operations & DND.DROP_COPY) != 0)
						event.detail = DND.DROP_COPY;
					else
						event.detail = DND.DROP_NONE;
				}
			}

			public void dragOver(DropTargetEvent event) {
				event.feedback = DND.FEEDBACK_NONE;
				// event.feedback = DND.FEEDBACK_SCROLL |
				// DND.FEEDBACK_INSERT_AFTER;
			}

			public void dragOperationChanged(DropTargetEvent event) {
				if (event.detail == DND.DROP_DEFAULT) {
					event.detail = DND.DROP_COPY;
				}
			}

			public void drop(DropTargetEvent event) {
				onTableDrop(event);
			}
		});
	}

	public Composite getComposite() {
		return main;
	}

	public void refresh() {
	}

	public void delete() {
		MainWindow.getWindow().getShell().setFocus();
	}

	public Table getTable() {
		return table;
	}

	public Tree getTree() {
		return tree;
	}

	private boolean isRoot(TreeItem item) {
		return item.getParentItem() == null;
	}

	public boolean isListed(TreeItem item) {
		Boolean b = (Boolean) item.getData("listed");
		if (b == null)
			return false;
		return b.booleanValue();
	}

	private void doTreeCollapse(TreeItem item) {
		if (!isRoot(item))
			item.setImage(ImageRepository.getImage("folder_closed"));
	}

	private void doTreeExpand(TreeItem item) {
		if (!isRoot(item))
			item.setImage(ImageRepository.getImage("folder_open"));
		TreeItem[] children = item.getItems();
		for (int i = 0; i < children.length; i++) {
			doListing(children[i], false, false, false);
		}
	}

	public void refreshTree() {
		TreeItem[] connections = tree.getItems();
		for (int i = 0; i < connections.length; i++)
			doListing(connections[i], true, true, true);
	}

	public void refreshFileListing() {
		TreeItem[] items = tree.getSelection();
		if (items.length < 1)
			return;
		doListing(items[0], false, true, false);
	}

	public void doListing(boolean remote) {
		TreeItem[] items = tree.getSelection();
		if (items.length < 1)
			return;
		doListing(items[0], true, true, remote);
	}

	public void doListing(TreeItem item, boolean force, boolean files,
			boolean remote) {
		boolean isListed = isListed(item);
		if (!force && !files && isListed)
			return;
		if (!isListed)
			remote = true;
		CConnection con = getConnection(item);
		Long id = getId(item);
		// TODO: Ajutine lahendus. FIX IT!
		con.getDBManager().closeSession();
		java.util.List list = con.getListing(id, remote, FileManager.FOLDERS
				| (files ? FileManager.FILES : 0));
		if (force || !isListed)
			doFolderListing(item, true, remote, list, con);
		if (files)
			doFileListing(list, con);
	}

	public void sortByName() {
		sortBy = JussFileComparator.NAME;
		fileComp = new JussFileComparator(JussFileComparator.TYPE);
		fileComp.by(JussFileComparator.NAME);
		fileComp.by(JussFileComparator.VERSION);
	}

	public void sortByDate() {
		sortBy = JussFileComparator.MODTIME;
		fileComp = new JussFileComparator(JussFileComparator.MODTIME);
		fileComp.by(JussFileComparator.TYPE);
		fileComp.by(JussFileComparator.NAME);
		fileComp.by(JussFileComparator.VERSION);
	}

	public void sortByVersion() {
		sortBy = JussFileComparator.VERSION;
		fileComp = new JussFileComparator(JussFileComparator.VERSION);
		fileComp.by(JussFileComparator.TYPE);
		fileComp.by(JussFileComparator.NAME);
	}

	public void sortBySize() {
		sortBy = JussFileComparator.SIZE;
		fileComp = new JussFileComparator(JussFileComparator.SIZE);
	}

	public void doFileListing(TreeItem item, boolean remote) {
		CConnection con = getConnection(item);
		Long id = getId(item);
		java.util.List list = con.getListing(id, remote, FileManager.FILES);
		doFileListing(list, con);
	}

	public void doFileListing(final List list, final CConnection con) {
		Runnable task = new Runnable() {
			public void run() {
				if (fileComp == null)
					sortByName();
				Collections.sort(list, fileComp);
				table.removeAll();
				Set displayedFiles = new HashSet();
				for (int i = 0; i < list.size(); i++) {
					JussFile listItem = (JussFile) (list.get(i));
					if (!listItem.getMainversion().equals(listItem.getId())
							&& !listItem.getExists())
						continue;
					if (!displayedFiles.add(listItem.getMainversion()))
						continue;
					addTableFile(con, listItem);
				}
			}
		};
		BusyIndicator.showWhile(display, task);
	}

	public void addTableFile(CConnection con, JussFile jf) {
		TableItem ti = new TableItem(table, SWT.NONE);
		if (jf.isFolder())
			ti.setImage(ImageRepository.getImage("folder_closed"));
		else {
			ti.setImage(ImageRepository.getImage("file"));
		}
		ti.setText(new String[] { jf.getName(), jf.getHumanReadableSize(),
				"" + jf.getVersion(), jf.getModtime().toString() });
		if (jf.getExists()) {
			int modified = con.checkModTime(jf);
			if (!jf.getId().equals(jf.getMainversion())) {
				ti.setForeground(display.getSystemColor(SWT.COLOR_DARK_GRAY));
			} else if (modified == 1) {
				ti.setForeground(display.getSystemColor(SWT.COLOR_RED));
			} else if (modified == -1) {
				ti.setForeground(display.getSystemColor(SWT.COLOR_BLUE));
			} else {
				ti.setForeground(display.getSystemColor(SWT.COLOR_DARK_GREEN));
			}
		}
		ti.setData("connection", con);
		ti.setData("id", jf.getId());

	}

	public void doFolderListing(TreeItem item, boolean first, boolean remote) {
		CConnection con = getConnection(item);
		Long id = getId(item);
		java.util.List list = con.getListing(id, remote, FileManager.FOLDERS);
		doFolderListing(item, first, remote, list, con);
	}

	public void doFolderListing(final TreeItem item, final boolean first,
			final boolean remote, final List list, final CConnection con) {
		final boolean expanded = item.getExpanded();
		Runnable task = new Runnable() {
			public void run() {
				JussFileComparator comp = new JussFileComparator(
						JussFileComparator.NAME);
				Collections.sort(list, comp);
				clear(item);
				TreeItem ti;
				for (int i = 0; i < list.size(); i++) {
					JussFile listItem = (JussFile) (list.get(i));
					if (!listItem.isFolder())
						continue;
					ti = new TreeItem(item, SWT.NONE);
					ti.setImage(ImageRepository.getImage("folder_closed"));
					ti.setText(listItem.getName());
					ti.setData("id", listItem.getId());
					ti.setData("connection", con);
					if (listItem.isFolder()) {
						if (first)
							doFolderListing(ti, false, remote);
					}
				}
			}
		};
		BusyIndicator.showWhile(display, task);
		if (first)
			item.setData("listed", new Boolean(true));
		item.setExpanded(expanded);
	}

	private void clear(TreeItem item) {
		clear(item.getItems());
	}

	private void clear(Tree tree) {
		clear(tree.getItems());
	}

	private void clear(TreeItem[] items) {
		for (int i = 0; i < items.length; i++) {
			items[i].dispose();
		}
	}

	public void doMeta() {
		TableItem[] items = table.getSelection();
		if (items.length < 1)
			return;
		doMeta(items[0]);
	}

	public void doMeta(final TableItem item) {
		Runnable task = new Runnable() {
			public void run() {
				meta.setMeta(getConnection(item), getId(item));
			}
		};
		BusyIndicator.showWhile(display, task);
	}

	public static Long getId(Item item) {
		return (Long) (item.getData("id"));
	}

	public static CConnection getConnection(Item item) {
		return (CConnection) (item.getData("connection"));
	}

	public CConnection getCurrentConnection() {
		TreeItem[] items = tree.getSelection();
		if (items.length < 1)
			return null;
		return getConnection(items[0]);
	}

	public CConnection[] getConnections() {
		TreeItem[] items = tree.getItems();
		CConnection[] cons = new CConnection[items.length];
		for (int i = 0; i < items.length; i++) {
			cons[i] = getConnection(items[i]);
		}
		return cons;
	}

	public void addConnection(CConnection con) {
		TreeItem ti = new TreeItem(tree, SWT.NONE);
		ti.setText(con.getName());
		ti.setData("connection", con);
		ti.setData("id", new Long(1));
		ti.setExpanded(false);
		ti.setImage(ImageRepository.getImage("offline"));
		TreeItem[] tl = { ti };
		tree.setSelection(tl);
		doListing(ti, false, true, false);
		doLogin(ti);
	}

	public void doLogin(final TreeItem item) {
		Runnable task = new Runnable() {
			public void run() {
				CConnection con = getConnection(item);
				item.setImage(ImageRepository.getImage("offline"));
				do {
					HashMap settings = LoginDialog.init(MainWindow.getWindow()
							.getShell());
					if (settings == null) {
						return;
					}

					CStatus.getInstance().setStatus("Logging in");
					if (con.login((String) settings.get("user"),
							(String) settings.get("password"))) {
						item.setImage(ImageRepository.getImage("online"));
						refreshTree();
						CStatus.getInstance().setStatus("");
						return;
					} else {
						if (!MainWindow.getWindow().getConfirmation(
								"Login Failed", "Login failed - try again?")) {
							break;
						}
					}
				} while (true);
				CStatus.getInstance().setStatus("");
			}
		};
		BusyIndicator.showWhile(display, task);
	}

	private void doCloseConnection() {
		TreeItem[] item = tree.getSelection();
		if (item.length < 1)
			return;
		JussClient.closeConnection(getConnection(item[0]).getRoot());
		item[0].dispose();
		table.removeAll();
		item = tree.getItems();
		if (item.length < 1)
			return;
		tree.setSelection(new TreeItem[] { item[0] });
		doListing(item[0], false, true, false);

	}

	private void onTreeMouseUp(MouseEvent e) {
		// TODO: IMPLEMENT ME
	}

	private void makeDirectory() {
		TreeItem[] items = tree.getSelection();
		if (items.length < 1)
			return;
		TreeItem item = tree.getSelection()[0];
		CConnection con = getConnection(item);

		// Bad style
		// TODO: fix it!
		Rectangle r1 = MainWindow.getWindow().getShell().getBounds();
		int sX = r1.x + (r1.width / 2) - (347 / 2);
		int sY = r1.y + (r1.height / 2) - (275 / 2);

		String[] settings = GuiImportFileFrm.init(null, 1, sX, sY, MainWindow
				.getWindow().getShell());

		if (settings[0] == null)
			return;
		else {
			CMetadata meta = new CMetadata("juss", settings[0]);
			meta.setKeywords(settings[1]);
			con.createFolder(getId(item), settings[2], meta);
			doListing(item, true, true, false);
			item.setExpanded(true);
		}
	}

	private void doFileOpen() {
		TableItem[] t = table.getSelection();
		if (t.length < 1)
			return;
		CConnection con = getConnection(t[0]);
		Long id = getId(t[0]);
		JussFile jf = con.getFile(id);
		if (jf.isFolder()) {
			doFolderOpen(t[0].getText());
			return;
		}
		if (!jf.getExists())
			return;
		Program.launch(con.getRoot() + con.getDBManager().getStringPath(jf));
	}

	public void doFolderOpen(String name) {
		TreeItem[] parentFolder = tree.getSelection();
		if (parentFolder.length < 1)
			return;
		TreeItem[] children = parentFolder[0].getItems();
		for (int i = 0; i < children.length; i++) {
			if (children[i].getText().equals(name)) {
				tree.setSelection(new TreeItem[] { children[i] });
				doListing(children[i], false, true, false);
				break;
			}
		}
	}

	public void openFolder(Long[] ids, CConnection con) {
		if (ids.length == 0)
			return;
		TreeItem[] connections = tree.getItems();
		if (connections.length < 1)
			return;
		for (int i = 0; i < connections.length; i++) {
			if (((CConnection) (connections[i].getData("connection"))) == con) {
				if (ids.length == 1) {
					tree.setSelection(new TreeItem[] { connections[i] });
					doListing(connections[i], false, true, false);
				} else {
					ArrayList lids = new ArrayList();
					for (int j = 1; j < ids.length; j++) {
						lids.add(ids[j]);
					}
					openFolderRecursive(lids, connections[i]);
				}
				break;
			}
		}
	}

	private void openFolderRecursive(ArrayList ids, TreeItem item) {
		if (ids.size() == 0)
			return;
		doListing(item, false, false, false);
		TreeItem[] children = item.getItems();
		for (int i = 0; i < children.length; i++) {
			if (((Long) (children[i].getData("id"))).equals(ids.get(0))) {
				if (ids.size() == 1) {
					tree.setSelection(new TreeItem[] { children[i] });
					doListing(children[i], false, true, false);
				} else {
					ids.remove(0);
					openFolderRecursive(ids, children[i]);
				}
				break;
			}
		}
	}

	private void doDownload(final TableItem item) {
		Runnable task = new Runnable() {
			public void run() {
				CConnection con = getConnection(item);
				JussClient.downloadFile(getId(item), con.getRoot());
			}
		};
		BusyIndicator.showWhile(display, task);
	}

	private void doUpload(String[] files) {
		// TODO: UGLY ASS SOLUTIONS TO IT ALL. FIX IT.
		if (files == null || files.length < 1)
			return;
		CConnection con = getCurrentConnection();
		if (con == null || !con.isConnected())
			return;
		TreeItem[] items = tree.getSelection();
		if (items.length < 1)
			return;
		CMetadata meta = null;
		boolean globalMeta = false;
		if (files.length > 1) {
			globalMeta = MainWindow
					.getWindow()
					.getConfirmation(
							"Multiple file upload",
							"You are uploading multiple files at once. Do you want to use the same metadata for all of them?");
			if (globalMeta) {
				Rectangle r1 = MainWindow.getWindow().getShell().getBounds();
				int sX = r1.x + (r1.width / 2) - (347 / 2);
				int sY = r1.y + (r1.height / 2) - (275 / 2);

				String[] settings = GuiImportFileFrm.init(new File(
						"Multiple files"), 0, sX, sY, MainWindow.getWindow()
						.getShell());

				if (settings[0] == null)
					return;

				meta = new CMetadata("juss", settings[0]);
				meta.setKeywords(settings[1]);
			}
		}
		for (int i = 0; i < files.length; i++) {
			File fileToAdd = new File(files[i]);
			if (files[i] == null || !fileToAdd.isFile())
				continue;

			boolean overwrite = true;

			if (con.getDBManager().getId(getId(items[0]), fileToAdd.getName()) != null) {
				if (MainWindow
						.getWindow()
						.getConfirmation(
								"New version or overwrite",
								"The file already exists. Do you wish to create a new version?(answering No will overwite the current file)"))
					overwrite = false;
			} else
				overwrite = false;

			if (!globalMeta) {
				if (!overwrite) {
					// Bad style
					// TODO: FIX IT
					Rectangle r1 = MainWindow.getWindow().getShell()
							.getBounds();
					int sX = r1.x + (r1.width / 2) - (347 / 2);
					int sY = r1.y + (r1.height / 2) - (275 / 2);

					String[] settings = GuiImportFileFrm.init(fileToAdd, 0, sX,
							sY, MainWindow.getWindow().getShell());

					if (settings[0] == null)
						return;

					meta = new CMetadata("juss", settings[0]);
					meta.setKeywords(settings[1]);
				} else {
					meta = new CMetadata("juss", "");
				}
			}
			JussClient.uploadFile(fileToAdd.getPath(), getId(items[0]), meta,
					(overwrite ? FileManager.OVERWRITE
							: FileManager.NEW_VERSION), con.getRoot());

		}
	}

	private void onTableDrop(DropTargetEvent e) {
		String[] files = (String[]) e.data;
		doUpload(files);
	}

	private void onUpload() {
		FileDialog fd = new FileDialog(MainWindow.getWindow().getShell(),
				SWT.OPEN | SWT.MULTI);
		fd.setText("Choose files to upload");
		fd.open();
		String[] files = fd.getFileNames();
		String filterPath = fd.getFilterPath();
		char endchar = filterPath.charAt(filterPath.length() - 1);
		if (endchar != '/' && endchar != '\\')
			filterPath += "/";
		for (int i = 0; i < files.length; i++) {
			files[i] = filterPath + files[i];
		}
		doUpload(files);
	}

	private void onCommit() {
		TableItem[] t = table.getSelection();
		if (t.length < 1)
			return;
		CConnection con = getCurrentConnection();
		ArrayList files = new ArrayList();
		for (int i = 0; i < t.length; i++) {
			JussFile jf = con.getFile(getId(t[i]));
			int modified = con.checkModTime(jf);
			if (modified == 1) {
				files.add(con.getRoot() + con.getDBManager().getStringPath(jf));
			}
		}
		if (files.size() > 0) {
			String[] tempArray = new String[files.size()];
			files.toArray(tempArray);
			doUpload(tempArray);
		}
	}

	private void onUpdate() {
		TableItem[] t = table.getSelection();
		if (t.length < 1)
			return;
		CConnection con = getCurrentConnection();
		for (int i = 0; i < t.length; i++) {
			JussFile jf = con.getFile(getId(t[i]));
			int modified = con.checkModTime(jf);
			if (modified == -1) {
				doDownload(t[i]);
			}
		}
	}

	public void saveConf() {
		CConfig conf = JussClient.getConf();
		conf.setValue("GUI/BrowserView/sash.weight1", sash.getWeights()[0]);
		conf.setValue("GUI/BrowserView/sash.weight2", sash.getWeights()[1]);
		conf.setValue("GUI/BrowserView/rightSide.weight1", rightSide
				.getWeights()[0]);
		conf.setValue("GUI/BrowserView/rightSide.weight2", rightSide
				.getWeights()[1]);
		conf.setValue("GUI/BrowserView/filesTable.column.name", c_name
				.getWidth());
		conf.setValue("GUI/BrowserView/filesTable.column.size", c_size
				.getWidth());
		conf.setValue("GUI/BrowserView/filesTable.column.version", c_version
				.getWidth());
		conf.setValue("GUI/BrowserView/filesTable.column.modified", c_modified
				.getWidth());
		conf.save();
		meta.saveConf();
	}

	public void loadConf() {
		CConfig conf = JussClient.getConf();
		Integer w1, w2;
		w1 = conf.getIntValue("GUI/BrowserView/sash.weight1");
		w2 = conf.getIntValue("GUI/BrowserView/sash.weight2");
		if (w1 != null && w2 != null)
			sash.setWeights(new int[] { w1.intValue(), w2.intValue() });
		else
			sash.setWeights(new int[] { 3, 10 });
		w1 = conf.getIntValue("GUI/BrowserView/rightSide.weight1");
		w2 = conf.getIntValue("GUI/BrowserView/rightSide.weight2");
		if (w1 != null && w2 != null)
			rightSide.setWeights(new int[] { w1.intValue(), w2.intValue() });
		else
			rightSide.setWeights(new int[] { 10, 2 });

		w1 = conf.getIntValue("GUI/BrowserView/filesTable.column.name");
		if (w1 != null)
			c_name.setWidth(w1.intValue());
		else
			c_name.setWidth(200);
		w1 = conf.getIntValue("GUI/BrowserView/filesTable.column.size");
		if (w1 != null)
			c_size.setWidth(w1.intValue());
		else
			c_size.setWidth(80);
		w1 = conf.getIntValue("GUI/BrowserView/filesTable.column.version");
		if (w1 != null)
			c_version.setWidth(w1.intValue());
		else
			c_version.setWidth(50);
		w1 = conf.getIntValue("GUI/BrowserView/filesTable.column.modified");
		if (w1 != null)
			c_modified.setWidth(w1.intValue());
		else
			c_modified.setWidth(150);
	}

	public void networkTaskFinished(final NetworkTask task) {
		if (((String) task.getData("type")).equals("ListingTask")) {
			if (((String) task.getData("update")).equals("false"))
				return;
			display.syncExec(new Runnable() {
				public void run() {
					Long id = (Long) task.getData("id");
					CConnection con = (CConnection) task.getData("connection");
					if (con == getCurrentConnection()) {
						if (tree.getSelectionCount() < 1)
							return;
						TreeItem[] it = tree.getSelection();
						if (getId(it[0]).equals(id)) {
							log.debug("Doing listing because of network sync: "
									+ task);
							doListing(it[0], true, true, false);
						} else {
							log
									.debug("Doing tree listing because of network sync: "
											+ task);
							doFolderListing(
									findTreeItem(id, findTreeItem(con)), true,
									false);
						}
					}
				}
			});
		}
	}

	public TreeItem findTreeItem(Long id, TreeItem root) {
		TreeItem[] items = root.getItems();
		for (int i = 0; i < items.length; i++) {
			if (getId(items[i]).equals(id))
				return items[i];
		}

		for (int i = 0; i < items.length; i++) {
			TreeItem it = findTreeItem(id, items[i]);
			if (it != null)
				return it;
		}

		return null;
	}

	public TreeItem findTreeItem(CConnection con) {
		TreeItem[] items = tree.getItems();
		for (int i = 0; i < items.length; i++) {
			if (getConnection(items[i]).equals(con))
				return items[i];
		}
		return null;
	}

	public void onViewVersions() {
		if (table.getSelectionCount() < 1)
			return;
		Runnable task = new Runnable() {
			public void run() {
				TableItem[] items = table.getSelection();
				CConnection con = getCurrentConnection();
				List list = new ArrayList();
				for (int i = 0; i < items.length; i++) {
					JussFile jf = con.getFile(getId(items[i]));
					if (jf.hasVersions()) {
						list.addAll(con.getDBManager().getVersions(jf));
					}
				}
				table.removeAll();
				if (fileComp == null)
					sortByName();
				Collections.sort(list, fileComp);
				Iterator it = list.iterator();
				while (it.hasNext()) {
					addTableFile(con, (JussFile) it.next());
				}
			}
		};
		BusyIndicator.showWhile(display, task);
	}

	public void onDelete() {
		if (table.getSelectionCount() < 1)
			return;
		if (!MainWindow
				.getWindow()
				.getConfirmation(
						"Delete",
						"Do you really want to delete the selected files? (this removes them from the server permanently)")) {
			return;
		}
		Runnable task = new Runnable() {
			public void run() {
				TableItem[] items = table.getSelection();
				CConnection con = getCurrentConnection();
				boolean deletedFolder = false;
				for (int i = 0; i < items.length; i++) {
					JussFile jf = con.getFile(getId(items[i]));
					if (!jf.isFolder()) {
						if (con.removeFile(jf.getId(), FileManager.FILES)) {
							log.info("Deleted " + jf.getName());
						} else {
							log.warn("Could not delete " + jf.getName());
						}
					} else {
						if(deleteFolder(con, jf)) {
							deletedFolder = true;
						}
					}
				}
				if (deletedFolder)
					refreshTree();
				else
					refreshFileListing();
			}
		};
		BusyIndicator.showWhile(display, task);
	}

	private boolean deleteFolder(CConnection con, JussFile jf) {
		List list = con.getListing(jf.getId(), true, FileManager.FOLDERS
				| FileManager.FILES);
		Iterator it = list.iterator();
		boolean delete = true; 
		while(it.hasNext()) {
			JussFile subJf = (JussFile)it.next();
			if(subJf.isFolder()) {
				if(!deleteFolder(con, subJf))
					delete = false;
			} else {
				if(con.removeFile(subJf.getId(), FileManager.FILES)) {
					log.info("Deleted " + subJf.getName());
				} else {
					log.warn("Could not delete " + subJf.getName());
					delete = false;
				}
			}
		}
		if(delete) {
			if(con.removeFile(jf.getId(), FileManager.FOLDERS)) {
				log.info("Deleted folder " + jf.getName());
				return true;
			} else {
				log.warn("Could not delete folder " + jf.getName());
			}
		}
		return false;
	}
}
