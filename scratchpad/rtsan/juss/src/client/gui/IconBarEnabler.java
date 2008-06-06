/*
 * Created on May 3, 2005
 *
 */
package client.gui;

/**
 * @author Henri Kuuste
 *
 */
public interface IconBarEnabler {
	  public boolean isEnabled(String itemKey);
	  public boolean isSelected(String itemKey);
	  public void itemActivated(String itemKey);
}
