/*
 * Created on May 1, 2005
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
package client;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.program.Program;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

/**
 * Help menüü About aken.  
 * @author Kaido Kalda
 * @version 1.0
 */
public class GuiHelpAboutFrm {
	private Shell sShell;
	private Label label;
	private Button button;
	private Label label1;
	private Label label2;
	private Label label3;
	private Label label4;
	private Label label5;
	private Label label6;
	private Label label7;
	private Label label8;
	
	/**
	 * Loob ja näitab abiakent määratud positsioonil 
	 * @param x akna ülemise vasaku nurga x-koordinaat
	 * @param y akna ülemise vasaku nurga y-koordinaat
	 * @param	old_shell	parent shell mille suhtes ollakse modal
	 */
	public static void init(int x, int y, Shell old_shell)
	{

		Display display = Display.getDefault();		
		GuiHelpAboutFrm thisClass = new GuiHelpAboutFrm();
		thisClass.createSShell(x ,y, old_shell) ;
		thisClass.sShell.open();
		
		while (!thisClass.sShell.isDisposed())
		{
			if (!display.readAndDispatch()) display.sleep ();
		}
		thisClass.sShell.dispose();
	}

	/**
	 * Peamine meetod akna laadimiseks. Tekitab kõik elemendid aknale
	 * ning lisatakse kuularid.
	 *
	 * @param x akna ülemise vasaku nurga x-koordinaat
	 * @param y akna ülemise vasaku nurga y-koordinaat
	 * @param old_shell paren shell mille suhtes ollakse modal
	 */
	
	private void createSShell(int x, int y, Shell old_shell)
	{
		sShell = new Shell(old_shell, SWT.BOLD | SWT.APPLICATION_MODAL);
		
		label2 = new Label(sShell, SWT.NONE);
		label3 = new Label(sShell, SWT.NONE);
		label4 = new Label(sShell, SWT.NONE);
		label5 = new Label(sShell, SWT.NONE);
		label6 = new Label(sShell, SWT.NONE);
		label7 = new Label(sShell, SWT.NONE);
		label8 = new Label(sShell, SWT.NONE);
		label = new Label(sShell, SWT.NONE);
		button = new Button(sShell, SWT.NONE);
		label1 = new Label(sShell, SWT.NONE);

		sShell.setText("About");
		sShell.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		label.setBounds(new Rectangle(0,5,184,51));

		label.setImage(ImageRepository.getImage("splashLogo"));
		button.setBounds(new Rectangle(211,187,64,20));
		button.setText("Close");
		button.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		label1.setBounds(new Rectangle(186,0,188,211));

		label1.setImage(ImageRepository.getImage("about"));
		label2.setBounds(new Rectangle(10,60,215,15));
		label2.setText("Code and functionality by:");
		label2.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.BOLD));
		label2.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		label3.setBounds(new Rectangle(10,75,108,15));
		label3.setText("Henri Kuuste");
		label3.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		label3.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		label4.setBounds(new Rectangle(10,130,143,15));
		label4.setText("GUI and graphics by:");
		label4.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		label4.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.BOLD));
		label5.setBounds(new Rectangle(10,145,108,15));
		label5.setText("Kaido Kalda");
		label5.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		label5.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		label6.setBounds(new Rectangle(10,90,108,15));
		label6.setText("Taimo Peelo");
		label6.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		label6.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		label7.setBounds(new Rectangle(10,105,108,15));
		label7.setText("Vilen Looga");
		label7.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		label7.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		label8.setBounds(new Rectangle(10,190,198,15));
		label8.setText("http://ats.cs.ut.ee/oop2005/juss/");
		label8.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		label8.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		label8.setForeground(new Color(Display.getDefault(), 128, 128, 255));
		
		sShell.setSize(375,212);
		sShell.setLocation(x,y);
		
		label8.addMouseListener(new MouseAdapter() { 
			public void mouseUp(MouseEvent e) {    
				Program.launch(label8.getText());
			}
		});
		button.addMouseListener(new MouseAdapter() { 
			public void mouseUp(MouseEvent e) {    
				sShell.dispose();
			}
		});
	
	}
}
