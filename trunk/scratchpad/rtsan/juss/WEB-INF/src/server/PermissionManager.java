/*
 * PermissionManager.java
 *
 * Created on April 19, 2005, 7:27 AM
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

import javax.servlet.http.*;

/**
 * Õiguste haldur.
 * 
 * @author Henri Kuuste
 */
public class PermissionManager {
	
	/**
	 * Kontrollib kas login sobib või mitte.
	 * @param user Kasutaja.
	 * @param pass Parool.
	 * @return <B>true</B> kui login on sobilik <B>false</B> vastasel juhul.
	 */
	public static boolean authenticate(String user, String pass) {
		if(user.equals("thief"))
			return false;
		return true;
	}
	
	/**
	 * Kontrollib kas login sobib või mitte.
	 * @param session HTTP sessioon kus peaksid sisalduma muutujad "user" ja "pass".
	 * @return <B>true</B> kui login on sobilik <B>false</B> vastasel juhul.
	 */
	public static boolean authenticate(HttpSession session) {
			if(session == null)
				return false;
			String user = (String)session.getAttribute("user");
			String pass = (String)session.getAttribute("pass");
			if(pass == null || user == null) {
				return false;
			}
			if(authenticate(user, pass))
				return true;
			return false;
	}
	
	/**
	 * Kontrollib kas kasutajal on õigusi antud failiga sooritada antud operatsioone. 
	 * @param user Kasutaja.
	 * @param id Faili ID.
	 * @param accessType Operatsiooni tüüp. Suvaline kombinatsioon järgmistest asjadest:<BR>
	 * FileManager.F_READ, FileManager.F_WRITE, FileManager.F_CREATE ja FileManager.F_DELETE.<BR/>
	 * Kuna nad on string tüüpi tuleks nad kokku liita "+" abil.
	 * @return <B>true</B> kui õigused seda operatsiooni läbi viia eksisteerivad <B>false</B> vastasel juhul.
	 */
	public static boolean isFileAccessible(String user, Long id, String accessType) {
		if(user == null || user.equals("bob"))
			return false;
		return true;
	}
}
