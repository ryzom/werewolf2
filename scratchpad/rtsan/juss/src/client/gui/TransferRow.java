/*
 * Created on May 9, 2005
 *
 */
package client.gui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.TableEditor;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;

import shared.CConfig;

import client.CDownload;
import client.CTransfer;
import client.CUpload;
import client.ITransferListener;
import client.JussClient;

/**
 * @author Henri Kuuste
 *
 */
public class TransferRow implements ITransferListener {

	private Display display;
	private CTransfer t;
	private TableItem ti;
	private TableEditor te;
	private ProgressBar pb;
	private Table table;
	private boolean updating = false;
	private long lastUpdate = System.currentTimeMillis();
	private static long PROGRESS_UPDATE_INTERVAL;
	
	public TransferRow(CTransfer transfer, Table tab, Display d) {
		t = transfer;
		table = tab;
		display = d;

		t.addListener(this);
		ti = new TableItem (table, SWT.NONE);
		ti.setData("transfer", t.getName());
		pb = new ProgressBar (table, SWT.NONE);
		te = new TableEditor (table);
		te.grabHorizontal = te.grabVertical = true;
		te.setEditor (pb, ti, 2);
		updateStatus();
		updateProgress();
	}
	
	public void done() {
		display.syncExec(new Runnable() {
			public void run() {
				MainWindow.getWindow().doLocalRefresh();
			}
		});
	}
	
	public void updateProgress() {
		if(updating)
			return;
		if((System.currentTimeMillis() - lastUpdate) < PROGRESS_UPDATE_INTERVAL)
			return;
		lastUpdate = System.currentTimeMillis();
		updating = true;
		display.asyncExec(new Runnable() {
			public void run() {
				ti.setText(1, t.getDone() + " / " + t.getSize());
				ti.setText(3, (int)(t.getProgress()*100) + " %");
				pb.setSelection((int)(t.getProgress()*100));
				updating = false;
			}
		});
	}
	
	public void updateStatus() {
		display.syncExec(new Runnable() {
			public void run() {
				ti.setText (new String[] { t.getFileName(),
						t.getDone() + " / " + t.getSize(),
						"Progress",
						(int)(t.getProgress()*100) + " %",
						t.getNiceStatus(),
						t.getConnection().getName()});
				pb.setSelection((int)(t.getProgress()*100));
				setImage();
			}
		});
	}

	public void destory() {
		ti.dispose();
		pb.dispose();
		t.removeListener(this);
	}
	
	public TableItem getTableItem() {
		return ti;
	}
	
	private void setImage() {
		String image;
		if(t instanceof CDownload) {
			image = "download";
		} else if(t instanceof CUpload) {
			image = "upload";
		} else {
			return;
		}
		image += "." + t.getNiceStatus();
		ti.setImage(0, ImageRepository.getImage(image));
	}
	
	public static void loadConf() {
		CConfig conf = JussClient.getConf();
		Long temp = conf.getLongValue("TransferManager/TransferUpdateInterval");
		if(temp == null) {
			conf.setValue("TransferManager/TransferUpdateInterval", (long)300);
			conf.save();
		}
		PROGRESS_UPDATE_INTERVAL = conf.getLongValue("TransferManager/TransferUpdateInterval").longValue();
		
	}
}
