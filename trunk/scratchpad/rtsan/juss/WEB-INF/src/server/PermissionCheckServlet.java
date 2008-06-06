/*
 * PermissionCheckServlet.java
 *
 * Created on April 20, 2005, 7:47 AM
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

/**
 * Servlet õiguste kontrollimiseks.
 * Sisendina on parameetrid "id" (faili ID) ja "accessType" (mis sorti õigusi kontrollida).<BR/>
 * Tagastab:<BR/>
 * <B>SC_OK</B> kui õigused on olemas.<BR/>
 * <B>SC_BAD_REQUEST</B> kui üks nõutud parameetritest puudus.<BR/>
 * <B>SC_UNAUTHORIZED</B> kui õigusi pole<BR/>
 * 
 * @author Henri Kuuste
 * @see server.PermissionManager#isFileAccessible(String, Long, String)
 */
public class PermissionCheckServlet extends HttpServlet {
	
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
		Long id = Long.valueOf(request.getParameter("id"));
		if(id == null) {
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		String accessType = request.getParameter("accessType");
		if(accessType == null || accessType.length() == 0) {
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		HttpSession session = request.getSession(false);
		if(!PermissionManager.authenticate(session) ) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;
		}
		String user = (String)session.getAttribute("user");
		if(!PermissionManager.isFileAccessible(user, id, accessType)) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;
		}
		response.setStatus(HttpServletResponse.SC_OK);
	}
}