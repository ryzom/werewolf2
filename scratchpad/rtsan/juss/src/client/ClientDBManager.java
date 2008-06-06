/*
 * Created on Apr 22, 2005
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
import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.apache.log4j.Logger;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.cfg.Configuration;

import shared.CMetadata;
import shared.DBManager;
import shared.FileManager;
import shared.JussFile;

/**
 * JUSSi kliendipoolse andmebaasiga suhtlemist korraldav klass
 * @author Henri Kuuste, Taimo Peelo 
 */

public class ClientDBManager extends DBManager {
	private static final String DB_FILE = "/database/database";
	
	private static final String HIBERNATE_CONFIG = "hibernate.client.cfg.xml";
	
	private static final String DB_URL = "jdbc:hsqldb:file:";
	
	private static final String DB_OPTIONS = ";hsqldb.default_table_type=cached";

	private static final Logger log = Logger.getLogger("juss.client.ClientDBManager");

	/**
	 * Konstruktor, loob uue kliendiandmebaasihalduri.
	 * @param root uue andmebaasihalduri töökataloog
	 */
	public ClientDBManager(String root) {
		Configuration cfg = null;
		try {
			cfg = new Configuration().configure(HIBERNATE_CONFIG);
		} catch (Throwable ex) {
			log.fatal("Initial Configuration() creation failed");
			throw new ExceptionInInitializerError(ex);
		}
		cfg.setProperty("hibernate.connection.url", DB_URL + root + DB_FILE
				+ DB_OPTIONS);
		try {
			sessionFactory = cfg.buildSessionFactory();
		} catch (Throwable ex) {
			log.fatal("Initial SessionFactory creation failed." + ex);
            throw new ExceptionInInitializerError(ex);
		}

		init();
	}

	public synchronized void init() {
		super.init();
	}

	public synchronized Long getModTime(Long id, String type) {
		return super.getModTime(id, type);
	}

	/**
	 * Kontrollib kas antud ID-ga fail on andmebaasis. 
	 * @param id faili andmebaasisisene ID.
	 * @return kas sellise ID-ga fail eksisteerib andmebaasis
	 */
	public synchronized boolean hasData(Long id) {
		if (id == null)
			return false;
		
		boolean hTx = haveTransaction();
		Session session = getSession();
        beginTransaction();
        
		if(session.get(JussFile.class, id) == null) {
			if (!hTx)
				commitTransaction();
			return false;
		}
		if (!hTx)
			commitTransaction();
		return true;
	}

	/**
	 * Kontrollib kas antud ID-ga faili kohta leidub andmebaasis metadatat. 
	 * @param id faili andmebaasisisene ID.
	 * @return kas sellise ID-ga faili jaoks eksisteerib metadata.
	 */
	
	public synchronized boolean hasMetadata(Long id) {
		boolean hTx = haveTransaction();
		Session session = getSession();
        beginTransaction();
        
		if(session.get(CMetadata.class, id) == null) {
			if (!hTx)
				commitTransaction();
			return false;
		}
		if (!hTx)
			commitTransaction();
		return true;
	}

	
	/**
	 * Tagastab kausta sisu JussFile'de Listina
	 * @param id kausta ID mille kohta listingut küsitakse 
	 * @return List JussFile'dega mis asusid nõutud kaustas
	 *         või null kui tegemist pole kaustaga
	 */
	public synchronized List getFolderContents(Long id) {
		boolean hTx = haveTransaction();
		Session session = getSession();
        beginTransaction();

		JussFile jf = (JussFile) session.get(JussFile.class, id);
		
		if (jf != null && jf.isFolder()) {
			List result = session.createQuery(
					"FROM JussFile WHERE parentID = ?")
					.setLong(0, jf.getId().longValue()).list();
			if (!hTx)
				commitTransaction();
			return result;
		}
		
		if (!hTx)
			commitTransaction();
		
		return null;
	}

	public synchronized CMetadata getMetadata(Long id) {
		return super.getMetadata(id);
	}


	/**
	 * Lisab faili kliendiandmebaasi.
	 * @param f Lisatav JussFile
	 * @return lisatud faili andmebaasisisene ID 
	 */
	public synchronized Long addFile(JussFile f) {
		boolean hTx = haveTransaction();
		Session session = getSession();
		beginTransaction();
		
		JussFile jf = (JussFile)session.get(JussFile.class, f.getId()); 
		if (jf != null) {
			f.setExists(jf.getExists());
			session.merge(f);
		} else
			session.saveOrUpdate(f);
		
		if (!hTx)
			commitTransaction();
		
		log.debug("Added to database \"" + f.getName() + " " + f.getId() + " " + f.getModtime() + "\"");
		return f.getId();	
	}
	
	public synchronized JussFile getFile(Long id) {
		return super.getFile(id);
	}

	/**
	 * Suleb kliendiandmebaasiga seotud sessioonid ja transaktsioonid,
	 * lõpetab ühenduse andmebaasiga.
	 */
	public void shutdown() {
		Session session = getSession();
		log.info("Database shutdown");
		try {
			beginTransaction();
			commitTransaction();
			session.connection().createStatement().execute("SHUTDOWN");
			closeSession();
			reportOpenedAndClosedSessions();
			reportOpenedAndClosedTransactions();
		} catch (Exception e) {
			log.error("Could not shut down HSQLDB: " + e.toString());
		}
	}
	
	public synchronized Long getId(Long parentId, String name) {
		return super.getId(parentId, name);
	}
	
	/**
	 * Lisab Listitäie JussFile'de kohta käiva info kliendiandmebaasi
	 * @param l List lisatavate JussFile'dega
	 */
	public void addFileList(List l) {
		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();
        
		Iterator i = l.iterator();
		while (i.hasNext()) {
			Object next = i.next();
			if (next instanceof JussFile) {
				JussFile jf = (JussFile) next;
				addFile(jf);
			}
		}
		
		if (!hTx)
			commitTransaction();
	}
	
	/**
	 * Eemaldab kohalikust andmebaasist failid mis asuvad teatud kaustas ja 
	 * mille ID-d ei sisaldu argumentlistis. Kasutatakse nt lokaalsest 
	 * andmebaasist nende failide kustutamiseks, mida serveris enam ei leidu. 
	 * @param l List Long'idest, sisaldab failide andmebaasi ID-sid  
	 * @param id kausta ID, milles Listis sisalduvaid faile otsida. 
	 * @param root kliendi failihoidlajuurika absoluutne rada
	 */
	public void removeDeadFiles(List l, Long id, String root) {
		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();
				 
		Collections.sort(l);
		List o = getFolderContents(id);
		if(o != null) {
			Iterator i = o.iterator();
			while(i.hasNext()) {
				JussFile jf = (JussFile)i.next();
				Long fid = jf.getId();
				if(Collections.binarySearch(l, fid) < 0) {
					removeFile(jf, root, (jf.isFolder()?FileManager.FOLDERS:FileManager.FILES));
				}
			}
		}
		
		if (!hTx)
			commitTransaction();
	}
	
	/**
	 * Eemaldab faili kõvakettalt ja kliendiandmebaasist
	 * @param jf JussFile mida soovitakse andmebaasist eemaldada
	 * @param root absoluutne rada kliendiandmebaasi juurikani
	 * @return kas eemaldamine õnnestus või mitte
	 */
	public synchronized boolean removeFile(JussFile jf, String root, long type) {
		return removeFile(jf.getId(), root, type);
	}
	
	/**
	 * Eemaldab faili kõvakettalt ja kliendiandmebaasist
	 * @param id Kustutatava faili andmebaasisisene ID
	 * @param root absoluutne rada kliendiandmebaasi juurikani
	 * @return kas eemaldamine õnnestus või mitte
	 */
	public synchronized boolean removeFile(Long id, String root, long type) {
		FileManager.deleteFile(root + getStringPath(id));
//			return false;
		if((type & FileManager.FILES) != 0)
			return super.removeFile(id);
		else if((type & FileManager.FOLDERS) != 0)
			return super.removeEmptyFolder(id);
		return false;
	}
	
	public synchronized List getIdListing(Long id, long type) {
		return super.getIdListing(id, type);
	}
	
	/**
	 * Leiab failid, mille viimatist eksemplari kohalikus hoidlas ei leidu.
	 * @param in List kaheelemendilistest objektide massiividest, milles esimene element on faili ID (Long)
	 * ja teine element selle ID-ga faili muutmisaeg (Date)
	 * @return List failide ID-dest, mille viimatist versiooni kohalikult ei leidu 
	 */
	public synchronized List getDatedIdListing(List in) {
		boolean hTx = haveTransaction();
        Session session = getSession();
		ArrayList result = new ArrayList();
        if(in == null)
        	return result;
		Iterator i = in.iterator();
		
		while(i.hasNext()) {
			Object[] idmodtime = (Object[])i.next();
			beginTransaction();
			
			Query q = session.createQuery(
					"SELECT file.name " +
					"FROM JussFile AS file " +
					"WHERE fileID=? AND modtime=?")
					.setLong(0,((Long)idmodtime[0]).longValue())
					.setString(1,((Date)idmodtime[1]).toString());
			
			if (q.list().size() == 0) {
				result.add((Long)idmodtime[0]);
				log.debug("Missing up-2-date file " + idmodtime[0]);
			}
		}
		
		if (!hTx)
			commitTransaction();
		
		return result;
	}
	
	/**
	 * Salvestab andmebaasi etteantud metadata
	 * @param meta metadata mida salvestada
	 */
	public synchronized void setMetadata(CMetadata meta) {
		if(meta.getId() == null)
			return;
		
		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();

		if(session.get(CMetadata.class, meta.getId()) != null) {
			session.merge(meta);
		} else {
			session.saveOrUpdate(meta);
		}
		
		// ehh?
		Set s = meta.getKeywords();
		setKeywords(s, meta.getId());
		
		if (!hTx) 
			commitTransaction();
	}
	
	/**
	 * Salvestab andmebaasi metadata
	 * @param meta List CMetadata objektidest, mida andmebaasi salvestada
	 */
	public void setMetadataList(List meta) {
		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();

		Iterator i = meta.iterator();
		while(i.hasNext()) {
			Object next = i.next();
			if(next instanceof CMetadata) {
				CMetadata m = (CMetadata)next;
				setMetadata(m);
			}
		}
		
		if (!hTx) 
			commitTransaction();
	}
	
	/**
	 * Seab kohalikus andmebaasis lipu faili eksisteerimise kohta
	 * @param jf JussFile mille lokaalset eksisteerimisinfot muudetakse
	 * @param exists seatav lipp
	 */
	public synchronized void setFileExists(JussFile jf, boolean exists) {
		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();
        
		jf.setExists(exists);
		
		if (!hTx)
			commitTransaction();
	}
}
