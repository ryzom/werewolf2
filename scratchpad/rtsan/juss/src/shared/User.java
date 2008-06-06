/*
 * Created on Apr 20, 2005
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

package shared;

import java.security.*;
import java.util.Arrays;

/**
 * JUSSi kasutajat esitav klass
 * @author Taimo Peelo
 */

public class User {
	
	/** algoritm mida kasutatakse kasutaja parooli räsi arvutamiseks MessageDigest abil */
	public static final String HashAlgorithm = "SHA";

	/**
	 * Sisene kasutaja ID
	 * 
	 * @uml.property name="id" multiplicity="(0 1)"
	 */
	private Long id = null;

	/**
	 * JUSSi kasutajanimi, 2-16 märki
	 * 
	 * @uml.property name="userName" multiplicity="(0 1)"
	 */
	private String userName;

	/**
	 * kasutaja eesnimi
	 * 
	 * @uml.property name="firstName" multiplicity="(0 1)"
	 */
	private String firstName;

	/**
	 * kasutaja perekonnanimi
	 * 
	 * @uml.property name="lastName" multiplicity="(0 1)"
	 */
	private String lastName;

	/**
	 * kasutaja e-maili aadress
	 * 
	 * @uml.property name="email" multiplicity="(0 1)"
	 */
	private String email;

	/**
	 * kasutaja parooli räsi
	 * 
	 * @uml.property name="passwordHash" multiplicity="(0 1)"
	 */
	private byte[] passwordHash;

	

	User () {}
		
	User (String userName, String firstName, String lastName, String email, String password) {
		this.userName = userName;
		this.firstName = firstName;
		this.lastName = lastName;
		this.email = email;
		this.setPassword(password);
	}

	/**
	 * @return Returns the id.
	 * 
	 * @uml.property name="id"
	 */
	public Long getId() {
		return id;
	}

	/**
	 * @param id The id to set.
	 * 
	 * @uml.property name="id"
	 */
	private void setId(Long id) {
		this.id = id;
	}

	/**
	 * @return Returns the email.
	 * 
	 * @uml.property name="email"
	 */
	public String getEmail() {
		return email;
	}

	/**
	 * @param email The email to set.
	 * TODO : basic sanity check of email address ???
	 * 
	 * @uml.property name="email"
	 */
	public void setEmail(String email) {
		this.email = email;
	}

	/**
	 * @return Returns the firstName.
	 * 
	 * @uml.property name="firstName"
	 */
	public String getFirstName() {
		return firstName;
	}

	/**
	 * @param firstName The firstName to set.
	 * 
	 * @uml.property name="firstName"
	 */
	public void setFirstName(String firstName) {
		this.firstName = firstName;
	}

	/**
	 * @return Returns the lastName.
	 * 
	 * @uml.property name="lastName"
	 */
	public String getLastName() {
		return lastName;
	}

	/**
	 * @param lastName The lastName to set.
	 * 
	 * @uml.property name="lastName"
	 */
	public void setLastName(String lastName) {
		this.lastName = lastName;
	}

	/**
	 * @return Returns the passwordHash.
	 * 
	 * @uml.property name="passwordHash"
	 */
	public byte[] getPasswordHash() {
		return passwordHash;
	}

	/**
	 * @param passwordHash The passwordHash to set.
	 * 
	 * @uml.property name="passwordHash"
	 */
	private void setPasswordHash(byte[] passwordHash) {
		this.passwordHash = passwordHash;
	}

	
	
	/** Seab kasutaja parooli. 
	 * @param password Seatav parool
	*/	
	public void setPassword (String password) {
		// NoSuchAlgorithmException, NoSuchProviderException ei tohiks kunagi ilmneda
		try {
			this.passwordHash = MessageDigest.getInstance(HashAlgorithm).digest(password.getBytes());
		} catch (Exception e) {}
	}

	/**
	 * @return Returns the userName.
	 * 
	 * @uml.property name="userName"
	 */
	public String getUserName() {
		return userName;
	}

	
	/**
	 * Seab kasutaja kasutajanime. Lubatud kasutajanime pikkus on 2-16 märki, 
	 * lubatavad märgid on inglise tähestiku tähed (konverteeritakse väiketähtedeks), 
	 * numbrid ja alakriipsud (alates kasutajanime teisest märgist).
	 * @param userName Seatav kasutajanimi 
	 * @return seatud kasutajanimi - null kui kasutajanimi ei vastanud reeglitele
	 */
	private String setUserName(String userName) {
		String s = userName.trim().toLowerCase();
		if (s.matches("[a-z]{1}[a-z0-9_]{1-15}"))
			return (this.userName = s);
		return null;
	}
	
	/** Autendib kasutaja, võrreldes kasutaja parooli tegelikku räsi funktsiooni argumenträsiga.
	 * @param pwHash Eeldatava parooli HashAlgorithm'iga genereeritud räsi
	 * @return kas autentimine õnnestus või mitte
	 */
	public boolean authenticate(byte[] pwHash) {
		return Arrays.equals(pwHash, passwordHash);
	}
}
