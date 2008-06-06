/*
 * Created on Apr 23, 2005
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

package server;

import java.io.File;
import java.io.IOException;
import java.io.ObjectInputStream;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import shared.*;

import com.oreilly.servlet.multipart.FilePart;
import com.oreilly.servlet.multipart.MultipartParser;
import com.oreilly.servlet.multipart.ParamPart;

/**
 * Servlet kataloogide loomiseks.
 * Sisendina on nõutud ülemkataloogi id, metadata ja uue kataloogi nimi.<BR/>
 * Tagastab:<BR/>
 * <B>SC_OK</B> ja headeri "newId" uue kataloogi id'ga kui kõik õnnestus.<BR/>
 * <B>SC_UNAUTHORIZED</B> kui pole õigusi kataloogi luua.<BR/>
 * <B>SC_BAD_REQUEST</B> kui ette antud parameetrid olid vigased.<BR/>
 * <B>SC_CONFLICT</B> kui ülemkataloogi ei eksisteeri.<BR/>
 * 
 * @author Henri Kuuste
 */
public class FolderCreationServlet extends HttpServlet {
	
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
		String name = null;;
		try {
			ParamPart ppart;
			ppart = (ParamPart)(parser.readNextPart());
			id = Long.valueOf(ppart.getStringValue());
			FilePart mpart = (FilePart)(parser.readNextPart());
			ObjectInputStream os = new ObjectInputStream(mpart.getInputStream());
			metadata = (CMetadata)os.readObject();
			ppart = (ParamPart)(parser.readNextPart());
			name = ppart.getStringValue();
		} catch(Exception e) {
			System.err.println("Could not read folder creation parts!");
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		if(!PermissionManager.isFileAccessible(user, id, FileManager.F_CREATE)) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;
		}
		Long existingId = JussServer.getDBManager().getId(id, name);
		if(existingId != null) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;			
		}
		JussFile dir = JussServer.getDBManager().getFile(id);
		if(dir == null || !dir.isFolder()) {
			response.setStatus(HttpServletResponse.SC_CONFLICT);
			return;
		}
		File f = new File(JussServer.getRoot() + JussServer.getDBManager().getStringPath(id) + "/" + name);
		f.mkdir();
		Long newId = JussServer.getDBManager().addFolder(id, name, metadata);
		response.setStatus(HttpServletResponse.SC_OK);
		response.addHeader("newId", newId.toString());
		response.flushBuffer();
	}
}
