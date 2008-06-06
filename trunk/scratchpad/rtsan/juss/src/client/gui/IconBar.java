/*
 * Created on May 3, 2005
 *
 */
package client.gui;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CBanner;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.CoolBar;
import org.eclipse.swt.widgets.CoolItem;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;


/**
 * @author Henri Kuuste
 *
 */
public class IconBar {
	  
	  CBanner main;
	  CoolBar coolBar;
	  Composite parent;
	  Composite searchArea;
	  Map itemKeyToControl;
	  int numSearches = 20;
	  
	  IconBarEnabler currentEnabler;
	  
	  public IconBar(Composite parent) {
	    this.parent = parent;
	    itemKeyToControl = new HashMap();
	    main = new CBanner(parent, SWT.NONE);
	    coolBar = new CoolBar(main,SWT.NONE);
	    // Should use CacheWrapper here somehow to avoid the resize flicker I guess
	    searchArea = new Composite(main, SWT.NONE);
	    initBar();       
	    coolBar.setLocked(true);
	  }
	  
	  public void setEnabled(String itemKey,boolean enabled) {
	    ToolItem toolItem = (ToolItem) itemKeyToControl.get(itemKey);
	    if(toolItem != null)
	      toolItem.setEnabled(enabled);
	  }
	  
	  public void setSelection(String itemKey,boolean selection) {
	  	ToolItem toolItem = (ToolItem) itemKeyToControl.get(itemKey);
	    if(toolItem != null)
	      toolItem.setSelection(selection);
	  }
	  
	  public void setCurrentEnabler(IconBarEnabler enabler) {
	    this.currentEnabler = enabler;
	    refreshEnableItems();
	  }
	  
	  private void refreshEnableItems() {
	    Iterator iter = itemKeyToControl.keySet().iterator();
	    while(iter.hasNext()) {
	      String key = (String) iter.next();
	      ToolItem toolItem = (ToolItem) itemKeyToControl.get(key);
	      if(toolItem == null )
	        continue;
	      if(currentEnabler != null) {
	        toolItem.setEnabled(currentEnabler.isEnabled(key));
	        toolItem.setSelection(currentEnabler.isSelected(key));
	      }
	      else {        
	        toolItem.setEnabled(false);
	        toolItem.setSelection(false);
	      }
	    }
	  }
	  
	  private ToolItem createToolItem(ToolBar toolBar,int style,String key,String imageName,String toolTip) {    
	    final ToolItem toolItem = new ToolItem(toolBar,style);
	    toolItem.setData("key",key);
	    toolItem.setToolTipText(toolTip);
	    toolItem.setImage(ImageRepository.getImage(imageName));
	    
	    toolItem.addListener(SWT.Selection,new Listener() {
	      public void handleEvent(Event e) {
	        if(currentEnabler != null)
	          currentEnabler.itemActivated((String)toolItem.getData("key"));        	
	      }
	    });
	    itemKeyToControl.put(key,toolItem);
	    return toolItem;
	  }  
	  
	  private void initBar() {
	    main.setSimple(false);
	    main.setLeft(coolBar);
	    main.setRight(searchArea);
	    main.setRightWidth(200);

	    //The File Menu
	    CoolItem coolItem = new CoolItem(coolBar,SWT.NONE); 
	    ToolBar toolBar = new ToolBar(coolBar,SWT.FLAT);
	    createToolItem(toolBar,SWT.PUSH,"new","new","Create a new project");    
	    createToolItem(toolBar,SWT.PUSH,"open","open","Open an existing project");    
	    toolBar.pack(); 
	    Point p = toolBar.getSize();
	    coolItem.setControl(toolBar);
	    coolItem.setSize(coolItem.computeSize (p.x,p.y));
	    coolItem.setMinimumSize(p.x,p.y);
	    
	    
	    coolItem = new CoolItem(coolBar,SWT.NONE); 
	    toolBar = new ToolBar(coolBar,SWT.FLAT);    
	    createToolItem(toolBar,SWT.PUSH,"refresh","refresh","Refresh");    
	    toolBar.pack();
	    p = toolBar.getSize();
	    coolItem.setControl(toolBar);
	    coolItem.setSize(p.x,p.y);
	    coolItem.setMinimumSize(p.x,p.y);    
	    
	    createSearchArea();
	  }
	  
	  public void createSearchArea() {
//	    coolItem = new CoolItem(coolBar, SWT.NONE);
	    FormLayout layout = new FormLayout();
	    searchArea.setLayout(layout);
	    final Button searchButton = new Button(searchArea, SWT.NONE);
	    searchButton.setText("Search");
	    FormData data = new FormData();
	    data.right = new FormAttachment(100, 0);
	    searchButton.setLayoutData(data);
	    final Combo searchBox = new Combo(searchArea, SWT.DROP_DOWN);
	    data = new FormData();
	    data.right = new FormAttachment(searchButton, -5);
	    data.left = new FormAttachment(0, 0);
	    searchBox.setLayoutData(data);
//	    coolItem.setControl(searchArea);
//	    coolItem.setSize(coolItem.computeSize (p.x,p.y));
//	    coolItem.setMinimumSize(p.x,p.y);
	    
	    searchButton.addMouseListener(new MouseAdapter() {
	    	public void mouseUp(MouseEvent e) {
	    		doSearch(searchBox);
			}
	    });
	    
	    searchBox.addKeyListener(new KeyAdapter() {
	    	public void keyPressed(KeyEvent e) {
	    		if(e.keyCode == SWT.CR) {
		    		doSearch(searchBox);
	    		}
	    	}
	    });
	  }
	  
	  public void setLayoutData(Object layoutData) {
	  	main.setLayoutData(layoutData);
//	    coolBar.setLayoutData(layoutData);
	  }
	  
	  public CBanner getBanner() {
	    return main;
	  }
	  
	  private void doSearch(Combo search) {
		search.add(search.getText(), 0);
	  	if(search.getItemCount() > numSearches) {
	  		search.remove(search.getItemCount() - 1);
	  	}
		MainWindow.getWindow().doSearch(search.getText());	    				  	
	  }

	}