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

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.events.*;
import java.io.File;
/**
 * Klass nii faili importimise kui uue kausta loomise akna tegemiseks.
 * Vajab käivitamiseks SWT jar faile ja dll'e.
 * Linuxi keskkonnas oleks soovitav kasutada GTK jar faile. 
 * Testitud versiooniga 3.0.1.
 * 
 * @author Kaido Kalda
 * @version 1.0
 */
public class GuiImportFileFrm
{

	/**
	 * 
	 * @uml.property name="sShell"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Shell sShell;

	/**
	 * 
	 * @uml.property name="g_general"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Group g_general;

	/**
	 * 
	 * @uml.property name="b_cancel"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Button b_cancel;

	/**
	 * 
	 * @uml.property name="b_finish"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Button b_finish;

	private String[] settings;

	/**
	 * 
	 * @uml.property name="l_fileNameText"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_fileNameText;

	/**
	 * 
	 * @uml.property name="l_fileName"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_fileName;

	/**
	 * 
	 * @uml.property name="l_fileDescription"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_fileDescription;

	/**
	 * 
	 * @uml.property name="t_fileDescription"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Text t_fileDescription;

	/**
	 * 
	 * @uml.property name="t_fileNameText"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Text t_fileNameText;

	/**
	 * 
	 * @uml.property name="l_fileKeywords"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_fileKeywords;

	/**
	 * 
	 * @uml.property name="t_fileKeywords"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Text t_fileKeywords;

	/**
	 * 
	 * @uml.property name="l_info"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_info;

	
	private int windowType;
	/**
	 * Teostab akna alglaadimise. 
	 * @param	fileToAdd	informatsioon faili kohta mida lisatakse.
	 * @param	x	akna x positsioon.
	 * @param	y	akna y positsioon
	 * @param	parent	aken, mille suhtes on antud aken MODAL
	 * @return 	kommentaari ja keyword'id
	 * @see	GuiMainFrm
	 */
  	public static String[] init(File fileToAdd, int type, int x, int y, Shell parent)
	{
		Display display = Display.getDefault();		
		GuiImportFileFrm thisClass = new GuiImportFileFrm();
		thisClass.windowType = type;
		
		thisClass.settings = new String[3];

		thisClass.createSShell(fileToAdd, x, y, parent) ;
		thisClass.sShell.open();

		while (!thisClass.sShell.isDisposed())
		{
			if (!display.readAndDispatch()) display.sleep();
		}
		
		return thisClass.settings;		
	}

	/**
	 * Peamine meetod akna laadimiseks. Tekitab kõik elemendid aknale
	 * ning lisatakse kuularid.
	 */
	private void createSShell(File fileToAdd, int x, int y, Shell parent)
	{
		sShell = new Shell(parent, SWT.APPLICATION_MODAL | SWT.DIALOG_TRIM);

		g_general = new Group(sShell, SWT.NONE);
		b_cancel = new Button(sShell, SWT.NONE);
		b_finish = new Button(sShell, SWT.NONE);
		
		l_fileNameText = new Label(g_general, SWT.NONE);
		t_fileNameText = new Text(g_general, SWT.BORDER);
		
		l_fileName = new Label(g_general, SWT.NONE);
		l_fileDescription = new Label(g_general, SWT.NONE);
		t_fileDescription = new Text(g_general, SWT.BORDER | SWT.MULTI | SWT.WRAP | SWT.V_SCROLL);
		l_fileKeywords = new Label(g_general, SWT.NONE);
		t_fileKeywords = new Text(g_general, SWT.BORDER | SWT.MULTI | SWT.WRAP | SWT.V_SCROLL);
		l_info = new Label(g_general, SWT.NONE);
		
		sShell.setSize(347,295);
		sShell.setLocation(x, y);

		b_cancel.setBounds(new Rectangle(185,230,70,30));
		b_cancel.setText("Cancel");

		b_finish.setBounds(new Rectangle(265,230,70,30));

		g_general.setText("File information");
		
		if(windowType == 0)
		{
			l_fileNameText.setBounds(new Rectangle(80,15,250,15));
			l_fileNameText.setText(fileToAdd.getName());
			sShell.setText("Import new file");
			b_finish.setText("Import");
		}
		else if(windowType == 1)
		{
			t_fileNameText.setBounds(new Rectangle(80,15,250,15));
			sShell.setText("Make new directory");
			b_finish.setText("Create");
		}
		
		l_fileNameText.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.BOLD));
		l_fileName.setBounds(new Rectangle(10,15,70,15));
		l_fileName.setText("Name:");
		l_fileName.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		l_fileDescription.setBounds(new Rectangle(10,35,70,15));
		l_fileDescription.setText("Description:");
		l_fileDescription.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		t_fileDescription.setBounds(new Rectangle(10,50,321,60));
		l_fileKeywords.setBounds(new Rectangle(10,115,70,15));
		l_fileKeywords.setText("Keywords:");
		l_fileKeywords.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		t_fileKeywords.setBounds(new Rectangle(10,130,321,60));
		l_info.setBounds(new Rectangle(10,190,321,15));
		l_info.setText("(Keywords must be ';' separated!)");
		g_general.setBounds(new Rectangle(1,1,340,225));

		b_finish.addMouseListener(new MouseAdapter()
		{ 
			public void mouseUp(MouseEvent e)
			{
				if(windowType == 0)
				{
					settings[0] = t_fileDescription.getText();
					settings[1] = t_fileKeywords.getText();
				}
				else if(windowType == 1)
				{
					settings[0] = t_fileDescription.getText();
					settings[1] = t_fileKeywords.getText();
					settings[2] = t_fileNameText.getText();
				}
				sShell.dispose();
			}
		});

		b_cancel.addMouseListener(new MouseAdapter()
		{ 
			public void mouseUp(MouseEvent e)
			{    
				sShell.dispose();				
			}
		});

		
	}
}
