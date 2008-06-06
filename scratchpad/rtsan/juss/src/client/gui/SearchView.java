/*
 * Created on May 4, 2005
 *
 */
package client.gui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CLabel;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;

import shared.SearchCriteria;

/**
 * @author Henri Kuuste
 *
 */
public class SearchView implements IView {

	private Display display;
	private Composite main;
	private Composite searchArea;
	private CTabFolder folder;
	private Button keywords;
	private Button filename;
	private Button description;
	
	public void delete() {
	  	MainWindow.getWindow().getShell().setFocus();
	}

	public Composite getComposite() {
		return main;
	}

	public void initialize(Composite composite) {
		try {
			display = composite.getDisplay();

			main = new Composite(composite, SWT.NONE);
			main.setLayout(new FillLayout());
			main.setBackground(display.getSystemColor(SWT.COLOR_WHITE));
			LiveSashForm sash = new LiveSashForm(main, SWT.VERTICAL);
			sash.setBackground(display.getSystemColor(SWT.COLOR_DARK_GRAY));

			searchArea = new Composite(sash, SWT.NONE);
			folder = new CTabFolder(sash, SWT.CLOSE | SWT.FLAT);
			
			searchArea.setLayout(new FillLayout(SWT.HORIZONTAL));
			CLabel l = new CLabel(searchArea, SWT.NONE);
			l.setText("Search based on: ");
			keywords = new Button(searchArea, SWT.CHECK);
			keywords.setText("Keywords");
			keywords.setSelection(true);
			filename = new Button(searchArea, SWT.CHECK);
			filename.setText("Filenames");
			description = new Button(searchArea, SWT.CHECK);
			description.setText("Descriptions");
			
			sash.setWeights(new int[] {1, 10});

			
			folder.setSimple(false);
			try {
				folder.setSelectionBackground(new Color[] {
						display.getSystemColor(SWT.COLOR_LIST_BACKGROUND),
						display.getSystemColor(SWT.COLOR_LIST_BACKGROUND),
						folder.getBackground() }, new int[] { 10, 90 }, true);
			} catch (NoSuchMethodError e) {
				folder.setSelectionBackground(
						new Color[] { display
								.getSystemColor(SWT.COLOR_LIST_BACKGROUND) },
						new int[0]);
			}
			folder.setSelectionForeground(display.getSystemColor(SWT.COLOR_LIST_FOREGROUND));
			
			SearchTab.setFolder(folder);
		} catch(Throwable e) {
			e.printStackTrace();
		}
	}

	public void refresh() {
	}
	
	public SearchCriteria getSearchCriteria(String search) {
		long type = 0;
		type |= (keywords.getSelection()?SearchCriteria.KEYWORDS:0);
		type |= (filename.getSelection()?SearchCriteria.FILENAME:0);
		type |= (description.getSelection()?SearchCriteria.DESCRIPTION:0);
		SearchCriteria result = new SearchCriteria(search, new Long(type));
		return result;
	}
}
