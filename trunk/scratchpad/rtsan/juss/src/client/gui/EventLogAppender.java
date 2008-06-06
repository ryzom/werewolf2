/*
 * Created on May 11, 2005
 *
 */
package client.gui;

import java.util.ArrayList;
import java.util.Iterator;

import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.spi.LoggingEvent;

/**
 * @author Henri Kuuste
 *
 */
public class EventLogAppender extends AppenderSkeleton {
	
	public static ArrayList listeners = new ArrayList();
	
	public static void addListener(IEventLogListener l) {
		listeners.add(l);
	}
	
	public static void removeListener(IEventLogListener l) {
		listeners.remove(l);
	}

	/**
	 *
	 */

	protected void append(LoggingEvent event) {
		String message = layout.format(event);
		Iterator it = listeners.iterator();
		while(it.hasNext())
			((IEventLogListener)it.next()).doLog(message, event.getLevel().toInt());
	}

	/**
	 *
	 */

	public boolean requiresLayout() {
		return true;
	}

	/**
	 *
	 */

	public void close() {
		if(this.closed)
			return;
		this.closed = true;
	}

}
