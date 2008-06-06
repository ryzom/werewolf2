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

import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;

import org.eclipse.swt.graphics.Rectangle;

import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Button;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;

/**
 * Klass sisselogimise akna loomiseks. Vajab käivitamiseks SWT jar faile ja dll'e.
 * Linuxi keskkonnas oleks soovitav kasutada GTK jar faile. 
 * Testitud versiooniga 3.0.1.
 * 
 * @author Kaido Kalda
 * @version 1.0
 */
public class GuiLogInFrm {
	/**
	 * 
	 * @uml.property name="sShell"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private org.eclipse.swt.widgets.Shell sShell;

	/**
	 * 
	 * @uml.property name="l_text"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_text;

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
	 * @uml.property name="l_image"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Label l_image;

	/**
	 * 
	 * @uml.property name="b_logIn"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Button b_logIn;

	/**
	 * 
	 * @uml.property name="b_cancel"
	 * @uml.associationEnd multiplicity="(0 1)"
	 */
	private Button b_cancel;

	
	private String[] settings;
	/**
	 * Teostab akna alglaadimise. 
	 * 
	 * @param	x	akna x positsioon.
	 * @param	y	akna y positsioon
	 * @param	parent	aken, mille suhtes on antud aken MODAL
	 * @return 	kasutaja nime ja parooli.
	 * @see	client.GuiMainFrm
	 */
	public static String[] init(int x, int y, Shell parent)
	{

		Display display = Display.getDefault();		
		GuiLogInFrm thisClass = new GuiLogInFrm();
		thisClass.settings = new String[2];
		thisClass.createSShell(x, y, parent) ;
		thisClass.sShell.open();
		
		while (!thisClass.sShell.isDisposed())
		{
			if (!display.readAndDispatch()) display.sleep ();
		}
		return thisClass.settings;	
	}

	/**
	 * Peamine meetod akna laadimiseks. Tekitab kõik elemendid aknale
	 * ning lisatakse kuularid.
	 */
	private void createSShell(int x, int y, Shell parent)
	{
		sShell = new Shell(parent, SWT.DIALOG_TRIM | SWT.APPLICATION_MODAL);		   
		l_text = new Label(sShell, SWT.WRAP);
		l_userName = new Label(sShell, SWT.NONE);
		l_password = new Label(sShell, SWT.NONE);
		t_userName = new Text(sShell, SWT.BORDER);
		t_password = new Text(sShell, SWT.BORDER);
		l_image = new Label(sShell, SWT.NONE);
		b_logIn = new Button(sShell, SWT.NONE);
		b_cancel = new Button(sShell, SWT.NONE);
		
		FormLayout layout = new FormLayout();
		sShell.setLayout(layout);
	
		sShell.setText("Log in");
		sShell.setBackground(new org.eclipse.swt.graphics.Color(org.eclipse.swt.widgets.Display.getDefault(), 255, 255, 255));

		l_text.setText("Please enter your user name and password.");

		l_text.setBackground(new org.eclipse.swt.graphics.Color(org.eclipse.swt.widgets.Display.getDefault(), 255, 255, 255));

		l_userName.setText("User name:");

		l_userName.setBackground(new org.eclipse.swt.graphics.Color(org.eclipse.swt.widgets.Display.getDefault(), 255, 255, 255));
		l_userName.setFont(new org.eclipse.swt.graphics.Font(org.eclipse.swt.widgets.Display.getDefault(), "Verdana", 8, org.eclipse.swt.SWT.NORMAL));

		l_password.setText("Password:");

		l_password.setBackground(new org.eclipse.swt.graphics.Color(org.eclipse.swt.widgets.Display.getDefault(), 255, 255, 255));
		l_password.setFont(new org.eclipse.swt.graphics.Font(org.eclipse.swt.widgets.Display.getDefault(), "Verdana", 8, org.eclipse.swt.SWT.NORMAL));

		t_password.setEchoChar('*');

		l_image.setBounds(new Rectangle(10,10,40,40));
		l_image.setText("Label");

		l_image.setImage(ImageRepository.getImage("login"));

		b_logIn.setText("Log in");


		b_cancel.setText("Cancel");
		
		sShell.setDefaultButton(b_logIn);

		FormData data = new FormData();
		data.left = new FormAttachment(0, 5);
		data.top = new FormAttachment(0, 5);
		l_image.setLayoutData(data);
		data = new FormData();
		data.top = new FormAttachment(l_image, 10);
		data.left = new FormAttachment(0, 5);
		l_userName.setLayoutData(data);
		data = new FormData();
		data.top = new FormAttachment(l_userName, 15);
		data.left = new FormAttachment(0, 5);
		l_password.setLayoutData(data);
		data = new FormData();
		data.top = new FormAttachment(l_image, 10);
		data.right = new FormAttachment(100, -5);
		data.left = new FormAttachment(l_userName, 20);
		t_userName.setLayoutData(data);
		data = new FormData();
		data.right = new FormAttachment(100, -5);
		data.left = new FormAttachment(l_userName, 20);
		data.top = new FormAttachment(l_userName, 15);
		t_password.setLayoutData(data);
		data = new FormData();
		data.bottom = new FormAttachment(100, -5);
		data.right = new FormAttachment(45, 0);
		data.top = new FormAttachment(t_password, 15);
		b_logIn.setLayoutData(data);
		data = new FormData();
		data.bottom = new FormAttachment(100, -5);
		data.left = new FormAttachment(55, 0);
		data.top = new FormAttachment(t_password, 15);
		b_cancel.setLayoutData(data);
		data = new FormData();
		data.left = new FormAttachment(l_image, 20);
		data.right = new FormAttachment(100, -5);
		data.top = new FormAttachment(0, 5);
		data.bottom = new FormAttachment(t_userName, -5);
		l_text.setLayoutData(data);
		
		
	
		sShell.pack();

		sShell.setLocation(x, y);

		
		b_logIn.addMouseListener(new MouseAdapter() { 
			public void mouseUp(MouseEvent e) {onLogin();}

		});
		b_cancel.addMouseListener(new MouseAdapter() { 
			public void mouseUp(MouseEvent e) { onCancel(); }
		});
		
	}
	/**
	 * Meetod, mis kutsutakse välja, kui kasutaja vajutab "Login" nuppu.
	 */
	private void onLogin() {
		if(!t_userName.getText().equals(""))
		{
			settings[0] = t_userName.getText();
		}
		else
		{
			MessageBox messageBox = new MessageBox(sShell, SWT.ICON_WARNING | SWT.OK);
			messageBox.setText("Field emty...");
			messageBox.setMessage("You must type your user name!");
			int response = messageBox.open();
			return;
		}

		if(!t_password.getText().equals(""))
		{
			settings[1] = t_password.getText();
		}
		else
		{
			MessageBox messageBox = new MessageBox(sShell, SWT.ICON_WARNING | SWT.OK);
			messageBox.setText("Field emty...");
			messageBox.setMessage("You must type your password!");
			int response = messageBox.open();
			return;
		}
		sShell.dispose();
	}
	/**
	 * Meetod, mis kutsutakse välja, kui kasutaja vajutab "Cancel" nuppu.
	 */
	private void onCancel() {
		sShell.dispose();
	}
}
