/*
 * Created on Apr 21, 2005
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

package server;

import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import shared.*;

import org.hibernate.*;
import org.hibernate.cfg.Configuration;
import org.apache.log4j.Logger;


/**
 * JUSSi serveripoolse andmebaasiga suhtlemist korraldav klass
 * @author Taimo Peelo, Henri Kuuste
 */

// NB! Transactionitest:
// However, it is intended that there be at most one uncommitted Transaction
// associated with a particular Session at any time.

public class ServerDBManager extends DBManager {
	private static final Logger log = Logger.getLogger("juss.server.ServerDBManager");
	
	/** vt DBManager.init() */
	protected void init() {
		System.out.println("Creating ServerDBManager");
		super.init();
	}

	/** vaikekonstruktor, loob uue ServerDBManageri, muuhulgas kutsub
	 *  funktsiooni DBmanager.init() */
	public ServerDBManager() {
		try {
			// Create the SessionFactory
            sessionFactory = new Configuration().configure().buildSessionFactory();
        } catch (Throwable ex) {
            // Make sure you log the exception, as it might be swallowed
            log.fatal("Initial SessionFactory creation failed." + ex);
            throw new ExceptionInInitializerError(ex);
        }
        this.init();
	}
	
	/**
	 * Võrdleb antud IDga faili/metadata viimase muutmise aega parameetriks antud ajaga
	 * @param id faili/kausta andmebaasi ID mille muutmisaega kontrollitakse
	 * @param modtime aeg, millega võrreldakse tegelikku muutmisaega
	 * @param type kui väärtuseks on "metadata" kontrollitakse metadata muutmise
	 *             aega, mitte faili oma
	 * @return kas faili/metadata muutmise aeg on sama, mis parameetriks antud aeg
	 */
	public boolean verifyModTime(Long id, Long modtime, String type) {
		Long idModTime = getModTime(id, type);
		if (idModTime == null)
			return false;
		return idModTime.equals(modtime);
	}	 
	
	/**
	 * Loob uue JussFile ja salvestab tema kohta käiva informatsiooni andmebaasi.
	 * @param parentID kausta ID, kuhu uus fail/kaust tekitatakse
	 * @param name uue faili/kausta nimi
	 * @param metadata uue failiga seostatav metadata
	 * @param folder kas tegemist on kaustaga või mitte
	 * @param type lisamise tyyp, FileManager.OVERWRITE | FileManager.NEW_VERSION
	 * @param size faili suurus baitides
	 * @return lisatud failile omistatud andmebaasisisene ID
	 */
	private Long addJussFile(Long parentID, String name, CMetadata metadata, boolean folder, String type, Long size) {
        Session session = getSession();
        beginTransaction();
		
		JussFile jf = (JussFile) session.createQuery(
				"FROM JussFile WHERE ParentID = ? AND name = ? AND mainversion = id")
				.setLong(0, parentID.longValue()).setString(1, name).uniqueResult();
		
		// MYSQL ja HSQLDB salvestavad selle erineva täpsusega, niisiis
		// paneme MySQL täpsusega siia.
		Date dt = new Date();
		long time = cut_precision(dt.getTime());

		boolean fileExists = false;
		
		boolean version = type.equals(FileManager.NEW_VERSION);

		if(jf != null) {
			if (jf.isFolder() != folder) {
				commitTransaction();
				return null; // kui meil juba eksisteerib sama tyypi sama nimega JussFile
			}
			fileExists = true;
		}
		Long oldMainVersion = null;
		int ver = 1;
		if(fileExists && version) {
			oldMainVersion = jf.getMainversion();
			ver = jf.getVersion() + 1;
		}
		
		if (version || !fileExists) {
			jf = new JussFile();
			jf.setFolder(folder);
			jf.setName(name);
			jf.setParent(parentID);
			jf.setVersion(ver);
			jf.setSize(size);
			JussFile parent = (JussFile) session.get(JussFile.class, parentID);

			int len = 0;
			if (parent.getLongAncestors() != null)
				len = parent.getLongAncestors().length;
			Long[] ancestors = new Long[len + 1];
			if (len > 0)
				System.arraycopy(parent.getLongAncestors(), 0, ancestors, 0, len);
			ancestors[len] = parent.getId();
			jf.setLongAncestors(ancestors);

			parent.setModtime(new Date(time)); // sea ka vanema muutmisaeg!
		}

		jf.setModtime(new Date(time));

		if(!fileExists || version)
			session.save(jf);

		System.out.println("Added to database \"" + name + " "
				+ jf.getId() + " " + jf.getModtime() + "\"");


		Long newVersion = jf.getId();
		jf.setMainversion(jf.getId());
		
		if(fileExists && version) {
			if (newVersion != null) {
				List l = session.createQuery(
						"FROM JussFile WHERE mainversion = ?").setLong(0,
						oldMainVersion.longValue()).list();
				Iterator it = l.iterator();
				while (it.hasNext()) {
					JussFile lv = (JussFile) it.next();
					lv.setMainversion(jf.getId());
					lv.setModtime(new Date(time));
				}
			}
		}
		
		// commit here - if problems arise setting file's metadata,
		// recovery can be done without re-commiting file information
		try {
			commitTransaction();
		} catch (HibernateException e) {
			e.printStackTrace();
			rollbackTransaction();
			closeSession();
			log.fatal("ServerDBManager::addJussFile() failed to add file :" + e);
			System.exit(1);
		}
		
		// tegeleme ka metadataga veidike
		if (metadata != null && (!fileExists || version)) {
			Set s = null;
			try {
				beginTransaction();
				metadata.setId(jf.getId());
				metadata.setModTime(new Date(time));
				s = metadata.getKeywords();
				session.save(metadata);
				commitTransaction();
			} catch (HibernateException e1) {
				e1.printStackTrace();
				rollbackTransaction();
				closeSession();
				log.fatal("ServerDBManager::addJussFile() failed to add metadata :" + e1);
				System.exit(1);
			}
			setKeywords(s, metadata.getId());
		}
		return newVersion;
	}
	
	
	/**
	 * Leiab antud faili asukoha failisüsteemis relatiivselt serverijuure suhtes.
	 * @param id faili andmebaasi ID
	 * @return faili asukohta näitav sõne (relatiivne serverijuure suhtes)
	 */
	public String getLocalStringPath(Long id) {
		JussFile jf = getFile(id);
		if(jf == null)
			return null;
		String path = getStringPath(id) + "_" + jf.getVersion();
		return path;
	}

	/**
	 * Saab parameetriks faili ID, faili nime ja operatsioonitüübi mida selle failiga soovitakse
	 * ette võtta. Tagastab parameetreid arvestades serverijuure suhtes relatiivse asukoha, kus
	 * see fail peaks asuma.
	 * 
	 * Spetsiifiline funktsioon, kasutusel ainult UploadServletis. Ettevaatust kasutamisel :) 
	 * 
	 * @param id faili andmebaasi ID
	 * @param name faili nimi
	 * @param type FileManager.OVERWRITE | FileManager.NEW_VERSION
	 * @return parameetritele vastava faili asukohta määrav sõne (relatiivne serverijuure suhtes) 
	 */
	public String getLocalStringPath(Long id, String name, String type) {
        String path = getStringPath(id);
		if(path == null)
			return null;

		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();
		
        JussFile jf = (JussFile) session.createQuery(
		"FROM JussFile WHERE ParentID = ? AND name = ? AND mainversion = id")
		.setLong(0, id.longValue()).setString(1, name).uniqueResult();
	
        if(jf == null) {
        	path += "/" + name + "_1";
        } else {
        	int version = jf.getVersion();
        	if(type.equals(FileManager.NEW_VERSION))
        		version++;
        	path += "/" + jf.getName() + "_" + version; 
        }
        
		if (!hTx)
			commitTransaction();
			
		return path;
	}

	/**
	 * Loob uut faili iseloomustava JussFile ja salvestab tema kohta 
	 * käiva informatsiooni andmebaasi.
	 * @param parentID kausta ID, kuhu uus fail tekitatakse
	 * @param name uue faili nimi
	 * @param metadata uue failiga seostatav metadata
	 * @param type lisamise tyyp, FileManager.OVERWRITE | FileManager.NEW_VERSION
	 * @return null kui lisamine ei õnnestunud, vastasel korral loodud JussFile'le omistatud ID
	 */
	public Long addFile(Long parentID, String name, CMetadata metadata, String type) {
		return this.addJussFile(parentID, name, metadata, false, type, null);
	}
	
	/**
	 * Loob uut faili iseloomustava JussFile ja salvestab tema kohta 
	 * käiva informatsiooni andmebaasi.
	 * @param parentID kausta ID, kuhu uus fail tekitatakse
	 * @param name uue faili nimi
	 * @param metadata uue failiga seostatav metadata
	 * @param type lisamise tyyp, FileManager.OVERWRITE | FileManager.NEW_VERSION
	 * @param size lisatava faili suurus baitides
	 * @return null kui lisamine ei õnnestunud, vastasel korral loodud JussFile'le omistatud ID
	 */
	public Long addFile(Long parentID, String name, CMetadata metadata, String type, Long size) {
		return this.addJussFile(parentID, name, metadata, false, type, size);
	}
	
	/**
	 * Loob uue kausta iseloomustava JussFile ja salvestab tema kohta 
	 * käiva informatsiooni andmebaasi.
	 * @param parentID ülemkausta ID, kuhu uus kaust tekitatakse
	 * @param name uue kausta nimi
	 * @param metadata uue kaustaga seostatav metadata
	 * @return null kui lisamine ei õnnestunud, vastasel korral loodud JussFile'le omistatud ID
	 */
	public Long addFolder(Long parentID, String name, CMetadata metadata) {
		System.out.println("Creating folder " + parentID + " " + name);
		return this.addJussFile(parentID, name, metadata, true, FileManager.OVERWRITE, null);		
	}
	
	/**
	 * Leiab failide kohta käiva metadata 
	 * @param idlist Long objektide List faili ID-dega, mille kohta metadatat soovitakse
	 * @return List metadata failide kohta, List CMetadata objektidest
	 */
	public List getMetadataList(List idlist) {
		boolean hTx = haveTransaction();
        Session session = getSession();
        beginTransaction();
        
		List result = new ArrayList();
		for (Iterator i = idlist.iterator(); i.hasNext(); )
			result.add(getMetadata((Long)i.next()));
		
		if (!hTx)
			commitTransaction();
		
		return result;
	}
	
	public boolean removeFile(Long id) {
		if(!FileManager.deleteFile(JussServer.getRoot() + getLocalStringPath(id)))
			return false;
		return super.removeFile(id);
	}
	
	public boolean removeEmptyFolderFS(Long id) {
		if(removeEmptyFolder(id))
			return !FileManager.deleteFile(JussServer.getRoot() + getLocalStringPath(id));
		return false;
	}
}
