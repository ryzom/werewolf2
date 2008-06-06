/*
 * Created on May 4, 2005
 *
 */
package client.gui;

import java.util.Collections;
import java.util.Iterator;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.program.Program;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;

import shared.JussFile;
import shared.JussFileComparator;
import shared.SearchCriteria;
import client.CConnection;

/**
 * @author Henri Kuuste
 * 
 */
public class SearchTableView implements IView {

	SearchCriteria search;

	String shortTitle;

	Display display;

	Table main;

	public SearchTableView(SearchCriteria c) {
		search = c;
		if (c.getString().length() > 20) {
			shortTitle = c.getString().substring(0, 17) + "...";
		} else {
			shortTitle = c.getString();
		}
	}

	public String getShortTitle() {
		return shortTitle;
	}

	public String getFullTitle() {
		return search.getString();
	}

	public void initialize(Composite composite) {
		try {
			display = composite.getDisplay();
			main = new Table(composite, SWT.SINGLE);
			main.setHeaderVisible(true);
			TableColumn col = new TableColumn(main, SWT.LEFT);
			col.setText("Name");
			col.setWidth(200);
			col = new TableColumn(main, SWT.RIGHT);
			col.setText("Size");
			col.setWidth(80);
			col = new TableColumn(main, SWT.RIGHT);
			col.setText("Version");
			col.setWidth(50);
			col = new TableColumn(main, SWT.RIGHT);
			col.setText("Modified");
			col.setWidth(150);
			col = new TableColumn(main, SWT.RIGHT);
			col.setText("Connection");
			col.setWidth(100);
			col = new TableColumn(main, SWT.LEFT);
			col.setText("Location");
			col.setWidth(200);

			createRMBMenu();
			search();
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}
	
	public void search(SearchCriteria c) {
		search = c;
		search();
	}
	
	public void search() {
		TableItem item;
		main.removeAll();
		CConnection[] cons = MainWindow.getWindow().getConnections();
		for (int con = 0; con < cons.length; con++) {
			java.util.List s_list = cons[con].search(search);
			JussFileComparator comp = new JussFileComparator(JussFileComparator.TYPE);
			comp.by(JussFileComparator.NAME);
			comp.by(JussFileComparator.VERSION);
			Collections.sort(s_list, comp);
			Iterator i = s_list.iterator();
			while (i.hasNext()) {
				JussFile jf = (JussFile) i.next();
				item = new TableItem(main, SWT.NONE);
				if (jf.isFolder()) {
					item
							.setImage(ImageRepository
									.getImage("folder_closed"));
				} else {
					item.setImage(ImageRepository.getImage("file"));
				}
				item.setText(new String[] { jf.getName(),
						jf.getHumanReadableSize(),
						"" + jf.getVersion(),
						jf.getModtime().toString(),
						cons[con].getName(),
						cons[con].getDBManager().getStringPath(jf)});
				item.setData("id", jf.getId());
				item.setData("connection", cons[con]);
			}
		}
	}

	
	private void createRMBMenu() {
		main.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
//				doTableMouseUp(e, fileMenu);
			}

			public void mouseDoubleClick(MouseEvent e) {
				doFileOpen();
			}
		});
	}
	
	private void doFileOpen() {
		TableItem[] t = main.getSelection();
		if (t.length < 1)
			return;
		CConnection con = BrowserView.getConnection(t[0]);
		Long id = BrowserView.getId(t[0]);
		JussFile jf = con.getFile(id);
		if (jf.isFolder()) {
			Long[] ancestors = jf.getLongAncestors();
			Long[] ids = new Long[ancestors.length + 1];
			System.arraycopy(ancestors, 0, ids, 0, ancestors.length);
			ids[ids.length-1] = jf.getId();
			MainWindow.getWindow().openFolder(ids, con);
			return;
		}
		if (!jf.getExists())
			return;
		Program.launch(con.getRoot() + con.getDBManager().getStringPath(jf));
	}

	public Composite getComposite() {
		return main;
	}

	public void refresh() {
	}

	public void delete() {
		// MainWindow.getWindow().getShell().setFocus();
	}

}
