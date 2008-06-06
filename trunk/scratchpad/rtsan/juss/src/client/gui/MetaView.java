/*
 * Created on May 4, 2005
 *
 */
package client.gui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;

import shared.CConfig;
import shared.CMetadata;
import shared.JussFile;
import client.CConnection;
import client.JussClient;

/**
 * @author Henri Kuuste
 *
 */
public class MetaView implements IView {
	private Display display;
	
	private ScrolledComposite scroll;

	private LiveSashForm main;

	private Table t_meta;
	private TableColumn col1;
	private TableColumn col2;
	private Group l_description;
	
	private StyledText t_description;
	
	
	public void initialize(Composite composite) {
		try {
			display = composite.getDisplay();
			
			scroll = new ScrolledComposite(composite, SWT.V_SCROLL | SWT.H_SCROLL);
			scroll.setExpandHorizontal(true);
			scroll.setExpandVertical(true);
			main = new LiveSashForm(scroll, SWT.HORIZONTAL);
			scroll.setContent(main);

			t_meta = new Table(main, SWT.SINGLE | SWT.HIDE_SELECTION);
			t_meta.setBackground(display.getSystemColor(SWT.COLOR_INFO_BACKGROUND));
			
			col1 = new TableColumn(t_meta, SWT.LEFT);
			
			col2 = new TableColumn(t_meta, SWT.LEFT);
			
			l_description = new Group(main, SWT.NONE);
			l_description.setText("Description");
			l_description.setLayout(new FillLayout());
			l_description.setBackground(display.getSystemColor(SWT.COLOR_INFO_BACKGROUND));
			t_description = new StyledText(l_description, SWT.MULTI | SWT.WRAP | SWT.READ_ONLY);
			t_description.setBackground(display.getSystemColor(SWT.COLOR_INFO_BACKGROUND));
			
			loadConf();
			
			scroll.setMinSize(main.computeSize(SWT.DEFAULT, SWT.DEFAULT));
			clearMeta();			
			
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	public Composite getComposite() {
		return scroll;
	}

	public void refresh() {
	}

	public void delete() {
	  	MainWindow.getWindow().getShell().setFocus();
	}
	
	public void clearMeta() {
		t_meta.removeAll();
		
		TableItem item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Name:", ""});
		item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Version:", ""});
		item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Creator:", ""});
		item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Status:", ""});
		item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Modified:", ""});		
		col2.pack();
		col1.pack();

	}
	
	public void setMeta(CConnection con, Long id) {
		JussFile jf = con.getFile(id);
		CMetadata meta = con.getMetadata(id);
		
		t_meta.removeAll();
		
		TableItem item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Name:", jf.getName()});
		item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Version:", ""+jf.getVersion()});
		item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Creator:", meta.getCreator()});
		item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Status:", (jf.getExists() ? "Local" : "Remote")});
		item = new TableItem(t_meta, SWT.NONE);
		item.setText(new String[] {"Modified:", jf.getModtime().toString()});		
		t_description.setText(meta.getDescription());
		col2.pack();
		col1.pack();
	}

	public void saveConf() {
		CConfig conf = JussClient.getConf();
		conf.setValue("GUI/MetaView/main.weight1", main.getWeights()[0]);
		conf.setValue("GUI/MetaView/main.weight2", main.getWeights()[1]);
		conf.save();
	}
	
	public void loadConf() {
		CConfig conf = JussClient.getConf();
		Integer w1, w2;
		w1 = conf.getIntValue("GUI/MetaView/main.weight1");
		w2 = conf.getIntValue("GUI/MetaView/main.weight2");
		if(w1 != null && w2 != null)
			main.setWeights(new int[] {w1.intValue(), w2.intValue()});
		else
			main.setWeights(new int[] { 6, 4 });
	}
}
