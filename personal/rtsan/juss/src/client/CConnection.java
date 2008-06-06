/*
 * Created on Apr 21, 2005
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

import java.io.File;
import java.io.ObjectInputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.methods.multipart.MultipartRequestEntity;
import org.apache.commons.httpclient.methods.multipart.Part;
import org.apache.commons.httpclient.methods.multipart.StringPart;
import org.apache.log4j.Logger;

import shared.CConfig;
import shared.CMetadata;
import shared.FileManager;
import shared.JussFile;
import shared.SearchCriteria;

/**
 * Klass mis loob ühenduse ühe kindla serveriga ja kasutab
 * kindlat kausta oma failide jaoks lokaalsel kõvakettal.
 * Selles kaustas peab olema konfiguratsiooni fail connection.conf ja
 * sinna luuakse database kataloog kohaliku (HSQLDB) andmebaasi jaoks
 * ja serverit mingil määral peegeldav failide struktuur.
 * 
 * @author Henri Kuuste
 * @see client.ClientDBManager
 * @see client.JussClient
 */
public class CConnection {

	/**
	 * Konfiguratsiooni fail connection.xml.
	 * 
	 * @uml.property name="conf"
	 * @uml.associationEnd multiplicity="(1 1)"
	 */
	private CConfig conf;

	/**
	 * Kohaliku andmebaasi (HSQLD) frontend.
	 * 
	 * @uml.property name="dbManager"
	 * @uml.associationEnd multiplicity="(1 1)"
	 */
	private ClientDBManager dbManager;

	/**
	 * Info selle kohta kas ühendus serveriga eksisteerib või mitte.
	 */
	private boolean connected;
	
	/**
	 * Viimane veateade.
	 */
	private String errorMsg;
	
	private static final String JUSSFILE = "jussfile";
	private static final String METADATA = "metadata";
	
	private static final Logger log = Logger.getLogger("juss.client.CConnection");

	/**
	 * Avab olemasoleva ühenduse.
	 * 
	 * @param path connection.xml faili sisaldava kataloogi asukohta kohalikus failisüsteemis.
	 */
	public CConnection(String path) {
		conf = new CConfig(path + "/connection.xml");
		if(conf.exists())
			init();
	}
	
	/**
	 * Loeb sisse antud ühenduse konfiguratsiooni ja kui see on korras, siis
	 * avab ka vastava kohaliku andmebaasiga ühenduse.
	 */
	public void init() {
		conf.load();
		if(isValid())
			dbManager = new ClientDBManager(getRoot());
		connected = false;
	}
	
	/**
	 * Tagastab kas antud ühendus on tegelikult serveriga ühenduses või mitte.
	 * <BR/>Seda ei tasu tegelikult väga usaldada. Kui server peaks kuidagi ühenduse katkestama (näiteks http timeout jõuab kätte vms),
	 * siis selle meetodi väärtus ei uuene. Turvalisem on välja kutsuda meetod checkLogin mis kontrollib ka serveri olemasolu ja seda
	 * kas sessioon on ikka veel aktiivne. Viimane meetod uuendab ka antud meetodi tagastatavat väärtust, kuid on aeglasem.
	 * 
	 * @see client.CConnection#checkLogin()
	 */
	public boolean isConnected() {
		return connected;
	}

	/**
	 * @return Serveri URL mida ühendus kasutab.
	 */
	public String getServer() {
		return conf.getStringValue("server");
	}
	
	/**
	 * @return Ühenduse asukoht (peakataloog) kohalikus failisüsteemis.
	 */
	public String getRoot() {
		return conf.getStringValue("root");
	}
	
	/**
	 * @return Ühenduse nimi (ühenduse looja määratud).
	 */
	public String getName() {
		return conf.getStringValue("name");
	}
	
	/**
	 * See meetod peaks teada andma kas ühendus on kasutuskõlblik.
	 * Eelkõige kontrollib ta konfiguratsiooni õigsust.
	 * 
	 * @return Kas ühendus on korras või mitte.
	 */
	public boolean isValid() {
		return (getRoot() != null && getServer() != null && getName() != null);
	}
	
	/**
	 * Loob ühenduse serveriga.
	 * 
	 * @param user Kasutaja nimi serveris.
	 * @param pass Kasutajanimele vastav parool.
	 * @return Kas ühenduse loomine õnnestus või mitte.
	 */
	public boolean login(String user, String pass) {
		connected = false;
		log.info("Logging in to " + getServer());
		PostMethod p;
		try {
			p = new PostMethod(getServer() + "Login");
		} catch (IllegalArgumentException e) {
			setError("Invalid server URI!");
			return false;
		}
		p.addParameter("login", "Log in");
		p.addParameter("user", user);
		p.addParameter("pass", pass);
		try {
			int statusCode = JussClient.getClient().executeMethod(p);
			if(statusCode != HttpStatus.SC_OK)  {
				setError("\tAuthentication failed: " + p.getStatusLine());
				return false;
			} else {
				log.info("\tLogged in!");
				connected = true;
				return true;
			}
		} catch (Exception e) {
			setError("login: Login failed: " + e);
			return false;
		} finally {
			p.releaseConnection();
		}
	}

	/**
	 * @return Kas ühendus serveriga eksisteerib või mitte.
	 * @see client.CConnection#login(String, String)
	 */
	public boolean checkLogin() {
		connected = false;
		PostMethod p = new PostMethod(getServer() + "Login");
		try {
			int statusCode = JussClient.getClient().executeMethod(p);
			if(statusCode != HttpStatus.SC_OK)  {
				return false;
			} else {
				connected = true;
				return true;
			}
		} catch (Exception e) {
			setError("checkLogin: login check failed: " + e);
			return false;
		} finally {
			p.releaseConnection();
		}
	}

	/**
	 * Kontrollib kas antud andmed on kohalikus ja serveri andmebaasis kooskõlas.
	 * 
	 * @param id Faili, mida kontrollida, ID andmebaasis.
	 * @param type JUSSFILE või METADATA
	 * @return <B>true</B> kui andmed on samad <B>false</B> vastaseljuhul
	 */
	public boolean verifyData(Long id, String type) {
		PostMethod p = new PostMethod(getServer() + "VerifyData");
		p.addParameter("id", id.toString());
		p.addParameter("type", type);
		p.addParameter("modtime", dbManager.getModTime(id, type).toString());
		try {
			int statusCode = JussClient.getClient().executeMethod(p);
			if(statusCode != HttpStatus.SC_OK) {
				return false;
			}
		} catch(Exception e) {
			setError("verifyData: \"" + id + "\": Failed to execute request: " + e);
			return false;
		} finally {
			p.releaseConnection();
		}	
		return true;
	}

	/**
	 * Tagastab nimekirja failidest mis asuvad etteantud kataloogis.
	 * Kui ühendus serveriga eksisteerib, siis küsitakse serverilt ka aegunud või
	 * puuduvate failide info ja uuendatakse kohalikku andmebaasi.
	 * 
	 * @param id Kataloogi ID, mille failide nimekirja soovitakse.
	 * @param remote Kas uuendada faile serverist. 
	 * @return java.util.List mille elemendid on klassist <B>JussFile</B>.
	 */
	public List getListing(final Long id, boolean remote, final long type) {
		// Kontrollime kas meil endal on andmed olemas ehk juba
		if(remote && isConnected()) {
			NetworkTask task = new NetworkTask("Listing: " + dbManager.getStringPath(id)) {
				public void runSupport() {
					List ids = getIdListing(id, type);
					removeDeadFiles(ids, id);
					List dids = dbManager.getDatedIdListing(ids);
					//TODO: Välja mõelda mida sellisel juhul erroritega teha. Eraldi CStatus klass?
					getFilesFromServer(dids);
					createMissingFolders(ids);
					if(dids.size() > 0) {
						setData("update", "true");
					} else {
						setData("update", "false");
					}
				}
			};
			task.setData("type", new String("ListingTask"));
			task.setData("id", id);
			task.setData("connection", this);
			NetworkThread.getInstance().addTask(task);
		}
		List l = dbManager.getFolderContents(id);
		checkIfFilesExist(l);
		return l;
	}
	
	/**
	 * Loob ettenatud kataloogid kohalikus failisüsteemis.
	 * 
	 * @param ids java.util.List kataloogide ID'dest (<B>Long</B>)
	 * @see client.CConnection#createMissingFolder(Long)
	 */
	public void createMissingFolders(List ids) {
		if(ids == null)
			return;
		Iterator i = ids.iterator();
		while(i.hasNext()) {
			Long id = (Long)((Object[])i.next())[0];
			createMissingFolder(id);
		}
	}
	
	/**
	 * Loob ette antud kataloogi kohalikku failisüsteemi.
	 * 
	 * @param id tüüpi kataloogi ID. 
	 */
	public void createMissingFolder(Long id) {
		JussFile jf = getFile(id);
		if(jf != null && jf.isFolder()) {
			File f = new File(getRoot() + dbManager.getStringPath(id));
			if(!f.exists()) {
				f.mkdir();
			}
		}
	}
	
	/**
	 * Loob kohalikku failisüsteemi ette antud ID järgi kõik puuduvad kataloogid selle ID'ga faili path'il.
	 * @param id Faili ID mille path'il asuvad kataloogid tuleks luua.
	 */
	public void createMissingAncestorFolders(Long id) {
		createMissingAncestorFolders(getFile(id));
	}
	
	/**
	 * Loob kohalikku failisüsteemi ette antud JussFile järgi kõik puuduvad kataloogid selle faili path'il.
	 * @param jf JussFile mille path'il asuvad kataloogid tuleks luua.
	 */
	public void createMissingAncestorFolders(JussFile jf) {
		Long[] ancestors = jf.getLongAncestors();
		for(int i = 1; i < ancestors.length; i++) {
			File f = new File(getRoot() + dbManager.getStringPath(ancestors[i]));
			if(!f.exists()) {
				f.mkdir();			
			}
		}		
	}
	
	/**
	 * Tagastab nimekirja faili ID'dest ja muutmise aegadest, mis asuvad ette antud ID'ga kausta all.
	 * See info saadakse alati serverist. Niisiis antud meetodit ei tasuks välja kutsuda kui serveriga ühendus puudub.
	 * Meetod kohalikku andmebaasi ei muuda.
	 * @param id Kausta ID, mille alt faile otsida.
	 * @return java.util.List failide ID'dega ja muutmise aegadega, mis asuvad antud kausta all (elemendi tüüp <B>Long</B>)
	 */
	public List getIdListing(Long id, long type) {
		List l = null;
		PostMethod p = new PostMethod(getServer() + "ListIds");
		p.addParameter("id", id.toString());
		p.addParameter("type", Long.toString(type));
		try {
			int statusCode = JussClient.getClient().executeMethod(p);
			if (statusCode != HttpStatus.SC_OK) {
				setError("getIdListing: \"" + id + "\": Server error: " + p.getStatusLine());
				return null;
			}
			ObjectInputStream os = new ObjectInputStream(p.getResponseBodyAsStream());
			try {
				l = (List) os.readObject();
			} catch (Exception e) {
				setError("getIdListing: \"" + id + "\": Failed to read server response: " + e);
				return null;
			}
		} catch (Exception e) {
			setError("getIdListing: \"" + id + "\": Failed to get listing from server: " + e);
			return null;
		} finally {
			p.releaseConnection();
		}		
		return l;
	}
	
	/**
	 * Meetod, mis eemaldab kohalikust andmebaasist failide
	 * kanded, mida serveris enam ei ole. Vajab töötamiseks serveriga
	 * ühenduse olemasolu.
	 * @param id Kataloogi ID mille all olevaid faile kontrollida.
	 */
	public void removeDeadFiles(Long id) {
		removeDeadFiles(getIdListing(id, FileManager.FILES | FileManager.FOLDERS), id);
	}
	
	/**
	 * Meetod, mis eemaldab kohalikust andmebaasist failide
	 * kanded, mida serveris enam ei ole.
	 * @param l java.util.List ID'dest ja muutmise aegadest (Object[] kus indeks 0 on ID (Long). Failid peavad kõik asuma ühe kausta all.
	 * @param id Kataloogi ID mille all antud failid asuvad.
	 */
	public void removeDeadFiles(List l, Long id) {
		ArrayList ids = new ArrayList();
		if(l != null) {
			Iterator i = l.iterator();
			while(i.hasNext()) {
				ids.add(((Object[])i.next())[0]);
			}
		}
		dbManager.removeDeadFiles(ids, id, getRoot());		
	}
	
	/**
	 * Eemaldab faili süsteemist.
	 * @param id Faili ID, mida eemaldada.
	 * @return <B>true</B> kui eemaldamine õnnestus <B>false</B> vastasel juhul.
	 */
	public boolean removeFile(Long id, long type) {
		PostMethod p = new PostMethod(getServer() + "RemoveFile");
		p.addParameter("id", id.toString());
		p.addParameter("type", Long.toString(type));
		try {
			int statusCode = JussClient.getClient().executeMethod(p);
			if (statusCode != HttpStatus.SC_OK) {
				setError("removeFile: \"" + id + "\": Server error: " + p.getStatusLine());
				return false;
			}
			if(!dbManager.removeFile(id, getRoot(), type)) {
				setError("removeFile: \"" + id + "\": Failed to remove file from local database");
				return false;
			}
		} catch (Exception e) {
			setError("removeFile: \"" + id + "\": Failed to remove file: " + e);
			return false;
		} finally {
			p.releaseConnection();
		}		
		return true;
	}
	
	/**
	 * Kontrollib õigusi antud failil. Meetodi tööks on vajalik ühenduse olemasolu
	 * serveriga.
	 * 
	 * @param id Faili ID mille õigusi kontrollida.
	 * @param accessType Mingi kombinatsioon väärtustest FileManager.F_READ, FileManager.F_WRITE, FileManager.F_CREATE ja FileManager.F_DELETE.
	 * @return <B>true</B> kui kõik ette antud õigused on olemas <B>false</B> vastasel juhul.
	 * @see server.PermissionManager#isFileAccessible(String, Long, String)
	 */
	public boolean checkPermission(Long id, String accessType) {
		PostMethod p = new PostMethod(getServer() + "CheckPermission");
		p.addParameter("id", id.toString());
		p.addParameter("accessType", accessType);
		try {
			int statusCode = JussClient.getClient().executeMethod(p);
			if(statusCode == HttpStatus.SC_OK) {
				return true;
			}
		} catch(Exception e) {
			setError("checkPermission: " + id + ": Failed to get permissions " + accessType + ": " + e);
			return false;
		} finally {
			p.releaseConnection();
		}			
		return false;
	}
	
	/**
	 * Tagastab ette antud faili metadata. Kui serveriga on ühendus
	 * olemas, siis uuendatakse andmeid ka serverist.
	 * 
	 * @param id Faili ID, mille metadatat soovitakse.
	 * @return Antud faili metadata.
	 */
	public CMetadata getMetadata(Long id) {
		CMetadata meta = null;
		// Kontrollime kas meil endal on andmed olemas ehk juba
		if(isConnected()) {
			if (!dbManager.hasMetadata(id) || !verifyData(id, METADATA)) {
				log.debug("Getting meta for " + id + " from server.");
				ArrayList l = new ArrayList();
				l.add(id);
				getMetadataFromServer(l);
			}
		}
		meta = dbManager.getMetadata(id);
		return meta;
	}
	
	/**
	 * Meetod mis otsib metadata serverist ja salvestab selle kohalikku andmebaasi.
	 * 
	 * @param ids java.util.List failid ID'dest (<B>Long</B>), mille metadatat peaks uuendama.
	 * @return <B>true</B> kui operatsioon õnnestus <B>false</B> vastasel juhul.
	 */
	public boolean getMetadataFromServer(List ids) {
		PostMethod p = new PostMethod(getServer() + "GetMetadata");
		try {
			Part[] parts = { new ObjectPart(ids) };
			p.setRequestEntity(new MultipartRequestEntity(parts, p.getParams()));
			int statusCode = JussClient.getClient().executeMethod(p);
			if (statusCode != HttpStatus.SC_OK) {
				setError("getMetadataFromServer: Server error: " + p.getStatusLine());
				return false;
			}
			ObjectInputStream os = new ObjectInputStream(p.getResponseBodyAsStream());
			List meta = null;
			try {
				meta = (List) os.readObject();
				dbManager.setMetadataList(meta);
			} catch (Exception e) {
				setError("getMetadataFromServer: Failed to set or read metadata: " + e);
				return false;
			}
		} catch (Exception e) {
			setError("getMetadataFromServer: Failed to get server response: " + e);
			return false;
		} finally {
			p.releaseConnection();
		}
		return true;
	}

	/**
	 * @return Antud ühendusega seotud andmebaasi frontend. null kui andmebaas ei käi.
	 */
	public ClientDBManager getDBManager() {
		return dbManager;
	}
	
	/**
	 * Meetod mis sulgeb antud ühenduse (eelkõige siis vabastab kohaliku andmebaasi).
	 */
	public void close() {
		if(dbManager != null)
			dbManager.shutdown();
	}
	
	/**
	 * Tagastab ette antud ID'le vastava JussFile. Kui eksisteerib ühendus
	 * serveriga siis uuendatakse antud faili andmeid ka serverist.
	 * 
	 * @param id Faili ID, mida soovitakse saada.
	 * @return JussFile, mis vastab antud ID'le. <B>null</B> kui fail ei eksisteeri või teda ei õnnestunud kätte saada.
	 */
	public JussFile getFile(Long id) {
		JussFile jf = null;
		// Kontrollime kas meil endal on andmed olemas ehk juba
		if(isConnected()) {
			if(!dbManager.hasData(id) || !verifyData(id, JUSSFILE)) {
				ArrayList l = new ArrayList();
				l.add(id);
				// TODO: Välja mõelda mida sellisel juhul erroritega teha. Eraldi CStatus klass?
				getJussFilesFromServer(l);
			}
		}
		jf = dbManager.getFile(id);
		return jf;
	}
	
	/**
	 * Uuendab serverist failide info ja metadata. Vajalik serveriga ühenduse olemasolu.
	 * Meetod ei kontrolli kas failid vajavad uuendamist.
	 * 
	 * @param ids java.util.List failide ID'dest (Long) mille andmeid soovitakse uuendada.
	 * @return <B>true</B> kui uuendamine õnnestus <B>false</B> vastasel juhul.
	 */
	public boolean getFilesFromServer(List ids) {
		return getJussFilesFromServer(ids) && getMetadataFromServer(ids);
	}
	
	/**
	 * Uuendab serverist failide info. Vajalik serveriga ühenduse olemasolu.
	 * Meetod ei kontrolli kas failid vajavad uuendamist.
	 * 
	 * @param ids java.util.List failide ID'dest (Long) mille infot soovitakse uuendada.
	 * @return <B>true</B> kui operatsioon õnnestus <B>false</B> vastasel juhul.
	 */
	public boolean getJussFilesFromServer(List ids) {
		PostMethod p = new PostMethod(getServer() + "GetFiles");
		try {
			Part[] parts = { new ObjectPart(ids) };
			p.setRequestEntity(new MultipartRequestEntity(parts, p.getParams()));
			int statusCode = JussClient.getClient().executeMethod(p);
			if (statusCode != HttpStatus.SC_OK) {
				setError("getJussFilesFromServer: Server error: " + p.getStatusLine());
				return false;
			}
			ObjectInputStream os;
			try {
				os = new ObjectInputStream(p.getResponseBodyAsStream());
				List files = (List) os.readObject();
				dbManager.addFileList(files);
			} catch (Exception e) {
				setError("getJussFilesFromServer: Failed to get files from server: " + e);
				return false;
			}
		} catch (Exception e) {
			setError("getJussFilesFromServer: Failed to execute server request: " + e);
			return false;
		} finally {
			p.releaseConnection();
		}		
		return true;
	}
	
	/**
	 * Loob uue kataloogi süsteemi. Meetodi toimimiseks on vajalik ühendus serveriga.
	 * Meetod loob kohe ka vastava kataloogi kohalikus failisüsteemis.
	 * @param parentId Ülemkataloogi ID kuhu uus kataloog luua.
	 * @param name Uue kataloogi nimi.
	 * @param meta Uue kataloogi metadata.
	 * @return <B>true</B> kui kataloogi loomine õnnestus <B>false</B> vastasel juhul.
	 */
	public boolean createFolder(Long parentId, String name, CMetadata meta) {
		if(name == null || name.length() == 0) {
			setError("createFolder: Illegal folder name (none given or 0 length)");
			return false;
		}
//		 TODO: Välja mõelda mida sellisel juhul erroritega teha. Eraldi CStatus klass?
		if(!checkPermission(parentId, FileManager.F_CREATE)) {
			setError("createFolder: No right to create the folder \"" + name + "\"");
			return false;
		}
		Long existingId = getDBManager().getId(parentId, name);
		if(existingId != null) {
			setError("createFolder: Folder already exists in the database \"" + name + "\"");
			return false;
		}
		PostMethod p = new PostMethod(getServer() + "CreateFolder");
		try {
			Part[] parts = { new StringPart("parentId", parentId.toString()),
					new ObjectPart(meta),
					new StringPart("name", name)};
			p.setRequestEntity(new MultipartRequestEntity(parts, p.getParams()));
			int statusCode = JussClient.getClient().executeMethod(p);
			if(statusCode != HttpStatus.SC_OK) {
				setError("createFolder: \"" + name + "\": Server error: " + p.getStatusLine());
				return false;
			}
			Long newId = Long.valueOf(p.getResponseHeader("newId").getValue());
			createMissingAncestorFolders(getFile(newId));
			getMetadata(newId);
			File f = new File(getRoot() + dbManager.getStringPath(newId));
			f.mkdir();
		} catch(Exception e) {
			setError("createFolder: Could not create folder \"" + name + "\": " + e);
			return false;
		} finally {
			p.releaseConnection();
		}
		return true;
	}
	
	/**
	 * Kontrollib kas fail eksisteerib kohalikus failisüsteemis.
	 * Meetod uuendab vastavat välja ka andmebaasis.
	 * 
	 * @param id Faili ID mida kontrollida.
	 * @return <B>true</B> kui fail eksisteerib <B>false</B> vastasel juhul.
	 */
	public boolean fileExists(Long id) {
		return fileExists(getFile(id));
	}
	
	/**
	 * Kontrollib kas fail eksisteerib kohalikus failisüsteemis.
	 * Meetod uuendab vastavat välja ka andmebaasis.
	 * Eelkõige kontrollib antud meetod kas failid mida andmebaas väidab
	 * eksisteerivat, ka tegelikult failisüsteemis olemas on (st kasutaja pole neid
	 * näiteks maha kustutanud).
	 * 
	 * @param jf JussFile mille eksistentsi kontrollida.
	 * @return <B>true</B> kui fail eksisteerib <B>false</B> vastasel juhul.
	 */
	public boolean fileExists(JussFile jf) {
		File f = new File(getRoot() + dbManager.getStringPath(jf));
		if(jf.getExists()) {
			boolean exists = f.exists();
			if(exists != jf.getExists()) {
				dbManager.setFileExists(jf, exists);
			}
		}
		return jf.getExists();
	}
	
	/**
	 * Meetod mis kontrollib kas antud fail eksisteerib kohalikus failisüsteemis ja uuendab
	 * ka andmebaasi. Seda meetodit läheb eelkõige vaja uue faili loomisel (download/upload).
	 * Niisiis juhul kui on üsna kindel, et fail nüüd eksisteerib ja soovitakse andmebaasi ka
	 * sellest teavitada.
	 * Meetod muudab vastavaid välju ka teistel faili versioonidel.
	 * 
	 * @param jf JussFile mille eksistentsi kontrollida.
	 */
	public void setNewFileExists(JussFile jf) {
		File f = new File(getRoot() + dbManager.getStringPath(jf));
		boolean exists = f.exists();
		if(exists != jf.getExists())
			dbManager.setFileExists(jf, exists);
		List l = dbManager.getVersions(jf);
		Iterator i = l.iterator();
		while(i.hasNext()) {
			JussFile ver = (JussFile)i.next();
			if(ver != jf)
				dbManager.setFileExists(ver, false);
		}
	}
	
	/**
	 * Sets the modification time of the file in the filesystem to the same value
	 * as the database file modification time.
	 * Used by upload and download to set the modtime of the newly created files.
	 * @param jf The file to set the modification time on
	 */
	public void setModTime(JussFile jf) {
		File f = new File(getRoot() + dbManager.getStringPath(jf));
		f.setLastModified(jf.getModtime().getTime());
	}
	
	/**
	 * Kutsub välja meetodit fileExists failide nimekirjal.
	 * @param files java.util.List failidest (<B>JussFile</B>)
	 * @see client.CConnection#fileExists(JussFile)
	 */
	public void checkIfFilesExist(List files) {
		Iterator i = files.iterator();
		while(i.hasNext()) {
			Object next = i.next();
			if(next instanceof JussFile)
				fileExists((JussFile)next);
		}
	}
	
	/**
	 * Teostab failide otsingu ette antud kriteeriumite põhjal.
	 * @param c Kriteeriumid mille järgi faile otsida.
	 * @return java.utli.List failidest (<B>JussFile</B>), mis vastavad antud kriteeriumitele. List võib sisaldada sama elementi ka mitu korda, kui element vastas rohkem kui ühele kriteeriumile (seda tuleks ehk parandada).
	 */
	public List search(SearchCriteria c) {
		return dbManager.getFileListing(dbManager.search(c)); // kohalik otsing momendil
	}
	
	/**
	 * Meetod viimati esinenud veateate sättimiseks.
	 * Meetod väljastab veateate ka System.err stream'i.
	 * @param str veateade.
	 */
	private void setError(String str) {
		errorMsg = "Connection error: " + str;
		log.warn(errorMsg);
	}
	
	/**
	 * @return Viimati esinenud veateade.
	 */
	public String getErrorMessage() {
		return errorMsg;
	}
	
	/**
	 * Checks the modification time of the JussFile against the one on the filesystem.
	 * @param jf The file to check
	 * @return -1 if the file on disk is older, 0 if the time is the same, 1 if the file on disk is newer
	 */
	public int checkModTime(JussFile jf) {
		File f = new File(getRoot() + getDBManager().getStringPath(jf));
		long modtime = f.lastModified();
		long lmodtime = jf.getModtime().getTime();
		if(lmodtime < modtime) {
			return 1;
		} else if(lmodtime > modtime) {
			return -1;
		} else {
			return 0;
		}
	}
}
