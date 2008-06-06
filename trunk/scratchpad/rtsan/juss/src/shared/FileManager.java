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

import java.io.*;
import java.util.StringTokenizer;


/**
 * Klass mis tegeleb JUSSi poolt hallatavate failide lugemise ja kirjutamisega
 * @author Henri Kuuste
 */
public class FileManager {
	/**
	 * Baitide arv mida korraga kettale kirjutada.
	 */
	public static int chunkSize = 4096;

	public static final String OVERWRITE = "overwrite";
	public static final String NEW_VERSION = "version";
	
	// erinevat t��pi ligip��su�igused, tulevikku silmas pidades
	public static final String F_READ = "r";
	public static final String F_CREATE = "c";
	public static final String F_WRITE = "w";
	public static final String F_DELETE = "d";

	public static final long FOLDERS = 1;
	public static final long FILES = 2;
	
	/**
	 * Kopeerib faili.
	 * @param src l�htefail
	 * @param dest v�ljundfail
	 * @throws IOException
	 */
	public static void copyFile(File src, File dest) throws IOException {
		if(src.equals(dest))
			return;
		copyFile(src, dest, null);
	}
	
	/**
	 * Kopeerib faili, v�imaldades j�lgida kopeerimise progressi.
	 * @param src l�htefail
	 * @param dest v�ljundfail
	 * @param p IProgress liidest realiseeriv objekt.
	 * @throws IOException
	 */
	public static void copyFile(File src, File dest, IProgress p) throws IOException {
		BufferedOutputStream bo = new BufferedOutputStream(new FileOutputStream(dest));
		BufferedInputStream bi = new BufferedInputStream(new FileInputStream(src));
		byte[] buffer = new byte[chunkSize];
		int len;
		while((len = bi.read(buffer)) >= 0) {
			bo.write(buffer, 0, len);
			bo.flush();
			if(p != null)
				p.addDone(len);
		}
		bo.close();
		bi.close();
	}
		
	/**
	 * Salvestab parameetrina antud sisendvoo sisu faili.
	 * @param in salvestatav sisendvoog 
	 * @param f v�ljundfail 
	 * @throws IOException
	 */
	public static void saveToFile(InputStream in, File f) throws IOException {
		saveToFile(in, f, null);
	}	
	
	/**
	 * Salvestab parameetrina antud sisendvoo sisu faili, v�imaldades j�lgida progressi.
	 * @param in salvestatav sisendvoog
	 * @param f v�ljundfail 
	 * @param p IProgress liidest realiseeriv objekt.
	 * @throws IOException
	 */
	public static void saveToFile(InputStream in, File f, IProgress p) throws IOException {
		BufferedOutputStream out = new BufferedOutputStream(new FileOutputStream(f));
		saveToStream(in, out, p);
		out.close();
	}
	
	/**
	 * Kirjutab sisendvoost v�ljundvoogu.
	 * @param in sisendvoog
	 * @param out v�ljundvoog
	 * @throws IOException
	 */
	public static void saveToStream(InputStream in, OutputStream out) throws IOException {
		saveToStream(in, out, null);
	}
	
	/**
	 * Kirjutab sisendvoost v�ljundvoogu, v�imaldades j�rgida progressi
	 * @param in sisendvoog
	 * @param out v�ljundvoog
	 * @throws IOException
	 */
	public static void saveToStream(InputStream in, OutputStream out, IProgress p) throws IOException {
		byte[] buffer = new byte[chunkSize];
		int len;
		while((len = in.read(buffer)) >= 0) {
			out.write(buffer, 0, len);
			out.flush();
			if(p != null)
				p.addDone(len);
		}
	}
	
	/**
	 * Asendab k�ik '\' ja '/' m�rgid  antud rajal '/'. 
	 * Ei arvesta veel ops�steemidega mis lubavad kaustade 
	 * ja failide nimedes s�mbolite '\' ja '/' kasutamist
	 * @param path
	 * @return S�ne kus kaldkriipsud "parandatud".
	 */
	public static String fixSlashes(String path) {
		StringTokenizer tok = new StringTokenizer(path, "/\\");
		String result = "";
		while(tok.hasMoreTokens())
			result += "/" + tok.nextToken();
		if(path.charAt(0) != '/')
			result = result.substring(1);
		return result;
	}
	
	
	/**
	 * Eemaldab failis�steemist antud nimega faili.
	 * @param filename eemaldatava faili nimi
	 * @return kas eemaldamine �nnestus v�i mitte
	 */
	public static boolean deleteFile(String filename) {
		return deleteFile(new File(filename));
	}
	
	/**
	 * Eemaldab failis�steemist antud faili.
	 * @param f eemaldatav fail
	 * @return kas eemaldamine �nnestus v�i mitte
	 */
	public static boolean deleteFile(File f) {
		return f.delete();
	}
}
