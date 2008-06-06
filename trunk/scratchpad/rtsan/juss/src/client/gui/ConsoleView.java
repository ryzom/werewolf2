package client.gui;

import java.util.Calendar;
import java.util.GregorianCalendar;

import org.apache.commons.logging.Log;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CLabel;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.ScrollBar;


/**
 * @author Henri Kuuste
 * 
 */
public class ConsoleView implements IView, IEventLogListener {

  Display display;
  Composite main;
  Combo logLevel;
  StyledText consoleText;
  
  private static Color debug_color;
  private static Color info_color;
  private static Color warn_color;
  private static Color error_color;
  private static Color fatal_color;

  public void initialize(Composite composite) {
    display = composite.getDisplay();
    main = new Composite(composite, SWT.NONE);
    main.setLayout(new FormLayout());
    Composite logLevelComp = new Composite(main, SWT.NONE);
    logLevelComp.setLayout(new FormLayout());
    FormData data = new FormData();
    data.top = new FormAttachment(0,0);
    data.left = new FormAttachment(0,0);
    data.right = new FormAttachment(100, 0);
    logLevelComp.setLayoutData(data);
	logLevel = new Combo(logLevelComp, SWT.READ_ONLY | SWT.SINGLE);
    data = new FormData();
    data.right = new FormAttachment(100, -5);
    logLevel.setLayoutData(data);
    logLevel.add("Debug", 0);
    logLevel.setData("0", new Integer(Level.DEBUG_INT));
    logLevel.add("Info", 1);
    logLevel.setData("1", new Integer(Level.INFO_INT));
    logLevel.add("Warning", 2);
    logLevel.setData("2", new Integer(Level.WARN_INT));
    logLevel.add("Error", 3);
    logLevel.setData("3", new Integer(Level.ERROR_INT));
    logLevel.add("Fatal", 4);
    logLevel.setData("4", new Integer(Level.FATAL_INT));
    logLevel.select(0);
    CLabel label = new CLabel(logLevelComp, SWT.NONE);
    label.setText("Log level: ");
    data = new FormData();
    data.right = new FormAttachment(logLevel, 5);
    label.setLayoutData(data);
    logLevelComp.setSize(logLevelComp.computeSize(SWT.NONE, SWT.NONE));
    logLevelComp.pack();
    consoleText = new StyledText(main, SWT.READ_ONLY | SWT.V_SCROLL | SWT.H_SCROLL);
    data = new FormData();
    data.top = new FormAttachment(logLevelComp, 0);
    data.left = new FormAttachment(0,0);
    data.right = new FormAttachment(100, 0);
    data.bottom = new FormAttachment(100, 0);
    consoleText.setLayoutData(data);
    
    debug_color = new Color(display, 168, 206, 143);
    info_color = new Color(display, 255, 255, 255);
    warn_color = new Color(display, 206, 187, 143);
    error_color = new Color(display, 206, 143, 143);
    fatal_color = new Color(display, 216, 90, 90);
    
    EventLogAppender.addListener(this);
  }

  public Composite getComposite() {
    return main;
  }

  public void refresh() {}

  public void delete() {
    EventLogAppender.removeListener(this);
  	MainWindow.getWindow().getShell().setFocus();
  }

  public void doLog(final String _text, final int level) {
    if(display == null || display.isDisposed())
      return;
    display.asyncExec(new JUSSRunnable() {
      public void runSupport() {
      	if(!checkLevel(level))
      		return;
        if (consoleText == null || consoleText.isDisposed())
          return;
        ScrollBar sb = consoleText.getVerticalBar();

        boolean autoScroll = sb.getSelection() == (sb.getMaximum() - sb.getThumb());
        int nbLines = consoleText.getLineCount();
        if (nbLines > 4096 + 256)
          consoleText.replaceTextRange(0, consoleText.getOffsetAtLine(256), "");
        Calendar now = GregorianCalendar.getInstance();        
        consoleText.append(_text + "\n");
        nbLines = consoleText.getLineCount();
         consoleText.setLineBackground(nbLines - 2, 1, getColor(level));
        if (autoScroll)
          consoleText.setTopIndex(nbLines-1);
      }
    });
  }
  
  private Color getColor(int level) {
  	switch(level) {
		case Level.DEBUG_INT:
  			return debug_color;
  		case Level.INFO_INT:
  			return info_color;
  		case Level.ERROR_INT:
  			return error_color;
  		case Level.FATAL_INT:
  			return fatal_color;
  		case Level.WARN_INT:
  			return warn_color;
  		default:
  		  	return info_color;
  	}
  }
  
  
  private String format(int n) {
    if(n < 10) return "0".concat(String.valueOf(n));
    return String.valueOf(n);
  }  

  private boolean checkLevel(int level) {
  	int lLevelIndex = logLevel.getSelectionIndex();
  	int lLevel = ((Integer)logLevel.getData(lLevelIndex+"")).intValue();
  	if(level >= lLevel)
  		return true;
  	return false;
  }
}
