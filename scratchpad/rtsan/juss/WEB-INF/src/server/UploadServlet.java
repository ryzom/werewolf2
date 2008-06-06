/*
 * UploadServlet.java
 *
 * Created on April 20, 2005, 7:37 AM
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

import java.io.*;

import javax.servlet.*;
import javax.servlet.http.*;

import com.oreilly.servlet.multipart.*;
import shared.*;
/**
 * Servlet faili uploadimiseks.
 * Sisendina nõutud kataloogi ID, uploadi tüüp (FileManager.OVERWRITE või FileManager.NEW_VERSION), metadata ja fail ise.<BR/>
 * Tagastab:<BR/>
 * <B>SC_OK</B> ja uue faili ID headeris "newId" kui kõik õnnestus.<BR/>
 * <B>SC_BAD_REQUEST</B> kui ette antud argumendid olid vigased.<BR/>
 * <B>SC_UNAUTHORIZED</B> kui pole õigusi uploadida.<BR/>
 * <B>SC_CONFLICT</B> kui kataloog ei eksisteeri või kui fail on lukus.<BR/>
 * 
 * @author Henri Kuuste
 */
public class UploadServlet extends HttpServlet {
	
	/** Initializes the servlet.
	 */
	public void init(ServletConfig config) throws ServletException {
		super.init(config);
		
	}
	
	/** Handles the HTTP <code>POST</code> method.
	 * @param request servlet request
	 * @param response servlet response
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response)
	throws ServletException, IOException {
		HttpSession session = request.getSession(false);
		if(!PermissionManager.authenticate(session) ) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;
		}
		String user = (String)session.getAttribute("user");
		MultipartParser parser = new MultipartParser(request, JussServer.getMaxUploadSize());
		CMetadata metadata = null;
		Long id = null;
		FilePart fpart = null;
		String type = null; 
		try {
			ParamPart ppart;
			ppart = (ParamPart)(parser.readNextPart());
			id = Long.valueOf(ppart.getStringValue());
			ppart = (ParamPart)(parser.readNextPart());
			type = ppart.getStringValue();
			FilePart mpart = (FilePart)(parser.readNextPart());
			ObjectInputStream os = new ObjectInputStream(mpart.getInputStream());
			metadata = (CMetadata)os.readObject();
			fpart = (FilePart)(parser.readNextPart());
		} catch(Exception e) {
			System.err.println("Could not read upload parts!");
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		if(metadata == null || id == null || fpart == null || type == null) {
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		if(!PermissionManager.isFileAccessible(user, id, FileManager.F_CREATE)) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;
		}
		Long existingId = JussServer.getDBManager().getId(id, fpart.getFileName());
		if(existingId != null && !PermissionManager.isFileAccessible(user, existingId, FileManager.F_WRITE)) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;			
		}
		JussFile dir = JussServer.getDBManager().getFile(id);
		if(dir == null || !dir.isFolder()) {
			response.setStatus(HttpServletResponse.SC_CONFLICT);
			return;
		}
		if(type.equals(FileManager.OVERWRITE) && existingId != null) {
			if(JussServer.isLocked(existingId)) {
				response.setStatus(HttpServletResponse.SC_CONFLICT);
				return;
			}
			JussServer.lock(existingId);
		}
		File outfile = new File(JussServer.getRoot() + JussServer.getDBManager().getLocalStringPath(id, fpart.getFileName(), type)); 
		Long size = new Long(fpart.writeTo(outfile));
		// Nüüd järgmisena lisaks siis faili kirje ka andmebaasi.
		Long newId = JussServer.getDBManager().addFile(id, fpart.getFileName(), metadata, type, size);
		if(type.equals(FileManager.OVERWRITE) && existingId != null)
			JussServer.unlock(existingId);
		response.setStatus(HttpServletResponse.SC_OK);
		response.addHeader("newId", newId.toString());
		response.flushBuffer();
	}
}