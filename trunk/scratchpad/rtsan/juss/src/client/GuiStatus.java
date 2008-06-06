/*
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

package client;
/**
 * Klass staatuse väljastamiseks konsooli.
 * 
 * @author Kaido Kalda
 * @version 1.0
 */
class GuiStatus
{
	/**
	 * Tühi konstruktor.
	 */
	GuiStatus()
	{
		
	}
	/**
	 * Konstruktor staatuse väljastamiseks.
	 * @param	s	staatus.
	 */
	GuiStatus(String s)
	{
		System.out.println(s);
	}
	/**
	 * Meetod staatuse väljastamiseks.
	 * @param	s	staatus.
	 */
	public void print(String s)
	{
		System.out.println(s);
	}
}