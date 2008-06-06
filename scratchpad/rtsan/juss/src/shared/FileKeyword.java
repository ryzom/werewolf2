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
 * Klass sidumaks faile ja nende kohta k‰ivaid vıtmesınasid.
 * @author Taimo Peelo
 */
public class FileKeyword implements Serializable {

	/**
	 * 
	 * @uml.property name="fileID" multiplicity="(0 1)"
	 */
	private Long fileID;

	/**
	 * 
	 * @uml.property name="keywordID" multiplicity="(0 1)"
	 */
	private Long keywordID;

	
	
	/**
	 * Loob v‰‰rtustamata isendi
	 */
	public FileKeyword() {}
	
	/**
	 * Seob vıtmesına failiga.  
	 * @param fID faili ID
	 * @param kID vıtmesına ID
	 */
	public FileKeyword(Long fID, Long kID) {
		fileID = fID;
		keywordID = kID;
	}

	/**
	 * Tagastab isendile kuuluva faili ID
	 * @return isendi faili ID
	 * 
	 * @uml.property name="fileID"
	 */
	public Long getFileID() {
		return fileID;
	}

	/**
	 * Seab isendile kuuluva faili ID.
	 * @param fileID faili ID.
	 * 
	 * @uml.property name="fileID"
	 */
	public void setFileID(Long fileID) {
		this.fileID = fileID;
	}

	/**
	 * Tagastab isendile kuuluva vıtmesına ID.
	 * @return vıtmesına ID.
	 * 
	 * @uml.property name="keywordID"
	 */
	public Long getKeywordID() {
		return keywordID;
	}

	/**
	 * Seab isendile kuuluva vıtmesına ID
	 * @param keywordID vıtmesına ID.
	 * 
	 * @uml.property name="keywordID"
	 */
	public void setKeywordID(Long keywordID) {
		this.keywordID = keywordID;
	}

	
	private void readObject(ObjectInputStream aInputStream)
			throws ClassNotFoundException, IOException {
		aInputStream.defaultReadObject();
	}

	private void writeObject(ObjectOutputStream aOutputStream)
			throws IOException {
		aOutputStream.defaultWriteObject();
	}
	
	public boolean equals (Object o) {
		if (this == o) 
			return true;
		if (!(o instanceof FileKeyword))
			return false;
		
		FileKeyword k = (FileKeyword) o;
		
		return (k.fileID == this.fileID && k.keywordID == this.keywordID);
	}
	
	public int hashCode() {
		return (int) (fileID.longValue() * keywordID.longValue() + fileID.longValue() + keywordID.longValue());
	}	
}
