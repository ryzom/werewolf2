/*
 * Created on Apr 25, 2005
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

import java.io.Serializable;

/**
 * Erinevaid otsingukriteeriume esitav klass. Kasutatakse JUSSi
 * s�steemis otsingute teostamisel.
 * @author Henri Kuuste
 */
public class SearchCriteria implements Serializable {
	private Long t = new Long(0);

	/**
	 * 
	 * @uml.property name="creator" multiplicity="(0 1)"
	 */
	private String creator = "";

	private String str = null;
	
	/** konstant, v�ljendab otsingut��pi v�tmes�na j�rgi */
	public static final long KEYWORDS = 1;
	/** konstant, v�ljendab otsingut��pi kirjelduse j�rgi */
	public static final long DESCRIPTION = 1<<1;
	/** konstant, v�ljendab otsingut��pi failinime j�rgi */
	public static final long FILENAME = 1<<2;
	/** konstant, v�ljendab otsingut��pi kommentaaride j�rgi */
	public static final long COMMENTS = 1<<3;
	/** konstant, v�ljendab otsingut��pi faili looja j�rgi */
	public static final long SEARCH_BY_CREATOR = 1<<4;
	
	/**
	 * Vaikekonstruktor. Otsingut��bi m��ramiseks v�ib otsingut��pi
	 * m��ravaid konstandid kas liita (c1 + c2 + c3) v�i ORida (c1 | c2 | c3),
	 * m�lemad annavad sama tulemuse.
	 * @param string s�ne mida hakatakse otsima
	 * @param type otsingut��bi m��rang - milliste v�ljade hulgast s�ne otsima hakatakse
	 */
	public SearchCriteria(String string, Long type) {
		setString(string);
		setType(type);
	}
	
	/**
	 * Seab otsis�ne.
	 * @param string Seatav otsis�ne
	 */
	public void setString(String string) {
		str = string.toLowerCase();
	}
	
	/**
	 * @return Tagastab selle isendi otsis�ne
	 */
	public String getString() {
		return str;
	}

	/**
	 * Seab otsingut��bi
	 * @param type otsingut��p 
	 */
	public void setType(Long type) {
		t = type;
	}
	
	/**
	 * @return tagastab otsingut��bi
	 */
	public Long getType() {
		return t;
	}

	/**
	 * Seab faili looja, kelle j�rgi otsing toimub
	 * @param c looja kasutajanimi
	 * 
	 * @uml.property name="creator"
	 */
	public void setCreator(String c) {
		creator = c;
	}

	/**
	 * @return millise kasutaja poolt loodud faile otsitakse isendi poolt
	 * 
	 * @uml.property name="creator"
	 */
	public String getCreator() {
		return creator;
	}

}
