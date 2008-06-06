/*
 * Created on May 9, 2005
 *
 */
package client.gui;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;

import client.CTransfer;
import client.ITransferManagerListener;
import client.JussClient;

/**
 * @author Henri Kuuste
 *
 */
public class TransferManagerView implements ITransferManagerListener, IView {

	private Display display;
	private Table main;
	
	private HashMap transfers = new HashMap();
	
	/**
	 *
	 */

	public void addTransfer(final CTransfer transfer) {
		display.syncExec(new Runnable() {
			public void run() {
				transfers.put(transfer.getName(), new TransferRow(transfer, main, display));
			}
		});
	}

	/**
	 *
	 */

	public void removeTransfer(final CTransfer transfer) {
		display.syncExec(new Runnable() {
			public void run() {
				TransferRow tr = (TransferRow)(transfers.get(transfer.getName()));
				tr.destory();
				transfers.remove(transfer.getName());
			}
		});
	}

	/**
	 *
	 */

	public void initialize(Composite composite) {
		JussClient.getTransferManager().addListener(this);
		TransferRow.loadConf();

		display = composite.getDisplay();
		main = new Table(composite, SWT.SINGLE);
		main.setHeaderVisible(true);
		TableColumn col = new TableColumn(main, SWT.LEFT);
		col.setText("Name");
		col.setWidth(300);
		col = new TableColumn(main, SWT.RIGHT);
		col.setText("Transfered");
		col.setWidth(120);
		col = new TableColumn(main, SWT.RIGHT);
		col.setText("Progress");
		col.setWidth(147);
		col = new TableColumn(main, SWT.RIGHT);
		col.setText("%");
		col.setWidth(50);
		col = new TableColumn(main, SWT.RIGHT);
		col.setText("Status");
		col.setWidth(80);
		col = new TableColumn(main, SWT.RIGHT);
		col.setText("Connection");
		col.setWidth(100);
	}

	/**
	 *
	 */

	public Composite getComposite() {
		return main;
	}

	/**
	 *
	 */

	public void refresh() {
	}

	/**
	 *
	 */

	public void delete() {
		Set keys = transfers.keySet();
		Iterator it = keys.iterator();
		while(it.hasNext()) {
			TransferRow tr = (TransferRow)transfers.get(it.next());
			tr.destory();
		}
		JussClient.getTransferManager().removeListener(this);
	}

}
