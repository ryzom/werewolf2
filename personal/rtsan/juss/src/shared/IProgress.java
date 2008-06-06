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
 * Liides, mis v�imaldab seda implementeerivate klasside
 * t��protsessi monitoorida. M�eldud peamiselt failit��tlusega
 * tegelevate klasside t��protsessi progressi teadasaamiseks.  
 * @author Henri Kuuste
 */
public interface IProgress {
	
	/**
	 * Seab t���hikute arvu milleni j�udes t��protsess l�peb. 
	 * @param end t��protsessi l�ppu n�itav suurus
	 */
	public void setSize(long end);
	
	/**
	 * Seab juba "tehtud" t���hikute arvu
	 * @param amount t���hikute arv
	 */
	public void setDone(long amount);
	
	
	/**
	 * Tagastab tehtud t���hikute arvu.
	 * @return tehtud t���hikute arv.
	 */
	public long getDone();
	
	/**
	 * Lisab juba t�idetud t���hikutele veel �hikuid. 
	 * @param amount lisatav �hikute arv
	 */
	public void addDone(long amount);
	
	/**
	 * Teatab kui kaugel t��ga ollakse. Tagastav v��rtus on suhteline - 
	 * kui t��ga pole veel alustatud tagastatakse v��rtus 0, kui t�� on juba
	 * l�petatud siis v��rtus 1.
	 * @return tagastab arvu vahemikus [0;1] 
	 */
	public double getProgress();
}
