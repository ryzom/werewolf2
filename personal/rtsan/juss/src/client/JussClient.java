/*
 * Created on Apr 19, 2005
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

import shared.*;

import org.apache.commons.httpclient.*;
import org.apache.commons.httpclient.params.*;
import org.apache.log4j.Logger;

/**
 * JUSS s�steemi peamine klass mille kaudu luuakse �hendusi, laaditakse faile
 * alla ja �les. Selle klassi l�bi suhtelb GUI JUSS'i backend'iga.
 * @author Henri Kuuste
 */

public class JussClient {
	/**
	 * �le�ldine kliendi konfiguratsioon.
	 */
	private static CConfig conf = new CConfig("client.conf.xml");
	/**
	 * Http �henduse loomiseks client.
	 */
	private static HttpClient client;
	/**
	 * �le kliendiline transpordi (download/upload) haldamise s�steem.
	 */
	private static TransferManager transferMan;
	/**
	 * Map �hendustest kujul "kohalik path" -> CConnection
	 */
	private static HashMap connections;
	
	/**
	 * Log4j logger
	 */
	private static final Logger log = Logger.getLogger("juss.client.JussClient");
	
	/**
	 * Meetod mis initsialiseerib kliendi. Loob ja loeb sisse konfiguratsiooni, loob http kliendi, transpordi halduri ja ka Map'i �henduste hoidmiseks.
	 * Seda peab enne JussClient'i kasutamist v�lja kutsuma. Soovitatavalt siis programmi k�ivitudes. 
	 */
	public static void init() {
		log.info("Starting JussClient.");
		makeConf();
		MultiThreadedHttpConnectionManager connectionManager =
			new MultiThreadedHttpConnectionManager();
		client = new HttpClient(connectionManager);
		HttpConnectionManagerParams params = new HttpConnectionManagerParams();
		params.setDefaultMaxConnectionsPerHost(30);
		params.setMaxTotalConnections(30);
		connectionManager.setParams(params);
		client.getParams().setParameter(HttpMethodParams.RETRY_HANDLER,new DefaultHttpMethodRetryHandler(3, false));
		transferMan = new TransferManager(conf.getIntValue("maxTransfers").intValue(), false);
		connections = new HashMap();
	}
	
	/**
	 * Meetod mis loeb sisse konfiguratsiooni ja loob selle kui vaja.
	 */
	private static void makeConf() {
		conf = new CConfig("client.conf.xml");
		if(!conf.exists()) {
			conf.setValue("maxTransfers", 2);
		}
		conf.load();		
	}
	
	/**
	 * @return Kliendi konfiguratsioon.
	 */
	public static CConfig getConf() {
		return conf;
	}
	
	/**
	 * @return Http �henduste loomiseks kasutatav klient.
	 */
	public static HttpClient getClient() {
		return client;
	}
	
	/**
	 * @return Transpordi (upload/download) haldur.
	 */
	public static TransferManager getTransferManager() {
		return transferMan;
	}
	
	/**
	 * Lisab faili allalaadimise j�rjekorda.
	 * @param id Faili ID andmebaasis, mida t�mmata.
	 * @param connection �henduse lokaalne asukoht.
	 */
	public static void downloadFile(Long id, String connection) {
		transferMan.add(new CDownload(id, getConnection(connection)));
	}
	
	/**
	 * Lisab faili �leslaadimise j�rjekorda.
	 * @param file Faili asukoht lokaalses failis�steemis.
	 * @param id Kataloogi ID mille alla soovitakse uus fail panna.
	 * @param metadata Uue faili metadata.
	 * @param type FileManager.NEW_VERSION v�i FileManager.OVERWRITE
	 * @param connection �henduse lokaalne asukoht.
	 */
	public static void uploadFile(String file, Long id, CMetadata metadata, String type, String connection) {
		transferMan.add(new CUpload(file, id, metadata, type, getConnection(connection)));
	}
	
	/**
	 * Tagastab �henduse lokaalsele asukohale vastava CConnectioni. �hendus peab
	 * juba eelnevalt loodud olema.
	 * @param connection �henduse lokaalne asukoht.
	 * @return Vastav CConnection v�i <B>null</B> kui �hendust ei eksisteeri kliendis.
	 */
	public static CConnection getConnection(String connection) {
		return (CConnection)connections.get(connection);
	}
	
	/**
	 * Lisab �henduse kliendi Map'i.
	 * @param connection �hendus mida lisada.
	 * @return Lisatud �henduse nimi.
	 */
	private static String addConnection(CConnection connection) {
		connections.put(connection.getRoot(), connection);
		return connection.getName();
	}
	
	/**
	 * Sulgeb �henduse ja eemaldab selle kliendist.
	 * @param path �henduse lokaalne asukoht.
	 */
	public static void closeConnection(String path) {
		CConnection con = getConnection(path);
		con.close();
		connections.remove(path);
	}
	
	/**
	 * Avab ette antud �henduse. �hendus ei pea kliendis eksisteerima.
	 * @param path �henduse lokaalne asukoht. 
	 * @return �henduse nimi kui avamine �nnestus <B>null</B> vastasel juhul.
	 */
	public static String openConnection(String path) {
		path = FileManager.fixSlashes(path);
		if(connections.containsKey(path))
			return null;
		log.debug("Opening connection: " + path);
		CStatus.getInstance().setStatus("Opening connection: " + path);
		CConnection connection = new CConnection(path);
		if(connection.isValid()) {
			CStatus.getInstance().setStatus("");
			return addConnection(connection);
		} else {
			connection.close();
			log.warn("Connection not valid!");
			CStatus.getInstance().setStatus("Connection not valid!");
		}
		return null;
	}
	
	/**
	 * Loob uue �henduse ette antud parameetritega.
	 * @param name �henduse nimi.
	 * @param root �henduse asukoht lokaalses failis�steemis (kataloog peab juba eksisteerima).
	 * @param server Serveri URL millega �hendus luua.
	 * @return �henduse nimi kui loomine �nnestus <B>null</B> vastasel juhul.
	 */
	public static String newConnection(String name, String root, String server) {
		// tee juurikasse conf file ja siis
		root = FileManager.fixSlashes(root);
		if(server.charAt(server.length()-1) != '/')
			server += "/";
		String conf = root + "/connection.xml";
		CConfig c = new CConfig(conf);
		c.setValue("root", root);
		c.setValue("server", server);
		c.setValue("name", name);
		c.save();
		log.debug("Creating a new connection: " + name + " at:" + root + " with the server: " + server);
		return openConnection(root);
	}
	
	/**
	 * Sulgeb kliendi ja k�ik �hendused.
	 */
	public static void shutdown() {
		log.info("Shutting down JussClient");
		Iterator i = connections.keySet().iterator();
		while(i.hasNext()) {
			((CConnection)(connections.get(i.next()))).close();
		}
	}
}
