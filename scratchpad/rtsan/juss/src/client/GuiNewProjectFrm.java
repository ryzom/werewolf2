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
/**
 * Klass uue projekti lisamise akna loomiseks.
 * Vajab käivitamiseks SWT jar faile ja dll'e.
 * Linuxi keskkonnas oleks soovitav kasutada GTK jar faile. 
 * Testitud versiooniga 3.0.1.
 * 
 * @author Kaido Kalda
 * @version 1.0
 */
public class GuiNewProjectFrm
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
	 * @uml.property name="g_personal"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Group g_personal;

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

	/**
	 * 
	 * @uml.property name="b_dir"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Button b_dir;

	/**
	 * 
	 * @uml.property name="t_proName"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Text t_proName;

	/**
	 * 
	 * @uml.property name="t_locPath"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Text t_locPath;

	/**
	 * 
	 * @uml.property name="t_server"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Text t_server;

	/**
	 * 
	 * @uml.property name="t_userName"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Text t_userName;

	/**
	 * 
	 * @uml.property name="t_password"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Text t_password;

	/**
	 * 
	 * @uml.property name="l_proName"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_proName;

	/**
	 * 
	 * @uml.property name="l_locPath"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_locPath;

	/**
	 * 
	 * @uml.property name="l_server"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_server;

	/**
	 * 
	 * @uml.property name="l_userName"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_userName;

	/**
	 * 
	 * @uml.property name="l_password"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_password;


	private String[] settings;

	/**
	 * Teostab akna alglaadimise. 
	 * 
	 * @param	x	akna x positsioon.
	 * @param	y	akna y positsioon
	 * @param	parent	aken, mille suhtes on antud aken MODAL
	 * @return 	projekti nime, asukoha kõvakettal ja serveri aadressi.
	 * @see	client.GuiMainFrm
	 */
	
  	public static String[] init(int x, int y, Shell parent)
	{
		Display display = Display.getDefault();		
		GuiNewProjectFrm thisClass = new GuiNewProjectFrm();
		
		thisClass.settings = new String[3];
		thisClass.createSShell(x, y, parent) ;

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

	private void createSShell(int x, int y, Shell parent)
	{
		sShell = new Shell(parent, SWT.APPLICATION_MODAL | SWT.DIALOG_TRIM);

		g_general = new Group(sShell, SWT.NONE);
		g_personal = new Group(sShell, SWT.NONE);

		b_cancel = new Button(sShell, SWT.NONE);
		b_finish = new Button(sShell, SWT.NONE);
		b_dir = new Button(g_general, SWT.NONE);
		   
		t_proName = new Text(g_general, SWT.BORDER);
		t_locPath = new Text(g_general, SWT.BORDER);
		t_server = new Text(g_personal, SWT.BORDER);

		l_proName = new Label(g_general, SWT.NONE);
		l_locPath = new Label(g_general, SWT.NONE);
		l_server = new Label(g_personal, SWT.NONE);


		sShell.setText("Create New Project");
		sShell.setSize(new Point(347,290));
		sShell.setLocation(x, y);

		b_cancel.setBounds(new Rectangle(185,210,70,30));
		b_cancel.setText("Cancel");

		b_finish.setText("Finish");
		b_finish.setBounds(new Rectangle(265,210,70,30));

		g_general.setText("General");
		g_general.setBounds(new Rectangle(1,1,340,120));

		t_proName.setBounds(new Rectangle(10,33,258,17));
		l_proName.setBounds(new Rectangle(10,17,151,15));
		l_proName.setText("Project name:");
		l_proName.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));

		t_locPath.setBounds(new Rectangle(10,73,258,17));
		l_locPath.setBounds(new Rectangle(10,56,150,15));
		l_locPath.setText("Local path:");
		l_locPath.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));

		b_dir.setBounds(new Rectangle(291,73,37,17));
		b_dir.setText("...");

		b_dir.addMouseListener(new MouseAdapter()
		{ 
			public void mouseUp(MouseEvent e)
			{    
				DirectoryDialog directoryDialog = new DirectoryDialog(sShell);
				directoryDialog.setText("New Project Path...");

				String location = directoryDialog.open();
				if(location != null) t_locPath.setText(location);
				
			}
		});

		b_finish.addMouseListener(new MouseAdapter()
		{ 
			public void mouseUp(MouseEvent e)
			{
				if(!t_proName.getText().equals(""))
				{
					settings[0] = t_proName.getText();
				}
				else
				{
					MessageBox messageBox = new MessageBox(sShell, SWT.ICON_WARNING | SWT.OK);
					messageBox.setText("Field emty...");
					messageBox.setMessage("You must type project name!");
					int response = messageBox.open();
					return;
				}

				if(!t_locPath.getText().equals(""))
				{
					settings[1] = t_locPath.getText();
				}
				else
				{
					MessageBox messageBox = new MessageBox(sShell, SWT.ICON_WARNING | SWT.OK);
					messageBox.setText("Field emty...");
					messageBox.setMessage("You must select local path!");
					int response = messageBox.open();
					return;
				}

				if(!t_server.getText().equals(""))
				{
					settings[2] = t_server.getText();
				}
				else
				{
					MessageBox messageBox = new MessageBox(sShell, SWT.ICON_WARNING | SWT.OK);
					messageBox.setText("Field emty...");
					messageBox.setMessage("You must type server address!");
					int response = messageBox.open();
					return;
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

		g_personal.setText("Personal");
		g_personal.setBounds(new Rectangle(1,120,340,80));

		l_server.setBounds(new Rectangle(10,17,300,15));
		l_server.setText("Server (nt \"http://hostname:port/juss/\"):");
		l_server.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		
		t_server.setBounds(new Rectangle(10,33,258,17));
	}
}
