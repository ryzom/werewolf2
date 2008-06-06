/*
 * Created on Apr 20, 2005
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

import java.io.*;

import shared.*;

import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.methods.multipart.MultipartRequestEntity;
import org.apache.commons.httpclient.methods.multipart.Part;
import org.apache.commons.httpclient.methods.multipart.StringPart;

/**
 * @author Henri Kuuste
 *
 */
public class CUpload extends CTransfer {

	/**
	 * Upload'itava faili metadata
	 * 
	 * @uml.property name="meta"
	 * @uml.associationEnd multiplicity="(1 1)"
	 */
	private CMetadata meta;

	/**
	 * Upload'itava faili asukoht lokaalses failisüsteemis.
	 */
	private String filename;
	/**
	 * Upload'i tüüp.
	 */
	private String type = FileManager.OVERWRITE;

	/**
	 * Loob uue upload'i.
	 * @param file Fail lokaalses failisüsteemis.
	 * @param id Kataloogi ID mille alla uus fail panna.
	 * @param metadata Uue faili metadata.
	 * @param type FileManager.OVERWRITE või FileManager.NEW_VERSION
	 * @param connection Ühendus mida upload'iks kasutada.
	 */
	public CUpload(String file, Long id, CMetadata metadata, String type, CConnection connection) {
		super(id, connection);
		meta = metadata;
		filename = file;
		this.type = type;
		File f = new File(file);
		JussFile jf = connection.getFile(id);
		setFileName(connection.getDBManager().getStringPath(jf) + "/" + f.getName());
		setSize(f.length());
	}
	
	public void run() {
		super.run();
		// ======== TODO: ALUMINE RIDA MAHA VÕTTA ========
		log.info("Uploading file \"" + filename + "\"");
		if(filename == null || filename.length() == 0) {
			setError("Illegal filename (no filename given or length 0)");
			fail();
			return;
		}
		File f = new File(filename);
		if(!f.exists() || !f.isFile()) {
			setError("Given file does not exist");
			fail();
			return;
		}
//		 TODO: Välja mõelda mida sellisel juhul erroritega teha. Eraldi CStatus klass?
		if(!con.checkPermission(id, FileManager.F_CREATE)) {
			setError("No right to create the file in the given folder");
			fail();
			return;
		}
		Long existingId = con.getDBManager().getId(id, f.getName());
		if(existingId != null && !con.checkPermission(existingId, FileManager.F_WRITE)) {
			setError("File already exists and no right to overwrite");
			fail();
			return;
		}
		PostMethod p = new PostMethod(con.getServer() + "Upload");
		try {
			FilePart fp = new FilePart(f.getName(), f);
			fp.addProgressListener(this);
			Part[] parts = { new StringPart("parentId", id.toString()),
					new StringPart("type", type),
					new ObjectPart(meta),
					fp};
			p.setRequestEntity(new MultipartRequestEntity(parts, p.getParams()));
			int statusCode = JussClient.getClient().executeMethod(p);
			if(statusCode != HttpStatus.SC_OK) {
				setError("Server error: " + p.getStatusLine());
				fail();
				return;
			}
			// pane fail ka kohalikku rep-i
			Long oldId = con.getDBManager().getId(id, f.getName());
			Long newId = Long.valueOf(p.getResponseHeader("newId").getValue());
			p.releaseConnection();
			JussFile jf = con.getFile(newId);
			con.getMetadata(newId);
			con.createMissingAncestorFolders(jf);
			FileManager.copyFile(f, new File(con.getRoot() + con.getDBManager().getStringPath(jf)));
			// ======== TODO: ALUMINE RIDA MAHA VÕTTA ========
			log.info("Upload of \"" + filename + "\" DONE!");
			if(oldId != null) {
				con.getFilesFromServer(con.getDBManager().getVersionIds(oldId));
			}
			con.setNewFileExists(jf);
			con.setModTime(jf);
			succeed();
		} catch(Exception e) {
			// Siin ilmneb mõnikord järgmine : teatatakse et "Could ... file failinimi : null"    
			// Tulemuseks on testimisel olnud see, et serverijuurde ja andmebaasi fail tegelikult 
			// tekkis, kuid kliendi juurikale ja andmebaasi seda ei pandud.
			// kas v6ime eeldada et kui e.getMessage.equals("null") läks kõik normaalselt tegelikult?
			// VASTUS: EI! Süütud exceptionid mis ilmnevad mujal programmis tuleb ilmenmiskohas
			// püüda või välistada, et see järgnev fail() alati asjakohane oleks.
			setError("Could not upload file to server: " + e.toString());
			fail();
		} finally {
			p.releaseConnection();
		}
	}

}
