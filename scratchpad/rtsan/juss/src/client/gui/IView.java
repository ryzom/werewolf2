/*
 * Created on May 2, 2005
 *
 */
package client.gui;

import org.eclipse.swt.widgets.Composite;


/**
 * @author Henri Kuuste
 * 
 */
public interface IView {
  public void initialize(Composite composite);
  public Composite getComposite();
  public void refresh();
  public void delete();
}
