/*
 * Created on Apr 21, 2005
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

import java.io.Serializable;
import java.util.Date;
import java.util.StringTokenizer;
import shared.DBManager;

/**
 * JUSSi faile ja kaustu esitav klass.
 * @author Taimo Peelo, Henri Kuuste
 */

public class JussFile implements Serializable {
	/** kibibaidi suurus baitides */
	public static final long KB = 1024;
	/** mebibaidi suurus baitides */
	public static final long MB = 1024*KB;

	/**
	 * 
	 * @uml.property name="id" multiplicity="(0 1)"
	 */
	private Long id = null;

	/**
	 * ylemkataloog
	 * 
	 * @uml.property name="parent" multiplicity="(0 1)"
	 */
	private Long parent;

	/**
	 * kõikide ülemkaustade k.a parent ID-d. kole jah ;)
	 * 
	 * @uml.property name="ancestors" multiplicity="(0 1)"
	 */
	private String ancestors = null;

	/**
	 * p6hiversioon (viimane sellest failist) - 
	 * NULL kui fail on ise viimane versioon 
	 * v6i sellest failist pole veel versioone. 
	 * katalooge ei versioneerita
	 * 
	 * @uml.property name="mainversion" multiplicity="(0 1)"
	 */
	private Long mainversion = null;

	/**
	 * selle faili või kausta nimi
	 * 
	 * @uml.property name="name" multiplicity="(0 1)"
	 */
	private String name;

	/**
	 * millal selles kaustas või failis muutused toimusid
	 * 
	 * @uml.property name="modtime" multiplicity="(0 1)"
	 */
	private Date modtime = null;

	/**
	 * faili suurus
	 * 
	 * @uml.property name="size" multiplicity="(0 1)"
	 */
	private Long size = null;

	/**
	 * kas on tegemist kaustaga või mitte
	 * 
	 * @uml.property name="folder" multiplicity="(0 1)"
	 */
	private boolean folder;

	/**
	 * kas fail eksisteerib kõvakettal - eelkõige vajalik client'i jaoks
	 * 
	 * @uml.property name="exists" multiplicity="(0 1)"
	 */
	private boolean exists = false;

	/**
	 * 
	 * @uml.property name="version" multiplicity="(0 1)"
	 */
	private int version = 1;

	
	/** loob initsialiseerimata väljadega JussFile'i */
	public JussFile() {}
	
	/**
	 * Loob JussFile' koos mõnede initsialiseeritud põhiväljadega.
	 * @param parentID ülemkausta andmebaasi ID
	 * @param name uue JussFile nimi
	 * @param folder kas tegemist kaustaga või mitte
	 */
	public JussFile(Long parentID, String name, boolean folder) {
		this.parent = parentID;
		this.name = name;
		this.folder = folder;
		modtime = new Date();
	}
	
	/**
	 * Leiab selle <i>JussFile</i>' kõik ülemkaustad.
	 * @return massiiv selle JussFile kõikide ülemkaustade ID'dest 
	 * ID-dest.
	 */
	public Long[] getLongAncestors() {
		if (ancestors == null)
			return null;
		StringTokenizer st = new StringTokenizer(ancestors, "/");
		Long [] result = new Long[st.countTokens()];
		int i = 0;
		while (st.hasMoreTokens()) {
			String s = st.nextToken();
			Long temp = new Long(Long.parseLong(s));
			result[i] = temp;
			i++;
		}
		return result;
	}
	
	/**
	 * Seab selle <i>JussFile</i> ülemkaustad.
	 * @param anc massiiv ülemkaustade ID'dest, alustades relatiivsest juurikast
	 *            ja lõpetades selle JussFile asukohakausta ID-ga 
	 */
	public void setLongAncestors(Long[] anc) {
		if (anc == null) {
			ancestors = null;
			return;
		}
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < anc.length; i++) {
			String s = String.valueOf(anc[i].longValue());
			sb.append(s);
			if (i != anc.length - 1)
				sb.append("/");
		}
		ancestors = sb.toString();
	}

	/**
	 * @return Returns the ancestors.
	 * 
	 * @uml.property name="ancestors"
	 */
	private String getAncestors() {
		return ancestors;
	}

	/**
	 * @param ancestors The ancestors to set.
	 * 
	 * @uml.property name="ancestors"
	 */
	private void setAncestors(String ancestors) {
		this.ancestors = ancestors;
	}

	/**
	 * Leiab selle JussFile' peaversiooni andmebaasi ID.
	 * @return faili peaversiooni andmebaasi ID
	 * 
	 * @uml.property name="mainversion"
	 */
	public Long getMainversion() {
		return mainversion;
	}

	/**
	 * Seab selle JussFile' peaversiooni ID
	 * @param mainversion seatav peaversiooni ID
	 * 
	 * @uml.property name="mainversion"
	 */
	public void setMainversion(Long mainversion) {
		this.mainversion = mainversion;
	}

	/**
	 * Leiab selle JussFile' viimase muutmisaja
	 * @return viimase muutmise aeg
	 * 
	 * @uml.property name="modtime"
	 */
	public Date getModtime() {
		return modtime;
	}

	/**
	 * Seab selle JussFile viimase muutmisaja
	 * @param modtime uus muutmisaeg
	 * 
	 * @uml.property name="modtime"
	 */
	public void setModtime(Date modtime) {
		this.modtime = new Date(DBManager.cut_precision(modtime.getTime()));
	}

	/**
	 * Leiab selle JussFile andmebaasi ID. Kui tegemist pole persistentse objektiga,
	 * siis peaks tagastatav väärtus olema <i>null</i> - kui seda IDd pole vahepeal
	 * teiste vahenditega initsialiseeritud.
	 * @return tagastab selle JussFile' ID
	 * 
	 * @uml.property name="id"
	 */
	public Long getId() {
		return id;
	}

	/**
	 * Seab selle JussFile' andmebaasi ID
	 * @param id seatav ID
	 * 
	 * @uml.property name="id"
	 */
	public void setId(Long id) {
		this.id = id;
	}

	/**
	 * Tagastab selle JussFile'i nime.
	 * @return JussFile'i nime.
	 * 
	 * @uml.property name="name"
	 */
	public String getName() {
		return name;
	}

	/**
	 * Seab selle JussFile' nime
	 * @param name seatav nimi
	 * 
	 * @uml.property name="name"
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * Leiab selle JussFile' vahetu ülemkausta ID
	 * @return Tagastab ülemkausta ID
	 * 
	 * @uml.property name="parent"
	 */
	public Long getParent() {
		return parent;
	}

	/**
	 * Seab selle JussFile' vahetu ülemkausta ID.
	 * @param parent Seatav ülemkausta ID
	 * 
	 * @uml.property name="parent"
	 */
	public void setParent(Long parent) {
		this.parent = parent;
	}

	
	/**
	 * Kontrollib kas see JussFile on kaust.
	 * @return Kas fail on kaust.
	 */
	public boolean isFolder() {
		return folder;
	}

	/**
	 * @param folder kas tegemist on kaustaga või mitte.
	 * 
	 * @uml.property name="folder"
	 */
	public void setFolder(boolean folder) {
		this.folder = folder;
	}

	/**
	 * Kontrollib kas fail lokaalselt eksisteerib. Omab
	 * tähtsust ainult kliendi puhul, serveris eeldatakse
	 * kõigi andmebaasikirjetes sisalduvate failide olemasolu 
	 * @return kas faili eksisteerimist näitav lipp on kohalikus
	 *         andmebaasis seatud või mitte
	 * 
	 * @uml.property name="exists"
	 */
	public boolean getExists() {
		return exists;
	}

	/**
	 * Seab lipu andmebaasis, mis näitab kas kohalik fail lokaalselt
	 * eksisteerib või mitte. Serveris ei oma tähendust.
	 * @param exists seatava lipu väärtus
	 * 
	 * @uml.property name="exists"
	 */
	public void setExists(boolean exists) {
		this.exists = exists;
	}

	/**
	 * Seab faili versiooninumbri
	 * @param ver seatav versiooninumber
	 * 
	 * @uml.property name="version"
	 */
	public void setVersion(int ver) {
		version = ver;
	}

	/**
	 * Leiab selle JussFile' versiooni
	 * @return tagastab JussFile versiooninumbri
	 * 
	 * @uml.property name="version"
	 */
	public int getVersion() {
		return version;
	}

	
	/**
	 * Leiab seda JussFile' iseloomustava lühikese sõne. Sõne on kujul: "ID failinimi (DIR | File)". 
	 * @return JussFile' iseloomustav sõne.
	 */
	public String toString() {
		return getId() + " " + getName() + " " + (isFolder() ? "DIR" : "File"); 
	}

	/**
	 * Leiab selle JussFile' poolt kirjeldatava faili poolt kasutatava ruumi kõvakettal.
	 * @return selle JussFile' poolt kirjeldatava faili suurus baitides.
	 * 
	 * @uml.property name="size"
	 */
	public Long getSize() {
		return size;
	}

	/**
	 * Seab selle JussFile' poolt kirjeldatava faili suuruse.
	 * @param size Seatav suurus baitides.
	 * 
	 * @uml.property name="size"
	 */
	public void setSize(Long size) {
		this.size = size;
	}

	
	
	/**
	 * Leiab faili suurust iseloomustava sõne. Kataloogide puhul
	 * on selleks "DIR", failidel suurus kas baitides, kibibaitides
	 * või mebibaitidtes.
	 * @return faili suurus inimloetava sõnena
	 */
	public String getHumanReadableSize() {
		if (this.isFolder())
			return "DIR";
		long sz = size.longValue();
		if (sz < 102)
			return sz + "B";
		else if (sz < KB)
			return "0." + (sz % KB/100) + "KB";
		else if (sz < MB) 
			return (sz / KB) + "KB";
		return (sz/MB) + "." + (sz % MB)/KB/100  + "MB";
	}
	
	public boolean hasVersions() {
		return (getVersion() > 1) || !getId().equals(getMainversion());
	}
}
