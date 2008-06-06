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

package server;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.*;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import com.oreilly.servlet.multipart.FilePart;
import com.oreilly.servlet.multipart.MultipartParser;

import shared.*;

/**
 * Servlet metadata saamiseks serverist.
 * Sisendina on nõutud java.util.List failide ID'dest mille kohta metadatat soovitakse.<BR/>
 * Tagastab:<BR/>
 * <B>SC_OK</B> ja java.util.List kus elementideks CMetadata kui kõik õnnestus.<BR/>
 * <B>SC_BAD_REQUEST</B> kui ID'de list oli puudu.<BR/>
 * <B>SC_UNAUTHORIZED</B> kui pole õigusi metadata listi saada.<BR/>
 * <B>SC_CONFLICT</B> kui metadata ei eksisteeri.<BR/>
 * 
 * @author Henri Kuuste
 */
public class MetadataServlet extends HttpServlet {
	/** Initializes the servlet.
	 */
	public void init(ServletConfig config) throws ServletException {
		super.init(config);
		
	}
	
	/** Destroys the servlet.
	 */
	public void destroy() {
		
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
			System.err.println("Could not read metadata id parts!");
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
		List meta = JussServer.getDBManager().getMetadataList(ids);
		if(meta == null) {
			response.setStatus(HttpServletResponse.SC_CONFLICT);
			return;
		}
		ObjectOutputStream os = new ObjectOutputStream(response.getOutputStream());
		try {
			os.writeObject(meta);
		} catch(IOException e) {
			System.err.println("Failed to send metadata!");
			e.printStackTrace();
		}
		response.setStatus(HttpServletResponse.SC_OK);
	}
	
}
