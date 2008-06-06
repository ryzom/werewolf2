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

import org.eclipse.swt.widgets.*;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.*;

import shared.CConfig;

/**
 * Klass splash akna loomiseks. Laeb sisse süsteemi algsed elemendid. 
 * Vajab käivitamiseks SWT jar faile ja dll'e.
 * Linuxi keskkonnas oleks soovitav kasutada GTK jar faile. 
 * Testitud versiooniga 3.0.1.
 * @author Kaido Kalda
 * @version 1.1
 * 
 * @uml.dependency supplier="client.GuiMainFrm" stereotypes="Omondo::Access"
 */

public class GuiLoaderFrm
{

	/**
	 * 
	 * @uml.property name="sShell"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Shell sShell;

	/**
	 * 
	 * @uml.property name="l_logo"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_logo;

	/**
	 * 
	 * @uml.property name="l_ver"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_ver;

	/**
	 * 
	 * @uml.property name="l_icons"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_icons;

	/**
	 * 
	 * @uml.property name="l_gird"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_gird;

	/**
	 * 
	 * @uml.property name="l_status"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_status;

	/**
	 * 
	 * @uml.property name="l_pbText"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_pbText;

	/**
	 * 
	 * @uml.property name="l_pbImg"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_pbImg;

	/**
	 * 
	 * @uml.property name="l_pbImgGover"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_pbImgGover;

	private int count;

	/**
	 * 
	 * @uml.property name="status"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	public GuiStatus status;

	/**
	 * 
	 * @uml.property name="conf"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	public CConfig conf;

	/**
	 * 
	 * @uml.property name="tray_item"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private TrayItem tray_item;

	/**
	 * 
	 * @uml.property name="tray"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Tray tray;

	/**
	 * Programmi peameetod. Laetakse ülejäänud programm.
	 * @param	args	käsurea argumendid (ei kasutata)
	 */

	public static void main(String[] args)
	{

		Display display = Display.getDefault();		
		GuiLoaderFrm thisClass = new GuiLoaderFrm();
		thisClass.createSShell();
		thisClass.sShell.open();

		while (!thisClass.sShell.isDisposed())
		{
			if (!display.readAndDispatch()) display.sleep ();
		}
		display.dispose();		
	}

	/**
	 *  Meetod elementide lisamiseks aknale
	 */

	private void createSShell()
	{
		sShell = new Shell(SWT.BOLD);
		ImageRepository.loadImagesForSplashScreen(Display.getDefault());
		   
		l_logo = new Label(sShell, SWT.NONE);
		l_ver = new Label(sShell, SWT.NONE);
		l_icons = new Label(sShell, SWT.NONE);
		l_status = new Label(sShell, SWT.NONE);
		l_pbText = new Label(sShell, SWT.NONE);
		l_pbImgGover = new Label(sShell, SWT.NONE);
		l_pbImg = new Label(sShell, SWT.NONE);
		l_gird = new Label(sShell, SWT.NONE);

		sShell.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		sShell.setText("Loading JUSS...");
		
		sShell.setImage(ImageRepository.getImage("tray16Icon"));

		Rectangle rec = Display.getCurrent().getBounds();
		int shellX = (rec.width - 302) / 2;
		int shellY = (rec.height - 146) / 2;
		sShell.setBounds(shellX, shellY, 302, 146);

		l_logo.setBounds(new Rectangle(0,10,184,51));
		l_logo.setImage(ImageRepository.getImage("splashLogo"));

		l_ver.setBounds(new Rectangle(240,10,60,20));
		l_ver.setText("Ver. 0.1");
		l_ver.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_ver.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.BOLD));
		l_ver.setForeground(new Color(Display.getDefault(), 192, 192, 192));

		l_gird.setBounds(new Rectangle(0,70,300,84));
		l_gird.setImage(ImageRepository.getImage("grid"));

		l_icons.setBounds(new Rectangle(1,70,146,31));
		l_icons.setImage(ImageRepository.getImage("Empty"));
			
		l_status.setBounds(new Rectangle(5,110,120,10));
		l_status.setText("Loading Console...");
		l_status.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_status.setFont(new Font(Display.getDefault(), "Verdana", 6, SWT.BOLD));

		l_pbText.setBounds(new Rectangle(45,122,36,10));
		l_pbText.setText("0 %");
		l_pbText.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_pbText.setFont(new Font(Display.getDefault(), "Verdana", 6, SWT.NORMAL));

		l_pbImg.setBounds(new Rectangle(5,132,104,11));
		l_pbImg.setImage(ImageRepository.getImage("bar"));

		l_pbImgGover.setBounds(new Rectangle(5,132,104,11));
		l_pbImgGover.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		
    	Runnable runnable = new Runnable()
		{
			public void run()
			{
				if(count < 20 ) Display.getDefault().timerExec(1, this); //250
				load_gui(count);
				count++;
      			}
		};
		
		Display.getDefault().timerExec(1, runnable); //250
	}
	
	/**
	 * Meetod ProgressBar väärtuse muutmiseks
	 * @param	i	uus väärtus.
	 */
	
	private void setPbVal(int i)
	{
		l_pbImgGover.setBounds(i + 8, 132, 105 - i, 11);
		l_pbText.setText(i + " %");
	}

	/**
	 * Meetod süsteemi järkjärguliseks laadimiseks.
	 * Saadakse kätte info teistest klassidest ning kutsutakse esile programmi peaaken.
	 * @param	count	laadimistase.
	 */

	private void load_gui(int count)
	{
		if(count == 0 || count == 2) l_icons.setImage(ImageRepository.getImage("console"));
		else if(count == 1 || count == 3)
		{
			l_icons.setImage(ImageRepository.getImage("empty"));
			l_status.setText("Loading Client...");

			if(count == 3)
			{
				status = new GuiStatus("Startup  initialized...");
				status.print("\tinit: client");
				JussClient.init();
			}
		}

		else if(count == 4 || count == 6) l_icons.setImage(ImageRepository.getImage("tray"));
		else if(count == 5 || count == 7)
		{
			l_icons.setImage(ImageRepository.getImage("console"));
			l_status.setText("Loading Tray...");

			if(count == 7)
			{
				status.print("\tinit: tray");
				loadTray();
			}
		}

		else if(count == 8 || count == 10) l_icons.setImage(ImageRepository.getImage("config"));
		else if(count == 9 || count == 11)
		{
			l_icons.setImage(ImageRepository.getImage("tray"));
			l_status.setText("Loading Config...");

			if(count == 11)
			{
				status.print("\tinit: config");
				conf = new CConfig("./gui.conf.xml");
				if(conf.exists()) conf.load();
				else
				{
					conf.setValue("pointX", 0);
					conf.setValue("pointY", 0);
					conf.save();
					status.print("\t\tconfig: new conf created");
				}
				
			}
		}
		
		else if(count == 12 || count == 14) l_icons.setImage(ImageRepository.getImage("net"));
		else if(count == 13 || count == 15)
		{
			l_icons.setImage(ImageRepository.getImage("config"));
			l_status.setText("Loading Network...");

			if(count == 15)
			{
				status.print("\tinit: network");
			}
		}

		else if(count == 16 || count == 18) l_icons.setImage(ImageRepository.getImage("window"));
		else if(count == 17 || count == 19)
		{
			l_icons.setImage(ImageRepository.getImage("net"));
			l_status.setText("Loading Main Window...");
			ImageRepository.loadImages(Display.getDefault());
		}
		else
		{	
			l_icons.setImage(ImageRepository.getImage("window"));

			status.print("\tinit: mainWindow");
			GuiMainFrm.init(tray, tray_item, sShell, conf);
			return;
		}
		
		setPbVal(count * 5);
	}
	
	/**
	 * Meetod "System Tray" laadimiseks
	 */

	private void loadTray()
	{
     	Image trans = ImageRepository.getImage("tray16"); 
		
		tray = Display.getCurrent().getSystemTray();
		
		if (tray == null)
		{
			status.print("\t\ttray: system tray not exist!");
		}
		
		else 
		{
			tray_item = new TrayItem(tray, SWT.NONE);
			tray_item.setToolTipText("J.U.S.S");
			tray_item.setImage(trans);
		}
	}
}