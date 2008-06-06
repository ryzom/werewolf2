/*
 * Created on May 11, 2005
 *
 */
package client.gui;

/**
 * @author Henri Kuuste
 *
 */
public interface IEventLogListener {
	public void doLog(String message, int level);
}
