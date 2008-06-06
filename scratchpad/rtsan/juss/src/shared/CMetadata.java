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

package shared;

import java.io.*;
import java.util.*;

import shared.DBManager; // cut_precision Date'i jaoks

/**
 * klass mis esitab failidega seotud metadatat
 * @author Henri Kuuste, Taimo Peelo 
 */
public class CMetadata implements Serializable {

	/**
	 * see ID alati sama mis metadataga seostatud faili enda ID
	 * 
	 * @uml.property name="id" multiplicity="(0 1)"
	 */
	private Long id;

	/**
	 * lühike kirjeldus, millega antud faili näol tegemist on
	 * 
	 * @uml.property name="description" multiplicity="(0 1)"
	 */
	private String description;

	/**
	 * siia tohib pista ainult ERINEVAID Keyword tyypi objekte
	 * 
	 * @uml.property name="keywords"
	 * @uml.associationEnd elementType="shared.Keyword" multiplicity="(0 -1)"
	 */
	private Set keywords = null;

	/**
	 * faili looja kasutajanimi faili loomise hetkel
	 * 
	 * @uml.property name="creator" multiplicity="(0 1)"
	 */
	private String creator;

	/**
	 * hinne failile mille juurde objekt kuulub ... < 0 t2hendab hinde puudumist
	 * 
	 * @uml.property name="grade" multiplicity="(0 1)"
	 */
	private int grade = -1;

	/**
	 * metadata viimase muutmise aeg
	 * 
	 * @uml.property name="modTime" multiplicity="(0 1)"
	 */
	private Date modTime;


	public CMetadata() {}
	
	public CMetadata(Long id, String creator, String description) {
		this.id = id;
		this.creator = creator;
		this.description = description; 
	}
	
	public CMetadata(String creator, String description) {
		setCreator(creator);
		setDescription(description);
	}

	/**
	 * @return Returns the modTime.
	 * 
	 * @uml.property name="modTime"
	 */
	public Date getModTime() {
		return modTime;
	}

	/**
	 * @param mtime The modTime to set.
	 * 
	 * @uml.property name="modTime"
	 */
	public void setModTime(Date mtime) {
		if (mtime != null) {
			long time = DBManager.cut_precision(mtime.getTime());
			this.modTime = new Date(time);
		} else
			this.modTime = new Date(0);
	}

	/**
	 * @return Returns the grade.
	 * 
	 * @uml.property name="grade"
	 */
	public int getGrade() {
		return grade;
	}

	/**
	 * @param grade The grade to set.
	 * 
	 * @uml.property name="grade"
	 */
	public void setGrade(int grade) {
		this.grade = grade;
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
	public void setId(Long id) {
		this.id = id;
	}

	/**
	 * @return Returns the description.
	 * 
	 * @uml.property name="description"
	 */
	public String getDescription() {
		return description;
	}

	/**
	 * @param description
	 *            The description to set.
	 * 
	 * @uml.property name="description"
	 */
	public void setDescription(String description) {
		this.description = description;
	}

	/**
	 * @return Returns the keywords.
	 * 
	 * @uml.property name="keywords"
	 */
	public Set getKeywords() {
		return keywords;
	}

	/**
	 * @param keywords
	 *            The keywords to set.
	 * 
	 * @uml.property name="keywords"
	 */
	public void setKeywords(Set keywords) {
		this.keywords = keywords;
	}

	
	
	/**
	 * @param keywords String mis sisaldab ";" eraldatud võtmesõnu.
	 * kasutatakse IDE poolt võtmesõnade lihtsaks seadmiseks. 
	 */
	public void setKeywords(String keywords) {
		StringTokenizer tok = new StringTokenizer(keywords, ";");
		HashSet r = new HashSet();
		while(tok.hasMoreTokens())
			r.add(new Keyword(tok.nextToken().trim()));
		setKeywords(r);
	}

	/**
	 * @return Returns the owner.
	 * 
	 * @uml.property name="creator"
	 */
	public String getCreator() {
		return creator;
	}

	/**
	 * @param creator
	 *            The owner to set.
	 * 
	 * @uml.property name="creator"
	 */
	public void setCreator(String creator) {
		this.creator = creator;
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
		String result = "";
		result += "[ " + id + "; " + creator + "; " + description + "; " + grade + "; " + keywords + " ]";
		return result;
	}
}
