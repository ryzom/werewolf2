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

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Node;
import org.dom4j.DocumentException;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.SAXReader;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.io.XMLWriter;
import java.io.*;
import java.util.ArrayList;
import java.util.*;
import shared.FileManager; // fixSlashes

/**
 * Konfiguratsioonihaldurklass XML-failidest konfiguratsioonide lugemiseks ja kirjutamiseks.
 * 
 * @author Vilen Looga, Taimo Peelo 
 */

public class CConfig {

	/**
	 * 
	 * @uml.property name="document"
	 * @uml.associationEnd elementType="org.dom4j.Element" multiplicity="(0 -1)"
	 */
	private Document document = DocumentHelper.createDocument();

	private String FileName = "conf.xml";

	/**
	 * 
	 * @uml.property name="confElement"
	 * @uml.associationEnd multiplicity="(1 1)"
	 */
	private Element confElement = document.addElement("conf");

	private static final String root = "//conf";

	private static Set classpathelements = null;
	
	private static final Logger log = Logger.getLogger("juss.client.CConfig");

	static {
		ArrayList tempList = new ArrayList();
		String pathsep = System.getProperty("path.separator");
		String classpath = System.getProperty("java.class.path");
		StringTokenizer st = new StringTokenizer(classpath, pathsep);
		while (st.hasMoreTokens())
			tempList.add(FileManager.fixSlashes((String) st.nextToken()));

		classpathelements = new HashSet();

		for (Iterator i = tempList.iterator(); i.hasNext();) {
			String cs = (String) i.next();

			File f = new File(cs);
			if (f.isFile()) {
				String[] elements = cs.split("\\/");
				StringBuffer sb = new StringBuffer();
				for (int j = 0; j < elements.length - 1; j++)
					sb.append(elements[j] + "/");
				if (sb.length() > 0)
					classpathelements.add(sb.toString());
			} else if (f.isDirectory()) {
				classpathelements.add(cs + "/");
			}
		}
	}

	/**
	 * Loob uue konfiguratsioonihalduri, seostades selle nıutud failiga.
	 * @param fileName fail millega antud konfiguratsioonihaldur seostatakse.
	 *                 Kui argument on null, kasutatakse vaikev‰‰rtusena faili "conf.xml".   
	 */
	public CConfig(String fileName) {
		if (fileName != null)
			FileName = fileName;
	}
	
	/**
	 * Loob uue konfiguratsioonihalduri, seostades selle failiga
	 * "conf.xml"    
	 */
	public CConfig() {
		this(null);
	}


	/**
	 * Laeb konfiguratsioonihalduriga seostatud faili kıvakettalt.
	 * Kui faili antud teel ei leidu, siis otsitakse seda ka CLASSPATHIlt
	 * ja kasutatakse esimest sealt leitud ıige nimega konfiguratsioonifaili. 
	 */
	public void load() {

		File f = new File(FileName);
		if (!f.exists()) {
			for (Iterator i = classpathelements.iterator(); i.hasNext();) {
				String cs = (String) i.next();
				String fullpath = cs + FileName;
				f = new File(fullpath);
				if (f.exists()) {
					FileName = fullpath;
					break;
				}
			}
		}

		try {
			SAXReader saxReader = new SAXReader();
			document = saxReader.read(FileName);
		}

		catch (DocumentException e) {
			log.error("File not found " + FileName);
		}
	}

	
	/**
	 * Kontrollib, kas isendiga seostatud konfiguratsioonfail eksisteerib ja on loetav
	 * @return kas konfiguratsioonifail eksisteerib ja on loetav 
	 */
	public boolean exists() {
		File f = new File(FileName);
		return (f.exists() && f.isFile() && f.canRead());
	}

	/**
	 * Salvestab isendis salvestatud konfiguratsiooni temaga seostatud
	 * konfiguratsioonifaili
	 */
	public void save() {
		try {
	        OutputFormat format = OutputFormat.createPrettyPrint();
	        XMLWriter output = new XMLWriter(new FileWriter(new File(FileName)), format);
			output.write(document);
			output.close();
		}

		catch (IOException e) {
			log.error(e.getMessage());
		}
	}

	/**
	 * Otsib konfiguratsioonist antud XPath Node'i parameetrit 'value' ja
	 * tagastab selle v‰‰rtuse sınena. Kui parameetrit eksisteeri, tagastab
	 * nulli.
	 * @param path XPath node kujul "//blaah/foo"
	 * @return node 'value' parameetri v‰‰rtus sınena
	 */
	public String getStringValue(String path) {
		String value = null;

		Node node = document.selectSingleNode(root + "/" + path);
		if (node != null)
			value = node.valueOf("@value");

		return value;
	}

	/**
	 * Otsib konfiguratsioonist antud XPath Node'i parameetrit 'value' ja
	 * tagastab selle v‰‰rtuse t‰isarvuna. Kui parameetrit ei eksisteeri
	 * vıi selle v‰‰rtus pole t‰isarv, tagastab null-i.
	 * @param path XPath node kujul "//blaah/foo"
	 * @return selle Node 'value' parameetri v‰‰rtus t‰isarvuna
	 */
	public Integer getIntValue(String path) {
		String s = getStringValue(path);
		if (s != null) {
			Integer result;
			try {
				result = Integer.decode(s);
			} catch (NumberFormatException e) {
				return null;
			}
			return result;
		}
		return null;
	}

	/**
	 * Otsib konfiguratsioonist antud XPath Node'i parameetrit 'value' ja
	 * tagastab selle v‰‰rtuse suure t‰isarvuna (Long). Kui parameetrit ei eksisteeri
	 * vıi selle v‰‰rtus pole t‰isarv, tagastab null-i.
	 * @param path XPath node kujul "//blaah/foo"
	 * @return selle Node 'value' parameetri v‰‰rtus t‰isarvuna
	 */
	public Long getLongValue(String path) {
		String s = getStringValue(path);
		if (s != null) {
			Long result;
			try {
				result = Long.decode(s);
			} catch (NumberFormatException e) {
				return null;
			}
			return result;
		}
		return null;
	}

	/**
	 * Otsib konfiguratsioonist antud XPath Node'i parameetrit 'value' ja
	 * tagastab selle v‰‰rtuse double t¸¸pi ujukomaarvuna. Kui parameetrit ei eksisteeri
	 * vıi selle v‰‰rtus pole ujukomaarv, tagastab null-i.
	 * @param path XPath node kujul "//blaah/foo"
	 * @return selle Node 'value' parameetri v‰‰rtus ujukomaarvuna
	 */
	public Double getDoubleValue(String path) {
		String s = getStringValue(path);
		if (s != null) {
			Double result;
			try {
				result = Double.valueOf(s);
			} catch (NumberFormatException e) {
				return null;
			}
			return result;
		}
		return null;
	}

	/**
	 * Otsib konfiguratsioonist antud XPath Node'i parameetrit 'value' ja
	 * tagastab selle v‰‰rtuse tıev‰‰rtusena. Tagastab "true" siis kui
	 * k¸situd Node eksisteerib ja tema value="true" vıi value="yes".
	 * Kıikidel teistel juhtudel tagastab "false". 
	 * @param path XPath node kujul "//blaah/foo"
	 * @return selle Node 'value' parameetri tıev‰‰rtus
	 */
	public Boolean getBooleanValue(String path) {
		// ei ole kontrolli vaja, Booleani erip2rad!
		return Boolean.valueOf(path); 
	}
	
	/**
	 * Seab/tekitab/muudab antud Node'i parameetri "value" 
	 * v‰‰rtuse konfiguratsioonis. 
	 * @param path XPath soovitava Node'ini 
	 * @param value parameetrile omistatav v‰‰rtus
	 */
	public void setValue(String path, boolean value) {
		String help = String.valueOf(value);
		setValue(path, help);
	}	

	/**
	 * Seab/tekitab/muudab antud Node'i parameetri "value" 
	 * v‰‰rtuse konfiguratsioonis. 
	 * @param path XPath soovitava Node'ini 
	 * @param value parameetrile omistatav v‰‰rtus
	 */
	public void setValue(String path, Boolean value) {
		String help = String.valueOf(value);
		setValue(path, help);
	}

	/*
	 * Antud: parameeter ja selle uus v‰‰rtus (parameetreid antakse ette
	 * kujul"a/b/c"). Kui antud parameetrit ei eksisteeri siis see luuakse koos
	 * vastava struktuuriga.
	 * 
	 * Kui etteantud parameeter eksisteerib menetletakse teda fastNode - iga,
	 * vastasel juhul hakatakse looma XML faili struktuuri.
	 * 
	 *  
	 */

	
	/**
	 * Seab/tekitab/muudab antud Node'i parameetri "value" 
	 * v‰‰rtuse konfiguratsioonis. 
	 * @param path XPath soovitava Node'ini 
	 * @param value parameetrile omistatav v‰‰rtus
	 */
	public void setValue(String path, String value) {
		Node fastNode = document.selectSingleNode(root + "/" + path
				+ "/@value");

		if (fastNode != null) {
			fastNode.setText(value);
			save();
		}

		else if (fastNode == null) {
			StringTokenizer st = new StringTokenizer(path, "/");
			String currentPath = root;
			String last = "";

			while (true) {
				last = st.nextToken();
				Node node = document.selectSingleNode(currentPath + "/" + last);
				Node valueNode = document.selectSingleNode(currentPath + "/"
						+ last + "/@value");

				if (st.hasMoreTokens() && node != null) {
					currentPath = currentPath + "/" + last;
					continue;
				}

				else if (st.hasMoreTokens() && node == null) {
					Element elem = (Element) document
							.selectSingleNode(currentPath);
					Element newElem = elem.addElement(last);
					currentPath = currentPath + "/" + last;
					save();
				}

				else if (!st.hasMoreTokens() && node != null
						&& valueNode != null) {
					valueNode.setText(value);
					save();
					break;
				}

				else if (!st.hasMoreTokens() && node != null
						&& valueNode == null) {
					Element elem = (Element) node.clone();
					elem.addAttribute("value", value);
					save();
					break;
				}

				else if (!st.hasMoreTokens() && node == null) {
					Element elem = (Element) document
							.selectSingleNode(currentPath);
					Element newElem = elem.addElement(last);
					newElem.addAttribute("value", value);
					save();
					break;
				}
			}
		}
	}

	/**
	 * Seab/tekitab/muudab antud Node'i parameetri "value" 
	 * v‰‰rtuse konfiguratsioonis. 
	 * @param path XPath soovitava Node'ini 
	 * @param value parameetrile omistatav v‰‰rtus
	 */
	public void setValue(String path, int value) {
		String help = String.valueOf(value);
		setValue(path, help);
	}
	
	/**
	 * Seab/tekitab/muudab antud Node'i parameetri "value" 
	 * v‰‰rtuse konfiguratsioonis. 
	 * @param path XPath soovitava Node'ini 
	 * @param value parameetrile omistatav v‰‰rtus
	 */
	public void setValue(String path, double value) {
		String help = String.valueOf(value);
		setValue(path, help);
	}

	/**
	 * Seab/tekitab/muudab antud Node'i parameetri "value" 
	 * v‰‰rtuse konfiguratsioonis. 
	 * @param path XPath soovitava Node'ini 
	 * @param value parameetrile omistatav v‰‰rtus
	 */
	public void setValue(String path, long value) {
		String help = String.valueOf(value);
		setValue(path, help);
	}
}

