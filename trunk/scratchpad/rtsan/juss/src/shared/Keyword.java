/*
 * Created on Apr 23, 2005
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

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

/**
 * klass esitamaks failide ja kaustade kohta k�ivaid v�tmes�nu JUSS s�steemis
 * @author Taimo Peelo
 */

public class Keyword implements Serializable {

	/**
	 * 
	 * @uml.property name="id" multiplicity="(0 1)"
	 */
	private Long id;

	/**
	 * 
	 * @uml.property name="keyword" multiplicity="(0 1)"
	 */
	String keyword;

	
	/** loob defineerimata sisuga Keywordi */
	public Keyword() {}
	
	/**
	 * Loob uue Keywordi defineeritud sisuga. Parameeters�nest eemaldatakse Keywordiks
	 * teisendamisel eelnev ja j�rgnev <i>whitespace</i> ning viiakse 
	 * see v�iket�helisele kujule. 
	 * @param keyword v�tmes�na
	 */
	public Keyword(String keyword) {
		setKeyword(keyword);
	}

	/**
	 * Tagastab selle v�tmes�na ID andmebaasis
	 * @return Returns the id.
	 * 
	 * @uml.property name="id"
	 */
	public Long getId() {
		return id;
	}

	/**
	 * Seab selle v�tmes�na ID andmebaasis
	 * @param id The id to set.
	 * 
	 * @uml.property name="id"
	 */
	private void setId(Long id) {
		this.id = id;
	}

	/**
	 * Tagastab selle v�tmes�na s�nena.
	 * @return s�ne selles Keyword'is hoitava v�tmes�naga.
	 * 
	 * @uml.property name="keyword"
	 */
	public String getKeyword() {
		return keyword;
	}

	/**
	 * Seab hoitava v�tmes�na.
	 * @param keyword seatav v�tmes�na.
	 * 
	 * @uml.property name="keyword"
	 */
	public void setKeyword(String keyword) {
		if (keyword != null)
			this.keyword = keyword.trim().toLowerCase();
		else
			this.keyword = null;
	}

	
	/** vajalik hibernate'le ja Set'ile v�tmes�nada v�rdsuse kontrollimiseks
	 * tagastab "true" siis ja ainult siis kui o on Keyword ja tema sisu v�rdub
	 * selle Keywordi omaga   
	 */
	public boolean equals(Object o) {
		if (this == o) 
			return true;
		if (!(o instanceof Keyword))
			return false;
		
		Keyword k = (Keyword) o;
		
		return keyword.equals(k.keyword);
	}
	
	/**
	 * Leiab selle v�tmes�na hashCode()
	 * @return selle v�tmes�na hashCode()
	 */
	public int hashCode() {
		return keyword.hashCode();
	}
	
	private void readObject(ObjectInputStream aInputStream)
			throws ClassNotFoundException, IOException {
		aInputStream.defaultReadObject();
	}

	private void writeObject(ObjectOutputStream aOutputStream)
			throws IOException {
		aOutputStream.defaultWriteObject();
	}

	public String toString() {
		return "[ " + id + "; " + keyword + " ]";
	}
	
}
