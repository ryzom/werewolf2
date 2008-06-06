/*
 * Created on Apr 19, 2005
 *
 *
 * Java Unified Sharing System (JUSS)
 * Copyright (c)2005 Henri Kuuste
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
package client;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.httpclient.methods.*;
import org.apache.log4j.Logger;

import shared.IProgress;

/**
 * Klass, mis on nii uploadimise kui ka dowloadimise aluseks.
 * @author Henri Kuuste
 */
public class CTransfer extends Thread implements IProgress {
	/**
	 * Faili täisnimi kohalikus failisüsteemis.
	 */
	protected Long id;

	/**
	 * HTTP ühendus mida antud ülekandeks kasutatakse.
	 * 
	 * @uml.property name="p"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	protected PostMethod p;

	/**
	 * Faili suurus baitides.
	 * 
	 * @uml.property name="size" multiplicity="(0 1)"
	 */
	private long size = 0;

	/**
	 * Baitides palju ära tehtud on.
	 * 
	 * @uml.property name="done" multiplicity="(0 1)"
	 */
	private long done = 0;

	/**
	 * Ühendus millega antud transfer seotud on.
	 * 
	 * @uml.property name="con"
	 * @uml.associationEnd multiplicity="(1 1)"
	 */
	protected CConnection con;

	
	/**
	 * Staatus - ootab järjekorras.
	 */
	public static final int QUEUED = 1;
	/**
	 * Staatus - transport käib.
	 */
	public static final int TRANSFERING = 2;
	/**
	 * Staatus - valmis, kuid ikka võtab veel TransferManager'is ühe koha ära.
	 */
	public static final int DONE_PENDING = 3;
	/**
	 * Staatus - valmis.
	 */
	public static final int DONE = 4;
	/**
	 * Staatus - läbi kukkunud, kuid ikka võtab veel TransferManager'is ühe koha ära.
	 */
	public static final int FAILED_PENDING = 5;
	/**
	 * Staatus - läbi kukkunud.
	 */
	public static final int FAILED = 6;
	/**
	 * Staatus - märgitud kustutamisele.
	 */
	public static final int TO_BE_REMOVED = 7;
	
	/**
	 * Antud transfer'i staatus - algselt ootab järjekorras.
	 */
	protected int state = QUEUED;
	
	/**
	 * Viimane veateade.
	 */
	private String errorMsg = null;
	
	/**
	 * Faili nimi.
	 */
	private String fName = "";
	
	private List listeners = new ArrayList();
	
	/**
	 * Log4j logger
	 */
	protected static final Logger log = Logger.getLogger("juss.client.CTransfer");
	
	/**
	 * Konstruktor
	 * @param id Transporditava faili andmebaasi ID. Omab alamklassides erinevat tähendust. 
	 * @param connection Ühendus mille läbi antud ülekanne toimub.
	 * @see client.CDownload#CDownload(Long, CConnection)
	 * @see client.CUpload#CUpload(String, Long, CMetadata, String, CConnection)
	 */
	public CTransfer(Long id, CConnection connection) {
		super("Transfer");
		this.id = id;
		con = connection;
	}
	
	/**
	 * Sätib faili nime.
	 * @param name Faili nimi.
	 */
	protected void setFileName(String name) {
		fName = name;
		setName(name);
		for(int i = 0; i < listeners.size(); i++) {
			((ITransferListener)listeners.get(i)).updateStatus();
		}		
	}
	
	/**
	 * @return Faili nimi.
	 */
	public String getFileName() {
		return fName;
	}
	
	/**
	 * Käivitab transfer'i.
	 */
	public void run() {
		state = TRANSFERING;
		for(int i = 0; i < listeners.size(); i++) {
			((ITransferListener)listeners.get(i)).updateStatus();
		}		
	}

	/**
	 * 
	 * @uml.property name="size"
	 */
	public long getSize() {
		return size;
	}

	
	public double getProgress() {
		return (double)done/(double)size;
	}

	/**
	 * 
	 * @uml.property name="done"
	 */
	public void setDone(long amount) {
		done = amount;
		for(int i = 0; i < listeners.size(); i++) {
			((ITransferListener)listeners.get(i)).updateProgress();
		}		
	}

	/**
	 * 
	 * @uml.property name="done"
	 */
	public long getDone() {
		return done;
	}


	public void addDone(long amount) {
		done += amount;
		for(int i = 0; i < listeners.size(); i++) {
			((ITransferListener)listeners.get(i)).updateProgress();
		}		
	}

	/**
	 * 
	 * @uml.property name="size"
	 */
	public void setSize(long end) {
		size = end;
		for(int i = 0; i < listeners.size(); i++) {
			((ITransferListener)listeners.get(i)).updateStatus();
		}		
	}

	
	/**
	 * Sulgeb transfer'i.
	 */
	public void close() {
		if(p != null) {
			p.releaseConnection();
		}
	}
	
	/**
	 * Seab viimati esinenud veateate.
	 * @param str Veateade.
	 */
	protected void setError(String str) {
		errorMsg = "Transfer Error: " + str;
		log.warn(errorMsg);
	}
	
	/**
	 * @return Viimati esinenud veateade.
	 */
	public String getErrorMessage() {
		return errorMsg;
	}
	
	/**
	 * Meetod seab staatuse läbi kukkunuks ja sulgeb andmebaasi sessiooni.
	 */
	public void fail() {
		con.getDBManager().closeSession();
		state = FAILED_PENDING;
		for(int i = 0; i < listeners.size(); i++) {
			((ITransferListener)listeners.get(i)).updateStatus();
		}		
	}
	
	/**
	 * Meetod seab staatuse õnnestunuks ja sulgeb andmebaasi sessiooni.
	 */
	public void succeed() {
		con.getDBManager().closeSession();
		state = DONE_PENDING;
		for(int i = 0; i < listeners.size(); i++) {
			((ITransferListener)listeners.get(i)).updateStatus();
			((ITransferListener)listeners.get(i)).done();
		}
	}
	
	/**
	 * @return <B>true</B> kui transfer on läbi (läbi kukkunud, õnnestunud või kustutamisele määratud) <B>false</B> vastasel juhul.
	 * Transfer Queue koht võib sellisel juhul ikka veel hõivatud olla. 
	 */
	public boolean isDone() {
		if(state > TRANSFERING) {
			return true;
		}
		return false;
	}
	
	/**
	 * @return <B>true</B> kui transfer on läbi (läbi kukkunud või õnnestunud) ja Transfer Queue koht vabastatud.
	 */
	public boolean isFinished() {
		if(state == DONE)
			return true;
		if(state == FAILED)
			return true;
		return false;
	}
	
	/**
	 * @return <B>true</B> kui transfer ootab järjekorras <B>false</B> vastasel juhul.
	 */
	public boolean isQueued() {
		return (state == QUEUED);
	}
	
	/**
	 * @return <B>true</B> kui transfer käib <B>false</B> vastasel juhul.
	 */
	public boolean isTransfering() {
		return (state == TRANSFERING);
	}

	/**
	 * Märgib transfer'i eemaldamisele.
	 */
	public void remove() {
		state = TO_BE_REMOVED;
		for(int i = 0; i < listeners.size(); i++) {
			((ITransferListener)listeners.get(i)).updateStatus();
		}		
	}
	
	/**
	 * @return <B>true</B> kui transfer on märgitud eemaldamisele <B>false</B> vastasel juhul.
	 */
	public boolean isToBeRemoved() {
		return (state == TO_BE_REMOVED);
	}
	
	/**
	 * Lõpetab transfer'i. Selleks peab staatus olema kas DONE_PENDING või FAILED_PENDING. Vastasel juhul on tulemus teadmata.
	 */
	public void finish() {
		state++;
		for(int i = 0; i < listeners.size(); i++) {
			((ITransferListener)listeners.get(i)).updateStatus();
		}		
	}
	
	public void addListener(ITransferListener l) {
		listeners.add(l);
	}
	
	public void removeListener(ITransferListener l) {
		listeners.remove(l);
	}
	
	public String getNiceStatus() {
		String status;
		switch(state) {
			case QUEUED:
				status = "Queued";
				break;
			case TRANSFERING:
				status = "Transfering";
				break;
			case DONE:
				status = "Completed";
				break;
			case FAILED:
				status = "Failed";
				break;
			case DONE_PENDING:
				status = "Completing";
				break;
			case FAILED_PENDING:
				status = "Failing";
				break;
			default:
				status = "Unknown";
		}
		return status;
	}
	
	public int getStatus() {
		return state;
	}
	
	public CConnection getConnection() {
		return con;
	}
}
