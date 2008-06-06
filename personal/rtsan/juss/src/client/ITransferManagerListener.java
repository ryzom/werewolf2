/*
 * Created on May 9, 2005
 *
 */
package client;

/**
 * @author Henri Kuuste
 *
 */
public interface ITransferManagerListener {
	public void addTransfer(CTransfer transfer);
	public void removeTransfer(CTransfer transfer);
}
