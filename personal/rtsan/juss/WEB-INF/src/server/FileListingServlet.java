/*
 * FileListingServlet.java
 *
 * Created on April 19, 2005, 5:05 PM
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

import shared.*;

import java.io.*;
import java.util.*;

import javax.servlet.*;
import javax.servlet.http.*;

import com.oreilly.servlet.multipart.FilePart;
import com.oreilly.servlet.multipart.MultipartParser;

/**
 * Servlet mille kaudu on võimalik serverist saada failide infot.
 * Sisendiks on failide id'dest koosnev java.util.List.<BR/>
 * Tagastab:<BR/>
 * <B>SC_OK</B> ja JussFile'dest koosnev java.util.List kui kõik õnnestus.<BR/>
 * <B>SC_UNAUTHORIZED</B> kui ei ole õigusi listi saada.<BR/>
 * <B>SC_BAD_REQUEST</B> kui sisendina nõutud list puudus.<BR/>
 * 
 * @author Henri Kuuste
 */
public class FileListingServlet extends HttpServlet {
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
		MultipartParser parser = new MultipartParser(request, JussServer.getMaxUploadSize());
		List in = null;
		try {
			FilePart mpart = (FilePart)(parser.readNextPart());
			ObjectInputStream os = new ObjectInputStream(mpart.getInputStream());
			in = (List)os.readObject();
		} catch(Exception e) {
			System.err.println("Could not read upload parts!");
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		if(in == null) {
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		String user = (String)session.getAttribute("user");
		Iterator i = in.iterator();
		ArrayList ids = new ArrayList();
		while(i.hasNext()) {
			Long id = (Long)i.next();
			if(PermissionManager.isFileAccessible(user, id, FileManager.F_READ)) {
				ids.add(id);
			}
		}
		List l = JussServer.getDBManager().getFileListing(ids);
		ObjectOutputStream os = new ObjectOutputStream(response.getOutputStream());
		try {
			os.writeObject(l);
		} catch(IOException e) {
			System.err.println("Failed to send listing!");
			e.printStackTrace();
		}
	}
	
}