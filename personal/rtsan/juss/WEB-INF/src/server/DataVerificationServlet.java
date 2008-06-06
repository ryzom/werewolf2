/*
 * DataVerificationServlet.java
 *
 * Created on April 20, 2005, 8:12 AM
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
 * Servlet mis kontrollib kas andmed serveri andmebaasis ja saadetud
 * andmed on sünkroonis.
 * Sisendiks on nõutud parameeter "id" mis sisaldab faili ID'd mida kontrollida.
 * Tagastab:<BR/>
 * <B>SC_OK</B> kui kõik on korras.<BR/>
 * <B>SC_BAD_REQUEST</B> kui üks vajalikest parameetritest oli puudu.<BR/>
 * <B>SC_NOT_ACCEPTABLE</B> kui andmed ei ole sünkroonis.<BR/>
 * @author Henri Kuuste
 */
public class DataVerificationServlet extends HttpServlet {
	
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
		String type = request.getParameter("type");
		Long modTime = Long.valueOf(request.getParameter("modtime"));
		if(id == null || type == null || modTime == null) {
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		if(!JussServer.getDBManager().verifyModTime(id, modTime, type)) {
			response.setStatus(HttpServletResponse.SC_NOT_ACCEPTABLE);
			return;
		}
		response.setStatus(HttpServletResponse.SC_OK);
	}
	
}