/*
 * Created on May 1, 2005
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

import java.util.*;
import shared.JussFile;

/**
 * Klass JussFile'de vırdlemiseks erinevate v‰ljade alusel. Sorteerimisel vaikimisi
 * on tulemused v‰iksemast suuremani. Kausta loetakse failist v‰iksemaks :)
 * ‹lej‰‰nud t¸¸pide j‰rjestus on loogiline.
 * @author Taimo Peelo
 */

public class JussFileComparator implements Comparator {
	/** konstant mis n‰itab sorteerimist nime j‰rgi */
	public final static int NAME = 1;
	/** konstant mis n‰itab sorteerimist muutmisaja j‰rgi */
	public final static int MODTIME = 2;
	/** konstant mis n‰itab sorteerimist t¸¸bi (fail, kaust) j‰rgi */
	public final static int TYPE = 4;
	/** konstant mis n‰itab sorteerimist suuruse j‰rgi */
	public final static int SIZE = 8;
	/** konstant mis n‰itab sorteerimist faili versiooni j‰rgi */ 
	public final static int VERSION = 256;

	// kas sorteerida vastupidises j‰rjekorras
	private boolean reversed = false;
	// kas ignoreerida failinimede vırdlusel suur- ja v‰iket‰htede erinevusi
	private boolean ignoreCase = false;

	/**
	 * hoiab j‰rjestatuna neid parameetreid, mille j‰rgi vırdlemine toimuma hakkab
	 * 
	 * @uml.property name="compareOrder"
	 * @uml.associationEnd elementType="java.lang.Integer" multiplicity="(0 -1)"
	 */
	private ArrayList compareOrder = new ArrayList();

	
	protected boolean isValidComparisonConstant(int x) {
		return x == NAME || x == MODTIME || x == TYPE || x == VERSION || x == SIZE;
	}
	
	/** 
	 * @param compareType KONSTANT m‰‰rab v‰lja mida k‰sitletakse sortimisj‰rjestuse m‰‰ramisel primaarsena  
	 */
	public JussFileComparator(int compareType) {
		if (!(isValidComparisonConstant(compareType)))  
			throw new IllegalArgumentException();
		else
			compareOrder.add(new Integer(compareType));
	}
	
	/**
	 * @param x KONSTANT mis m‰‰rab sekundaarse, tertsiaarse jne. v‰lja mille j‰rgi
	 *          sorteerimine toimub.
	 * @return muudetud JussFileComparator'i
	 */
	public JussFileComparator by(int x) {
		if (!(isValidComparisonConstant(x)))
			throw new IllegalArgumentException();
		Integer i = new Integer(x);
		if (!compareOrder.contains(i)) 
			compareOrder.add(new Integer(x));			
		return this;
	}
	
	public int compare(Object o1, Object o2) {
		JussFile jf1 = (JussFile) o1, jf2 = (JussFile)o2;
		for (Iterator i = compareOrder.iterator(); i.hasNext(); ) {
			int cResult = 0;
			int cParam = ((Integer)i.next()).intValue();
			switch (cParam) {
				case NAME:    cResult = compareByName(jf1, jf2);
						      break;
				case TYPE:    cResult = compareByType(jf1, jf2);
				   		      break; 
				case MODTIME: cResult = compareByModTime(jf1, jf2);
				   		      break;
				case VERSION: cResult = compareByVersion(jf1, jf2);
				   			  break;
				case SIZE:    cResult = compareBySize(jf1, jf2); 
				              break;
			}
			if (reversed)
				cResult = -cResult;
			if (cResult != 0)
				return cResult;
		}
		return 0;
	}
	
	/**
	 * Muudab j‰rjestuse selles komparaatoris.
	 * @return muudetud j‰rjestusega komparaator
	 */
	public JussFileComparator reverse() {
		reversed = (!reversed);
		return this;
	}

	/**
	 * Seab j‰rjestuse selles komparaatoris
	 * @param b tıev‰‰rtus n‰itamaks kas sorditakse "pˆˆratud" (mittekasvavas) j‰rjestuses vıi mitte
	 * @return muudetud komparaator
	 */
	public JussFileComparator reverse(boolean b) {
		reversed = b;
		return this;
	}
	
	/**
	 * Seab tıev‰‰rtuse mis m‰‰rab kas JussFile'de nimede vırdlused toimuvad
	 * tıstutundlikult vıi mitte. Vaikev‰‰rtuseks on <i>false</i>
	 * @param b seatav tıev‰‰rtus
	 * @return muudetud komparaatori
	 */
	public JussFileComparator setIgnoreCase(boolean b) {
		ignoreCase = b;
		return this;
	}

	private int compareBySize(JussFile jf1, JussFile jf2) {
		return jf1.getSize().compareTo(jf2.getSize());
	}
	
	private int compareByName(JussFile jf1, JussFile jf2) {
		return jf1.getName().compareTo(jf2.getName());
	}
	
	private int compareByVersion(JussFile jf1, JussFile jf2) {
		int v1 = jf1.getVersion(), v2 = jf2.getVersion();
		if (v1 < v2)
			return -1;
		else if (v1 > v2)
			return 1;
		return 0;
	}
	
	private int compareByType(JussFile jf1, JussFile jf2) {
		boolean f1 = jf1.isFolder(), f2 = jf2.isFolder();
		if ((!f1) && f2)
			return 1;
		else if (f1 && (!f2))
			return -1;
		return 0;
	}
	
	private int compareByModTime(JussFile jf1, JussFile jf2) {
		Date dt1 = jf1.getModtime(), dt2 = jf2.getModtime();
		return (dt1.compareTo(dt2));
	}
}
