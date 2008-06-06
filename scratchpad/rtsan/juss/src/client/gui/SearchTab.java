/*
 * Created on May 4, 2005
 *
 */
package client.gui;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabFolder2Adapter;
import org.eclipse.swt.custom.CTabFolderEvent;
import org.eclipse.swt.custom.CTabItem;

import shared.SearchCriteria;

/**
 * @author Henri Kuuste
 * 
 */
public class SearchTab {
	private static HashMap tabs;

	private static CTabFolder _folder;

	private CTabFolder folder;

	private CTabItem tabItem;

	private static CTabItem selectedItem = null;

	private SearchTableView view;

	static {
		tabs = new HashMap();
	}

	public CTabItem getTabItem() {
		return tabItem;
	}

	public SearchTab(SearchCriteria c) {
		if(_folder == null)
			return;
		if(tabs.containsKey(c.getString())) {
			SearchTableView v = (SearchTableView)((CTabItem)tabs.get(c.getString())).getData();
			v.search(c);
			_folder.setSelection((CTabItem)(tabs.get(c.getString())));
			return;
		}
		view = new SearchTableView(c);
		folder = _folder;
		tabItem = new CTabItem(folder, SWT.NULL);

		try {
			view.initialize(folder);
			// _view.setTabListener();
			tabItem.setText(view.getShortTitle());
			tabItem.setControl(view.getComposite());
			tabItem.setToolTipText(view.getFullTitle());
			folder.setSelection(tabItem);
			tabItem.setData(view);
			tabs.put(c.getString(), tabItem);
		} catch (Exception e) {
			e.printStackTrace();
		}

		folder.addCTabFolder2Listener(new CTabFolder2Adapter() {
			public void close(CTabFolderEvent e) {
				closed((CTabItem)e.item);
			}
		});

		// MainWindow.getWindow().refreshIconBar();
		selectedItem = tabItem;
		folder.setSelection(tabItem);
	}

	public static SearchTableView getView(CTabItem item) {
		return (SearchTableView) item.getData();
	}

	public static void closeAllTabs() {
		Set keys;
		keys = tabs.keySet();
		Iterator it = keys.iterator();
		while(it.hasNext()) {
			closed((CTabItem)it.next());
		}
	}

	public static void setFolder(CTabFolder folder) {
		_folder = folder;
	}

	public static void closed(CTabItem item) {
		SearchTableView view = (SearchTableView)item.getData();
		if (view != null) {
			try {
				tabs.remove(view.getFullTitle());
				view.delete();
				item.dispose();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
}
