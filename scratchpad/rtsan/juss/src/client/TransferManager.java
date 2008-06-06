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

import java.util.*;

import org.apache.log4j.Logger;

/**
 * JUSS'i transpordi (upload/download) haldur. See on l�im mis jookseb tagataustal
 * ja m��rab �ra kuna mingi upload v�i download toimuma peaks ja kuna neid eemaldada.
 * �hes�naga haldab sellist asja nagu Transfer Queue.
 * @author Henri Kuuste
 */
public class TransferManager {

	/**
	 * Nimekiri transfer'itest.
	 * 
	 * @uml.property name="transfers"
	 * @uml.associationEnd elementType="client.CTransfer" multiplicity="(0 -1)"
	 */
	private ArrayList transfers = new ArrayList();

	/**
	 * Timer mida kasutatakse selle klassi l�ime t�� tegemiseks.
	 */
	Timer timer = null;
	/**
	 * M��rab �ra mitu transfer'it korraga v�ib k�ia.
	 */
	private int maxSimultaneous = 2;
	/**
	 * Mitu transfer'it momendil k�ib.
	 */
	private int currentTransfers = 0;

	/**
	 * Kas eemaldada valmis transferid automaatselt?
	 * 
	 * @uml.property name="removeFinishedTransfers" multiplicity="(0 1)"
	 */
	private boolean removeFinishedTransfers = false;

	List listeners = new ArrayList();
	
	private static final Logger log = Logger.getLogger("juss.client.TransferManager");
	
	/**
	 * Loob uue instantsi sellest klassist.
	 * @param max Mitu transfer'it maksimaalselt korraga k�ia v�ib.
	 * @param removeFinished Kas eemaldada automaatselt valmis transferid?
	 */
	public TransferManager(int max, boolean removeFinished) {
		maxSimultaneous = max;
		removeFinishedTransfers = removeFinished;
	}
	
	/**
	 * K�ivitab halduri t�� (l�ime).
	 */
	public void start() {
		log.debug("Transfer manager started");
		timer = new Timer();
		timer.schedule(new TransferTask(), 0, 1000);
	}
	
	/**
	 * L�petab halduri t�� (peatab l�ime).
	 */
	public void stop() {
		log.debug("TransferManager stopped");
		timer.cancel();
		timer = null;
	}

	/**
	 * Loob vaikimisi halduri kus on lubatud kaks transfer'it korraga ja automaatselt transfer'eid ei eemaldata.
	 */
	public TransferManager() {
		this(2, false);
	}
	
	/**
	 * Lisab uue transfer'i j�rjekorda.
	 * @param t Transfer mida lisada.
	 * @return Uue transfer'i j�rjekorranumber.
	 */
	public int add(CTransfer t) {
		transfers.add(t);
		if(timer == null)
			start();
		for(int i = 0; i < listeners.size(); i++)
			((ITransferManagerListener)listeners.get(i)).addTransfer(t);
		return (transfers.size()-1);
	}
	
	/**
	 * Tagastab j�rjekorranumbrile vastava transfer'i.
	 * @param index J�rjekorranumber.
	 * @return J�rjekorranumbrile vastav transfer.
	 */
	public CTransfer get(int index) {
		return (CTransfer)transfers.get(index);
	}
	
	/**
	 * Eemaldab j�rjekorranumbrile vastava transfer'i.
	 * @param index J�rjekorranumber.
	 */
	public void remove(int index) {
		get(index).remove();
	}
	
	/**
	 * @return Transfer'ite arv j�rjekorras. 
	 */
	public int size() {
		return transfers.size();
	}
	
	/**
	 * @return <B>true</B> kui haldur eemaldab valmis transfer'id automaatselt <B>false</B> vastasel juhul.
	 */
	public boolean getRemoveFinishedTransfer() {
		return removeFinishedTransfers;
	}

	/**
	 * Kas haldur peaks valmis transfer'id automaatselt eemaldama v�i mitte?
	 * @param b <B>true</B> kui haldur peaks eemaldama valmis transfer'id automaatselt <B>false</B> vastasel juhul.
	 * 
	 * @uml.property name="removeFinishedTransfers"
	 */
	public void setRemoveFinishedTransfers(boolean b) {
		removeFinishedTransfers = b;
	}

	
	/**
	 * Tagastab transfer'ite nimekirja.
	 * @return java.util.List mille elementideks on k�ik j�rjekorras olevad transfer'id (CTransfer). Ka need mis on juba l�petanud kuid pole eemaldatud veel.
	 */
	public List getTransfers() {
		return transfers;
	}

	/**
	 * Eemaldab valmis transfer'id.
	 */
	public void removeFinishedTransfers() {
		for(int i = 0; i < transfers.size(); i++) {
			CTransfer t = (CTransfer)transfers.get(i);
			if(t.isFinished()) {
				t.remove();
			}
		}			
	}

	/**
	 * L�petab �ra transfer'id mis ise v�idavad, et nad on l�ppenud.
	 * Kui see tehtud saavad j�rjekorras olevad transferid asuda nende kohale mis �ra l�petati.
	 */
	private void finalizeTransfers() {
		for(int i = 0; i < transfers.size(); i++) {
			CTransfer t = (CTransfer)transfers.get(i);
			if(t.isDone() && !t.isFinished()) {
				t.finish();
				currentTransfers--;
			}
		}
	}
	
	/**
	 * Paneb k�ima transfer'id mis on j�rgmisene nimekirjas, kui samaaegsete transfer'ite limiidini pole veel j�utud.
	 */
	private void startTransfers() {
		if(currentTransfers >= maxSimultaneous)
			return;
		for(int i = 0; i < transfers.size(); i++) {
			if(currentTransfers >= maxSimultaneous)
				return;
			CTransfer t = (CTransfer)transfers.get(i);
			if(t.isQueued() && !t.isAlive()) {
				t.start();
				currentTransfers++;
			}
		}
	}
	
	/**
	 * Eemaldab transfer'id mis on m�rgitud maha kustutamisele.
	 */
	private void removeTransfers() {
		for(int i = 0; i < transfers.size();) {
			CTransfer t = (CTransfer)transfers.get(i);
			if(t.isToBeRemoved()) {
				t.close();
				for(int j = 0; j < listeners.size(); j++)
					((ITransferManagerListener)listeners.get(j)).removeTransfer(t);
				transfers.remove(i);
			} else {
				i++;
			}
		}	
	}
	
	/**
	 * TransferManager'i l�ime t��d tegev klass.
	 * @author Henri Kuuste
	 */
	class TransferTask extends TimerTask {
		/**
		 * Teeb halduri p�hit��d iga kindla ajavahemiku j�rel.
		 * <BR/>L�petab transferid, siis paneb k�ima j�rjekorras j�rgmised, eemaldab need mis on valmis kui vastav option on valitud.
		 */
		public void run() {
			finalizeTransfers();
			startTransfers();
			if(removeFinishedTransfers)
				removeFinishedTransfers();
			removeTransfers();
			if(currentTransfers == 0) {
				log.debug("Finished all transfers.");
				stop();
			}
		}
	}

	/**
	 * Kuvab transferite kohta info System.out stream'i. 
	 */
	public void showTransfers() {
		for(int i = 0; i < transfers.size(); i++) {
			CTransfer t = (CTransfer)transfers.get(i);
			log.info("Transfer: #" + i + " \"" + t.id + "\"");
		}
	}
	
	public void addListener(ITransferManagerListener l) {
		listeners.add(l);
	}
	
	public void removeListener(ITransferManagerListener l) {
		listeners.remove(l);
	}
}
