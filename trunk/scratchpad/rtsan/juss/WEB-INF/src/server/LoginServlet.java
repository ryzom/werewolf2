/*
 * LoginServlet.java
 *
 * Created on April 19, 2005, 7:40 AM
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
 * Servlet serverisse sisse logimiseks. Loob kasutajale sessiooni.
 * Kui sisendina on antud parameetrid "login", "user" ja "pass" siis toimub uue sessiooni loomine.
 * Kui need parameetrid puuduvad (eelkõige "login") siis servlet lihtsalt kontrollib kas on juba sisse logitud.<BR/>
 * Tagastab:<BR/>
 * <B>SC_OK</B> kui login õnnestus/on olemas.<BR/>
 * <B>SC_UNAUTHORIZED</B> kui login ebaõnnestus/ei ole olemas.<BR/>
 * 
 * @author Henri Kuuste
 */
public class LoginServlet extends HttpServlet {
	
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
		if(request.getParameter("login") != null) {
			String user = request.getParameter("user");
			if(user == null || user.length() == 0) user = "Anonymous";
			String pass = request.getParameter("pass");
			if(pass == null) pass = "";
			if(PermissionManager.authenticate(user, pass)) {
				HttpSession session = request.getSession(true);
				session.setAttribute("user", user);
				session.setAttribute("pass", pass);
				response.setStatus(HttpServletResponse.SC_OK);
			} else {
				response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			}
		} else {
			HttpSession session = request.getSession(false);
			if(PermissionManager.authenticate(session)) {
				response.setStatus(HttpServletResponse.SC_OK);
			} else {
				response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			}
		}
	}	
}