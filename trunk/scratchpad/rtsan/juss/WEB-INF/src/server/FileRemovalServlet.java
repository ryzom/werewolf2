/*
 * Created on May 1, 2005
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

import shared.*;

/**
 * Servlet failide eemaldamiseks süsteemist.
 * Sisendina on nõutud eemaldatava faili ID parameetrina "id".<BR/>
 * Tagastab:<BR/>
 * <B>SC_OK</B> kui eemaldamine õnnestus.<BR/>
 * <B>SC_BAD_REQUEST</B> kui parameeter "id" puudus.<BR/>
 * <B>SC_UNAUTHORIZED</B> kui kustutamise õigused puuduvad.<BR/>
 * <B>SC_CONFLICT</B> kui kustutamine ebaõnnestus.<BR/>
 * 
 * @author Henri Kuuste
 */
public class FileRemovalServlet extends HttpServlet {
	/**
	 * Initializes the servlet.
	 */
	public void init(ServletConfig config) throws ServletException {
		super.init(config);

	}

	/**
	 * Handles the HTTP <code>POST</code> method.
	 * 
	 * @param request
	 *            servlet request
	 * @param response
	 *            servlet response
	 */
	protected void doPost(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		Long id = Long.valueOf(request.getParameter("id"));
		Long type = Long.valueOf(request.getParameter("type"));
		if (id == null || type == null) {
			response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			return;
		}
		HttpSession session = request.getSession(false);
		if (!PermissionManager.authenticate(session)) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;
		}
		String user = (String) session.getAttribute("user");
		if (!PermissionManager.isFileAccessible(user, id, FileManager.F_DELETE)) {
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			return;
		}
		if((type.longValue() & FileManager.FILES) != 0) {
			if (!JussServer.getDBManager().removeFile(id)) {
				response.setStatus(HttpServletResponse.SC_CONFLICT);
				return;
			}
		} else if((type.longValue() & FileManager.FOLDERS) != 0) {
			if (!JussServer.getDBManager().removeEmptyFolderFS(id)) {
				response.setStatus(HttpServletResponse.SC_CONFLICT);
				return;
			}
			
		}
	}
}
