/*
 * Created on Apr 25, 2005
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

package shared;

import java.util.*;

import org.apache.log4j.Logger;
import org.hibernate.*;

/**
 * Serveri- ja kliendipoolsete andmebaasidega suhtlemise põhifunktsionaalsust
 * realiseeriv klass. 
 * @author Henri Kuuste
 * @author Taimo Peelo
 * @see client.ClientDBManager
 * @see server.ServerDBManager
 */

public class DBManager {

	/**
	 * @uml.property name="sesLocal"
	 * @uml.associationEnd elementType="org.hibernate.Session" multiplicity="(0 -1)"
	 */
	private final ThreadLocal sesLocal = new ThreadLocal();

	/** 
	 * @uml.property name="txLocal"
	 * @uml.associationEnd elementType="org.hibernate.Transaction" multiplicity="(0 -1)
	 */
	private final ThreadLocal txLocal = new ThreadLocal();

	private static long openedSessionsCount = 0; // mitu sessionit on selle DBManageri t88 k2igus avatud
	private static long closedSessionsCount = 0; // mitu sessionit on selle DBManageri t88 k2igus suletud
	private static long openedTransactionCount = 0;
	private static long closedTransactionCount = 0;
	
	private static final Logger log = Logger.getLogger("juss.client.DBManager");

	/** 
	 * @uml.property name="sessionFactory"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	protected SessionFactory sessionFactory;

	/**
	 * Ajaümardamisfunktsioon mida kasutatakse muutmisaegade samaseks esituseks
	 * andmebaasides.
	 * @param time aeg millisekundites alates 1970. aasta 1. jaanuarist 0:00
	 * @return aeg ümardatud sekundi täpsuseni
	 */
	public static long cut_precision(long time) {
		time -= time % 1000;
		return time;
	}
	
	/**
	 * Funktsioon lõimele vastavale sessioonile ligipääsuks
	 * @return org.hibernate.Session - lõime sessioon
	 */
	public Session getSession() {
		Session s = (Session) sesLocal.get();
		// Open a new Session, if this Thread has none yet
		if (s == null) {
			try {
				s = sessionFactory.openSession();
			} catch (HibernateException ex) {
				log.fatal("DBManager::getSession(): openSession() failed: " + ex);
				System.exit(1);
			}
			//System.out.println("OPENING SESSION " + s.hashCode());
			sesLocal.set(s);
			openedSessionsCount++;
		}
		return s;
	}

	/**
	 * Suleb lõimele kuuluva andmebaasisessiooni
	 */
	public void closeSession() {
		Session s = (Session) sesLocal.get();
		if (s != null) {
			//System.out.println("CLOSING SESSION " + s.hashCode());
			try {
				//NO FLUSH!!!
				s.close();
			} catch (HibernateException ex) {
				log.fatal("DBManager::closeSession(): failed: " + ex);
				System.exit(1);
			}
			closedSessionsCount++;
		}
		sesLocal.set(null);
		if (txLocal.get() != null)
			txLocal.set(null);
	}
	
	/**
     * Alustab või jätkab transaktsiooni andmebaasis sõltuvalt sellest,
     * kas funktsiooni kutsuval lõimel on transaktsioon juba avatud või mitte
     */
    protected void beginTransaction()  {
    	Transaction tx = (Transaction) txLocal.get();
    	if (tx == null) {
    		try {
				tx = getSession().beginTransaction();
			} catch (HibernateException e) {
				log.fatal("DBManager::beginTransaction(): failed: " + e);
				System.exit(1);
			}
            //System.out.println("OPENING TRANSACTION " + tx.hashCode());
            txLocal.set(tx);
            openedTransactionCount++;
    	}
    }
    
    /**
     * Lõpetab kutsuva lõime transaktsiooni andmebaasil. 
     */
    protected void commitTransaction() throws HibernateException {
    	Transaction tx = (Transaction) txLocal.get();
    	if ( tx != null) {
    		txLocal.set(null);
    		closedTransactionCount++;
    		//System.out.println("CLOSING TRANSACTION " + tx.hashCode());    		
    		tx.commit();
    	}
    }
    
    protected void rollbackTransaction() {
		Transaction tx = (Transaction) txLocal.get();
		if (tx != null) {
			txLocal.set(null);
			closedTransactionCount++; // really??
			try {
				tx.rollback();
			} catch (HibernateException e) {
				e.printStackTrace();
				log.fatal("DBManager::rollbackTransaction() failed " + e);
				System.exit(1);
			}
		}
    }

    /**
     * Kontrollib kas lõimel on juba käimas transaktsioon andmebaasiga.
     * <P>
     * Kasutame seda meetodit et saavutada olukorda kus kutsudes mõnd DBManageri meetodit
     * mis omakorda kutsub järgmist DBManageri meetodit ... - kasutatakse vaid ühte transactionit
     * see tähendab et meetodis:<BR/>
     * 1) tehakse kindlaks kas transaction juba eksisteerib: boolean hTx = haveTransaction()<BR/>
     * 2) kutsume välja beginTransactioni - kui transaction juba eksisteerib siis ei alusta see uut<BR/>
     * 3) meetodi l6pus kontrollime hTx väärtust - commitime vaid siis kui see on false, 
     * vastasel korral j2tame transactioni avatuks - kusagil peab aset leidma commit!!
     * <P/>
     * @return kas lõimel on aktiivne transaktsioon andmebaasiga
     */
    public boolean haveTransaction() {
    	return (txLocal.get() != null);
    }
			
	/**
	 * Väljastab standarväljundisse avatud ja suletud sessioonide loendurite väärtused
	 */
	public void reportOpenedAndClosedSessions() {
		log.debug("sessions opened " + openedSessionsCount + ", sessions closed " + closedSessionsCount);
	}
	
	/**
	 * Väljastab standarväljundisse avatud ja suletud transaktsioonide loendurite väärtused
	 */
	public void reportOpenedAndClosedTransactions() {
		log.debug("transactions opened " + openedTransactionCount + ", transactions closed " + closedTransactionCount);
	}
	
	/**
	 * Kontrollib, kas antud ID-ga JussFile eksisteerib andmebaasis
	 * @param id JussFile'i andmebaasi ID
	 * @return kas antud IDga JussFile eksisteerib andmebaasis
	 */
	public boolean existingID(Long id) {
		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();		
        
        JussFile jf = (JussFile) session.createQuery(
			"FROM JussFile WHERE FileID = ?")
			.setLong(0, id.longValue()).uniqueResult();
        
        if (!hTx)
			commitTransaction();
        if (jf == null)
        	return false;
        
        return true;
	}
	
	/**
	 * @param id JussFile'i andmebaasi ID
	 * @param name JussFile'i nimi
	 * @return kas selle ID ja nimega JussFile eksisteerib andmebaasis
	 */
	public boolean existingIDWithName(Long id, String name) {
		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();
		
		JussFile jf = (JussFile) session.createQuery(
			"FROM JussFile WHERE id = ? AND name = ?")
			.setLong(0, id.longValue()).setString(1, name).uniqueResult();
		
		if (!hTx)
			commitTransaction();
        if (jf == null)
        	return false;
        
		return true;
	}
	
	/**
	 * Loeb ja tagastab soovitava IDga JussFile'i andmebaasist
	 * @param id soovitava faili andmebaasi ID
	 * @return andmebaasi id-le vastav JussFile
	 */
	public JussFile getFile(Long id) {
		boolean hTx = haveTransaction();
		
        Session session = getSession();
        beginTransaction();
		JussFile jf = (JussFile) session.get(JussFile.class, id);
		if (!hTx)
			commitTransaction();
		return jf;
	}
	
	/**
	 * Eemaldab faili/kausta kohta käiva informatsiooni andmebaasist.
	 * Klasside ja ülemklasside siseseks kasutamiseks. 
	 * @param id eemaldatava faili/kausta andmebaasi ID
	 * @param folder kas eemaldatav objekt on kaust 
	 * @return kas eemaldamine õnnestus või mitte
	 */
	protected boolean removeFromDataBase(Long id, boolean folder) {
		boolean hTx;
		if (id != null) {
			hTx = haveTransaction();
			Session session = getSession();
			beginTransaction();
			
			JussFile jf = (JussFile) session.get(JussFile.class, id);
			if (jf == null || (jf.isFolder() != folder)) {
				if (!hTx)
					commitTransaction();
				return false;
			}
			session.delete(jf);
			if(jf.getId().equals(jf.getMainversion())) {
				List versions = session.createQuery("FROM JussFile WHERE mainversion = ?")
								.setLong(0, jf.getId().longValue()).list();
				if(versions.size() > 0) {
					Integer maxVersion = (Integer)(session.createQuery("SELECT max(f.version) FROM JussFile AS f WHERE f.mainversion = ?")
									.setLong(0, jf.getId().longValue()).uniqueResult());
					Long newMain = (Long)(session.createQuery("SELECT f.id FROM JussFile AS f WHERE f.mainversion = ? AND f.version = ?")
									.setLong(0, jf.getId().longValue())
									.setInteger(1, maxVersion.intValue()).uniqueResult());
					Iterator i = versions.iterator();
					while(i.hasNext()) {
						JussFile ver = (JussFile)i.next();
						ver.setMainversion(newMain);
					}
				}
			}
			
			// kustutame ka informatsiooni selle faili kohta käivate keywordide kohta 
			// loodame et väga palju lõimesid pole parajasti andmebaasile päringuid tegemas...
			session.createQuery(
					"DELETE FROM FileKeyword WHERE fileID = ?")
					.setLong(0, id.longValue())
					.executeUpdate();			
			
			// kustutada metadata
			session.createQuery(
				"DELETE FROM CMetadata WHERE fileID = ?")
				.setLong(0, id.longValue())
				.executeUpdate();

			if (!hTx)
				commitTransaction();			
			return true;
		}
		return false;
	}
	
	/**
	 * Eemaldab faili kohta käiva informatsiooni andmebaasist 
	 * @param id eemaldatava faili andmebaasi ID 
	 * @return kas eemaldamine õnnestus või mitte
	 */	
	public boolean removeFile(Long id) {
		return removeFromDataBase(id, false);
	}
	
	/**
	 * Eemaldab andmebaasist (tühja) kausta 
	 * @param id eemaldatava kausta andmebaasi ID
	 * @return kas eemaldamine õnnestus või mitte
	 */
	public boolean removeEmptyFolder(Long id) {
		if (id == null)
			return false;
		
		boolean hTx = haveTransaction();
		Session session = getSession();
		beginTransaction();
		
		// kontrollime kas kaust on tõepoolest tühi
		List listing = getIdListing(id, FileManager.FILES | FileManager.FOLDERS);
		if (listing == null || listing.size() != 0) {
			if (!hTx)
				commitTransaction();
			return false;
		}
		
		boolean result = removeFromDataBase(id, true);
		
		if (!hTx)
			commitTransaction();
		
		return result;
	}
	
	/**
	 * Initsialiseerimisfunktsioon - soovitatav välja kutsuda
	 * DBManageri laiendavate klasside konstruktorist. Loob andmebaasi
	 * põhistruktuure, mida veel ei eksisteeri ja kontrollib baasi
	 * vastavust mõnedele nõuetele, mis tööks vajalikud. Kui andmebaas
	 * leitakse olevat vigane, lõpetab programmi töö System.exit() väljakutsega!
	 */
	protected void init() {
        Session session = getSession();		
		beginTransaction();
		
		// kontrollime, ega JussFile'de tabel pole hoopis tyhi, kui on, lisame juurikakirje		
		if (session.createQuery("FROM JussFile").list().size() == 0) {
			JussFile jf = new JussFile(null, "/", true);
			jf.setModtime(new Date());
			jf.setId(new Long(1));
			session.save(jf);
		}
		
		// laeme juurika - selle id peab olema 1, parent null, folder true, name="/"
		JussFile serverRoot = null;
		try {
			serverRoot = (JussFile) session.get(JussFile.class, new Long(1));
		} catch (HibernateException e) {
			// kui juurikaid on rohkem kui yks, sureme
			commitTransaction();
			closeSession();
			log.fatal("no root (multiple?) at db");
			System.exit(1);
		}
		
		// kui juurikas puudub v6i ei vasta n6uetele
		if ((serverRoot == null) || 
		     !serverRoot.getName().equals("/") || 
			 serverRoot.getParent() != null || 
			 !serverRoot.isFolder() ||
			 serverRoot.getLongAncestors() != null
			 ) 
		{
			// sureme
			commitTransaction();
			closeSession();
			log.fatal("corrupted root");
			System.exit(1);
		}
		
		commitTransaction();
		closeSession();
	}

	/**
	 * @param id faili/metadata andmebaasi ID
	 * @param type string "jussfile" | "metadata"
	 * @return tagastab faili või tema metadata viimase muutmise aja  
	 * mõõdetuna millisekundites alates 1. jaanuarist 1970
	 */
	protected Long getModTime(Long id, String type) {
		boolean hTx = haveTransaction();
        Session session = getSession();
		beginTransaction();

		Date modTime = null;
		if(type.equals("metadata")) {
			CMetadata meta = (CMetadata) session.get(CMetadata.class, id);
			if(meta != null)
				modTime = meta.getModTime();
		} else {
			JussFile jf = (JussFile) session.get(JussFile.class, id);
			if(jf != null)
				modTime = jf.getModtime();
		}

		if (!hTx)
			commitTransaction();
		
		if (modTime != null)
			return new Long(modTime.getTime());
		
		return null;
	}
	
	
	/**
	 * Leiab antud IDga faili viimase muutmise aja
	 * @param id faili andmebaasi ID
	 * @return antud IDga faili viimase muutmise aeg
	 */
	public Long getFileModTime(Long id) {
		return getModTime(id, "file");
	}
	
	/**
	 * Leiab antud IDga faili metadata viimase muutmise aja
	 * @param id faili metadata andmebaasi ID (sama mis faili ID)
	 * @return antud IDga faili metadata viimase muutmise aeg
	 */
	public Long getMetaModTime(Long id) {
		return getModTime(id, "metadata");
	}

	/** 
	 * Leiab antud andmebaasi IDga kausta all paiknevate failide/kaustade ID-d ja vastavad muutmise ajad.
	 * @param id kausta id mille kohta listingut soovitakse
	 * @return tagastab Listi Failide ID'dest ja muutmise aegadest selle IDga kausta all. Listi elementideks (Object[] kus indeks 0 on Long ja indeks 1 Date)
	 */	
	public List getIdListing(Long id, long type) {
		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();

		JussFile jf = (JussFile) session.get(JussFile.class, id);
		
		if (jf != null && jf.isFolder()) {
			List result;
			if((type & (FileManager.FILES | FileManager.FOLDERS)) != 0) {
				result =  session.createQuery(
				"SELECT file.id, file.modtime " +
				"FROM JussFile AS file " +
				"WHERE parentID = ?")
				.setLong(0, jf.getId().longValue()).list();
			} else {
				result =  session.createQuery(
						"SELECT file.id, file.modtime " +
						"FROM JussFile AS file " +
						"WHERE parentID = ? AND folder = ?")
						.setLong(0, jf.getId().longValue())
						.setBoolean(1, ((type & FileManager.FOLDERS) != 0?true:false))
						.list();
			}
			
			if (!hTx) 
				commitTransaction();
			
			return result;
		}
		
		if (!hTx)
			commitTransaction();

		return null;
	}

	/**
	 * Leiab antud andmebaasi IDga faili kohta käiva metadata 
	 * @param id faili(metadata) andmebaasi ID
	 * @return antudi IDga faili kohta käiv metadata
	 */
	public CMetadata getMetadata(Long id) {
		if (id == null)
			return null;
		
		boolean hTx = haveTransaction();
        Session session = getSession();
		beginTransaction();
		
		CMetadata result = (CMetadata) session.get(CMetadata.class, id);
		
		if (result == null) {
			if (!hTx)
				commitTransaction();
			return null;
		}
		
		// leiame keywordid mis on seotud selle failiga
				
		List stringList = session.createQuery (
				"SELECT k.keyword FROM Keyword k, FileKeyword fk " +
				"WHERE k.id = fk.keywordID AND fk.fileID = ?")
				.setLong(0,id.longValue()).list();
		
		if (stringList.size() > 0) {
			HashSet hs = new HashSet(); 
			for (Iterator i = stringList.iterator(); i.hasNext(); ) {
				hs.add (new Keyword((String) i.next()));
			}
			result.setKeywords(hs);
		} else {
			result.setKeywords((Set)null);
		}
		
		if (!hTx)
			commitTransaction();

		return result;
	}

	public String getStringPath(Long id) {		
		return getStringPath(getFile(id));
	}
	
	
	/**
	 * Tagastab relatiivse tee JUSSi juurika suhtes kust võib  
	 * leida antud JussFile poolt kirjeldatava faili. Siinse klassi
	 * meetod ei hooli versioonidest ja on mõeldud kasutamiseks kliendis.
	 * @param jf JussFile mille kohta soovitakse StringPathi 
	 * @return  tagastab faili tegeliku asukoha kataloogipuus
	 */
	public String getStringPath(JussFile jf) {
		if(jf != null) {
			StringBuffer sb = new StringBuffer();
			Long [] ancestors = jf.getLongAncestors();
			int len = 0;
			if (ancestors != null)
				len = ancestors.length;

	        Session session = getSession();
			boolean hTx = haveTransaction();
			beginTransaction();
			
			for (int i = 0; i < len; i++) {
				if(i == 0)
					sb.append("/");
				else
					sb.append((getFile(ancestors[i])).getName() + "/");
			}
			sb.append(jf.getName());
			
			if (!hTx)
				commitTransaction();
			
			return sb.toString();
		}
		return null;		
	}
	
	
	/**
	 * Tagastab andmebaasi ID faili kohta mille kohta on teada tema kaust ja nimi.
	 * @param parentId faili kausta andmebaasi ID
	 * @param name faili nimi
	 * @return vastava faili andmebaasi ID
	 */
	public Long getId(Long parentId, String name) {
		Session session = getSession();
		boolean hTx = haveTransaction();
		beginTransaction();

		JussFile jf = (JussFile) session.createQuery(
				"FROM JussFile " +
				"WHERE ParentID = ? AND name = ? AND mainversion = id")
				.setLong(0, parentId.longValue())
				.setString(1, name).uniqueResult();
		
		if (!hTx)
			commitTransaction();

		if(jf == null)
			return null;
		
		return jf.getId();
	}

	
	/**
	 * Leiab antud JussFile kõikide versioonide info.
	 * @param jf JussFile mille versioonide kohta infot soovitakse
	 * @return List JussFile'dega mis on parameetriks antud JussFile versioonid.
	 *         praeguse definitsiooni kohaselt sisaldab ka parameetrina antud 
	 *         JussFile'i ennast
	 */
	public List getVersions(JussFile jf) {
		return getVersions(jf.getMainversion());
	}
	
	/**
	 * Leiab antud andmebaasi IDga faili kõikide versioonide info.
	 * @param id faili ID mille versioonide kohta infot soovitakse
	 * @return List JussFile'dega mis on parameetriks antud JussFile versioonid.
	 *         praeguse definitsiooni kohaselt sisaldab ka parameetrina antud 
	 *         JussFile'i ennast
	 */
	public List getVersions(Long id) {
		Session session = getSession();
		boolean hTx = haveTransaction();
		beginTransaction();

		List l = session.createQuery(
				"FROM JussFile WHERE mainversion = ?")
				.setLong(0, id.longValue()).list();
		
		if (!hTx)
			commitTransaction();
		return l;		
	}

	/**
	 * Leiab antud JussFile kõikide versioonide andmebaasi ID-d
	 * @param id faili ID mille versioonide kohta infot soovitakse
	 * @return List Long tüüpi faili ID-dest mis on parameetriks antud 
	 *         faili versioonid, praeguse definitsiooni kohaselt sisaldab ka parameetrina antud 
	 *         IDd ennast.
	 */
	public List getVersionIds(Long id) {
		Session session = getSession();
		boolean hTx = haveTransaction();
		beginTransaction();

		List l = session.createQuery(
				"SELECT f.id FROM JussFile AS f WHERE f.mainversion = ?")
				.setLong(0, id.longValue()).list();
		
		if (!hTx)
			commitTransaction();
		return l;
		
	}
	
	/**
	 * Seab antud ID-ga faili kohta käivad võtmesõnad
	 * @param s Set Keyword'idest
	 * @param fileID faili ID mille kohta käivad võtmesõnad määratakse
	 */
	protected void setKeywords(Set s, Long fileID) {
		if (s == null)
			return;
		
		Object[] keywords = s.toArray();
		
		Session session = getSession();
		
		for (int i = 0; i < keywords.length; i++) {
		 	Keyword k = (Keyword)keywords[i];
		 	
		 	try {
		 		beginTransaction();
		 		Keyword tk = (Keyword)session.createQuery(
		 				"FROM Keyword WHERE keyword = ?")
						.setString(0,k.getKeyword()).uniqueResult();
				if (tk == null) {//ei ole
					session.save(k);
					FileKeyword fkw = new FileKeyword (fileID, k.getId());
					session.save(fkw);
				} else { // on!
					List r = session.createQuery(
								"FROM FileKeyword WHERE fileID = ? AND keywordID = ?")
								.setLong(0,fileID.longValue())
								.setLong(1, tk.getId().longValue()).list();
					// k2ib juba selle faili kohta!
					if (r.size() > 0)
						break;
					FileKeyword fkw = new FileKeyword (fileID, tk.getId());
					session.save(fkw);
				}
				commitTransaction();
			} catch (HibernateException e) {
				log.debug("Problem adding keyword, " + k + " continuing anyway");
				rollbackTransaction();
				closeSession();
				session = getSession();
				i--;
			}
		 }
		
	}

	/**
	 * @param list
	 *            List Longidest kui FileIDdst
	 * @return tagastab nende IDdga JussFile'd mis eksisteerivad serveri
	 *         andmebaasis
	 */
	
	public List getFileListing(List list) {
		ArrayList result = new ArrayList();
		
		Session session = getSession();
		boolean hTx = haveTransaction();
		beginTransaction();

		for (Iterator i = list.iterator(); i.hasNext(); ) {
			JussFile jf = (JussFile) session.get(JussFile.class, ((Long) i.next()));
			if (jf != null)
				result.add(jf);
		}

		if (!hTx) 
			commitTransaction();
		
		return result;
	}
	
	/**
	 * 
	 * @param crit Kriteeriumid (SearchCriteria) mille järgi otsida
	 * @return List Long tüüpi faili id'dest mis vastavad antud otsingule
	 */
	public List search(SearchCriteria crit) {
		List result = new ArrayList();
		Session session = getSession();
		boolean hTx = haveTransaction();
		beginTransaction();
		List temp;
		if((crit.getType().longValue() & SearchCriteria.KEYWORDS) != 0) {
			temp = session.createQuery(
					"SELECT fk.fileID FROM FileKeyword fk, Keyword k " +
					"WHERE k.id = fk.keywordID AND k.keyword like ?")
					.setString(0, crit.getString()).list();
			result.addAll(temp);
		}
		if((crit.getType().longValue() & SearchCriteria.FILENAME) != 0) {
			temp = session.createQuery(
					"SELECT f.id FROM JussFile AS f WHERE f.name like ?")
					.setString(0, crit.getString()).list();
			result.addAll(temp);
		}
		if((crit.getType().longValue() & SearchCriteria.DESCRIPTION) != 0) {
			temp = session.createQuery(
					"SELECT m.id FROM CMetadata AS m WHERE m.description like ?")
					.setString(0, "%"+crit.getString()+"%").list();
			result.addAll(temp);
		}
		if(!hTx)
			commitTransaction();
		return result;
	}
}
