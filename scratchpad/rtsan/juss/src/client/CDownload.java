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

import java.io.*;
import shared.*;

import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.methods.PostMethod;

/**
 * @author Henri Kuuste
 *
 */
public class CDownload extends CTransfer {
	
	/**
	 * Loob uue download'i.
	 * Faili nimi võetakse andmebaasist antud ID järgi.
	 * 
	 * @param id Faili ID, mida tõmmata.
	 * @param connection Ühendus mida tõmbamiseks kasutada.
	 */
	public CDownload(Long id, CConnection connection) {
		super(id, connection);
		JussFile jf = connection.getFile(id);
		setSize(jf.getSize().longValue());
		setFileName(connection.getDBManager().getStringPath(jf));
	}
	
	public void run() {
		super.run();
		// ======== TODO: ALUMINE RIDA MAHA VÕTTA ========
		p = new PostMethod(con.getServer() + "Download");
		p.addParameter("id", id.toString());
		JussFile jf = con.getFile(id);
		if(jf == null) {
			setError("No database entry for the requested id: " + id);
			return;
		}
		log.info("Downloading file \"" + jf.getName() + "\"");
		try {
			int statusCode = JussClient.getClient().executeMethod(p);
			if(statusCode != HttpStatus.SC_OK) {
				setError("Server error: " + p.getStatusLine());
				fail();
				return;
			}
			setSize(p.getResponseContentLength());
			InputStream in = p.getResponseBodyAsStream();
			BufferedInputStream rd = new BufferedInputStream(in);
			try {
				con.createMissingAncestorFolders(id);
				FileManager.saveToFile(rd, new File(con.getRoot() + con.getDBManager().getStringPath(id)), this);
				con.setNewFileExists(jf);
				con.setModTime(jf);
				// ======== TODO: ALUMINE RIDA MAHA VÕTTA ========
				log.info("Download of \"" + jf.getName() + "\" DONE!");
				succeed();
			} catch(IOException e) {
				setError("Failed to write to disk: " + e.getMessage());
				fail();
				return;
			}
			
		} catch(IOException e) {
			setError("Failed to get response from the server: " + e.getMessage());
			fail();
		} finally {
			p.releaseConnection();
		}
	}
}
