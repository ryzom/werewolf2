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

import java.io.IOException;
import java.io.ObjectOutputStream;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import java.util.List;

import shared.*;


/**
 * Servlet faili ID'de ja muutmisaegade saamiseks serverist.
 * Sisendina on nõutud parameeter "id" mis peaks sisaldama kataloogi ID'd mille all asuvate failide ja kataloogide id'sid soovitakse.<BR/>
 * Tagastab:<BR/>
 * <B>SC_OK</B> ja java.utli.List elementidega Object[] kus indeks 0 on Long id ja indeks 1 Date, kui kõik õnnestus.<BR/>
 * <B>SC_BAD_REQUEST</B> kui parameeter "id" puudus.<BR/>
 * <B>SC_UNAUTHORIZED</B> kui nimekirja saamiseks polnud õigusi.<BR/>
 * 
 * @author Henri Kuuste
 */
public class IdListingServlet extends HttpServlet {
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
		Long id = Long.valueOf(request.getParameter("id"));
		Long type = Long.valueOf(request.getParameter("type"));
		if(id == null || type == null) {
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		HttpSession session = request.getSession(false);
		if(!PermissionManager.authenticate(session) ) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;
		}
		String user = (String)session.getAttribute("user");
		if(!PermissionManager.isFileAccessible(user, id, FileManager.F_READ)) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;
		}
		List l = JussServer.getDBManager().getIdListing(id, type.longValue());
		ObjectOutputStream os = new ObjectOutputStream(response.getOutputStream());
		try {
			os.writeObject(l);
		} catch(IOException e) {
			System.err.println("Failed to send metadata!");
			e.printStackTrace();
		}
		response.setStatus(HttpServletResponse.SC_OK);
	}
}
