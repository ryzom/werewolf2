/*
 * Created on May 9, 2005
 *
 */
package client;

/**
 * @author Henri Kuuste
 *
 */
public interface ITransferListener {
	public void done();
	public void updateProgress();
	public void updateStatus();
}
