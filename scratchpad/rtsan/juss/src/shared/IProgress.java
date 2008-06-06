/*
 * Created on Apr 21, 2005
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

/**
 * Liides, mis võimaldab seda implementeerivate klasside
 * tööprotsessi monitoorida. Mõeldud peamiselt failitöötlusega
 * tegelevate klasside tööprotsessi progressi teadasaamiseks.  
 * @author Henri Kuuste
 */
public interface IProgress {
	
	/**
	 * Seab tööühikute arvu milleni jõudes tööprotsess lõpeb. 
	 * @param end tööprotsessi lõppu näitav suurus
	 */
	public void setSize(long end);
	
	/**
	 * Seab juba "tehtud" tööühikute arvu
	 * @param amount tööühikute arv
	 */
	public void setDone(long amount);
	
	
	/**
	 * Tagastab tehtud tööühikute arvu.
	 * @return tehtud tööühikute arv.
	 */
	public long getDone();
	
	/**
	 * Lisab juba täidetud tööühikutele veel ühikuid. 
	 * @param amount lisatav ühikute arv
	 */
	public void addDone(long amount);
	
	/**
	 * Teatab kui kaugel tööga ollakse. Tagastav väärtus on suhteline - 
	 * kui tööga pole veel alustatud tagastatakse väärtus 0, kui töö on juba
	 * lõpetatud siis väärtus 1.
	 * @return tagastab arvu vahemikus [0;1] 
	 */
	public double getProgress();
}
