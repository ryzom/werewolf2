/*
 * Created on May 7, 2005
 *
 */
package server;

import java.io.FileWriter;
import java.io.IOException;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.XMLWriter;

import shared.CConfig;
import shared.FileManager;

/**
 * Teeb konfiguratsiooni failid Tomcat'ile
 * 
 * @author Henri Kuuste
 */
public class ServerConfCreator {

	private static int maxUploadSize = 2000000;
	private static String root = null;
	private static String server = null;
	private static String tomcatRoot = null;
	
	public static void main(String[] args) {
		parseArgs(args);
		if(root == null || server == null || tomcatRoot == null)
			return;
		createJussContext();
		createJussConf();
	}
	
	private static void parseArgs(String[] args) {
		for(int i = 0; i < args.length; i++) {
			if(args[i].equals("-maxUploadSize") || args[i].equals("-m")) {
				i++;
				if(i < args.length) {
					maxUploadSize = Integer.parseInt(args[i]);
				}
			} else if (args[i].equals("-root") || args[i].equals("-r")) {
				i++;
				if(i < args.length) {
					root = args[i];
				}
			} else if (args[i].equals("-server") || args[i].equals("-s")) {
				i++;
				if(i < args.length) {
					server = args[i];
				}
			} else if (args[i].equals("-tomcat") || args[i].equals("-t")) {
				i++;
				if(i < args.length) {
					tomcatRoot = args[i];
				}
			}
		}
	} 
	
	public static void createJussContext() {
		String confFile = tomcatRoot +  "/conf/Catalina/localhost/juss.xml";
		Document conf = DocumentHelper.createDocument();
		Element root = conf.addElement("Context");
		root.addAttribute("docBase", server);
		root.addAttribute("path", "/juss");
		root.addAttribute("reloadable", "true");
		root.addAttribute("workDir", server + "/work");

        OutputFormat format = OutputFormat.createPrettyPrint();
        try {
        	XMLWriter writer = new XMLWriter(new FileWriter(confFile), format);
        	writer.write( conf );
        	writer.close();
        } catch(IOException e) {
        	e.printStackTrace();
        }
	}
	
	public static void createJussConf() {
		CConfig conf = new CConfig(tomcatRoot + "/bin/server.conf.xml");
		conf.setValue("MaxUploadSize", maxUploadSize);
		conf.setValue("root", FileManager.fixSlashes(root));
		conf.save();
	}
}
