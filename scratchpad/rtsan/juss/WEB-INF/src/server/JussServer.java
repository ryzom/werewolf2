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

import shared.*;
import java.util.*;

/**
 * JUSS'i serveri globaalne klass.
 * 
 * @author Henri Kuuste
 */
public class JussServer {
	/**
	 * Serveri konfiguratsioon mis sisaldab muuhulgas ka serveri puu asukohta failisüsteemis.
	 */
	private static final CConfig conf = new CConfig("server.conf.xml");
	/**
	 * Serveri andmebaasi frontend.
	 */
	private static final ServerDBManager dbManager = new ServerDBManager();

	/**
	 * Ajutine lahendus samaaegsetele uploadidele ja downloadidele
	 * Uus süsteem välja mõeldud - teeme hiljem
	 * TODO: UUS SÜSTEEM :)
	 */
	private static final HashSet locks = new HashSet();
	
	static {
		conf.load();
	}
	
	/**
	 * Tagastab serveri puu asukoha failisüsteemis. 
	 * @return Serveri puu asukoht failisüsteemis.
	 */
	public static String getRoot() {
		return conf.getStringValue("root");
	}
	
	/**
	 * Tagastab maksimaalse uploadi suuruse baitides.
	 * @return Maksimaalne uploadi suurus baitides.  
	 */
	public static int getMaxUploadSize() {
		return conf.getIntValue("MaxUploadSize").intValue();
	}
	
	/**
	 * Tagastab serveri andmebaasi frontend'i.
	 * @return Serveri andmebaasi frontend.
	 */
	public static ServerDBManager getDBManager() {
		return dbManager;
	}
	
	/**
	 * Lukustab faili nii et seda ei saa enam tõmmata ega ka üle kirjutada.
	 * @param id Lukustatava faili ID.
	 */
	public static void lock(Long id) {
		locks.add(id);
	}
	
	/**
	 * Võtab faililt luku pealt maha nii et seda saaks jälle downloadida ja üle kirjutada.
	 * @param id Faili ID millelt lukk maha võtta.
	 */
	public static void unlock(Long id) {
		locks.remove(id);
	}
	
	/**
	 * Kontrollib kas fail on lukus.
	 * @param id Faili ID mida kontrollida.
	 * @return <B>true</B> kui fail on lukus <B>false</B> vastasel juhul.
	 */
	public static boolean isLocked(Long id) {
		return locks.contains(id);
	}
}
