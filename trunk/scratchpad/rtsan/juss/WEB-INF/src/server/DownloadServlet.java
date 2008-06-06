/*
 * DownloadServlet.java
 *
 * Created on April 19, 2005, 2:32 PM
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

import shared.*;

import javax.servlet.*;
import javax.servlet.http.*;

import com.oreilly.servlet.*;

/**
 * Servlet mis tegeleb downloadidega.
 * Sisendina võtab parameetri "id".<BR/>
 * Tagastab:<BR/>
 * <B>SC_OK</B>, content length headeri ja faili kui kõik on korras.<BR/>
 * <B>SC_BAD_REQUEST</B> kui parameeter "id" oli puudu.<BR/>
 * <B>SC_CONFLICT</B> kui fail on lukus ja seda ei saa avada või kui fail ei eksisteeri.<BR/>
 * <B>SC_UNAUTHORIZED</B> kui pole õigusi faili tõmmata.<BR/>
 * 
 * @author Henri Kuuste
 */
public class DownloadServlet extends HttpServlet {
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
//		int version = Integer.parseInt(request.getParameter("version"));
		if(id == null) {
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		if(JussServer.isLocked(id)) {
			response.setStatus(HttpServletResponse.SC_CONFLICT);
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
		String path = JussServer.getRoot() + JussServer.getDBManager().getLocalStringPath(id);
		File f = new File(path);
		if(!f.exists() || !f.isFile()) {
			response.setStatus(HttpServletResponse.SC_CONFLICT);
			return;
		}
		response.setCharacterEncoding("");
		response.setContentType("");
		response.setStatus(HttpServletResponse.SC_OK);
		response.setContentLength((int)f.length());
		ServletOutputStream out = response.getOutputStream();
	    ServletUtils.returnFile(path, out);
		out.close();
	}
}