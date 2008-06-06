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
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

/**
 * Spartalikku abiinfot sisaldavat akent kirjeldav klass. Abiinformatsioon on
 * <i>hard-coded</i> sellessamas klassis. 
 * @author Taimo Peelo
 */
public class GuiHelpFrm {
	private Shell sShell;	
	
	/**
	 * Loob ja näitab abiakent määratud positsioonil 
	 * @param x akna ülemise vasaku nurga x-koordinaat
	 * @param y akna ülemise vasaku nurga y-koordinaat
	 */
	
	public static void init(int x, int y) {

		org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();		
		GuiHelpFrm thisClass = new GuiHelpFrm();
		thisClass.createSShell(x, y) ;
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
	 * @param y akna ülemise vasaku nurga x-koordinaat
	 */
	private void createSShell(int x, int y)
	{
		sShell = new Shell(SWT.SHELL_TRIM);
		sShell.setText("Help");
		
		sShell.setBackground(new org.eclipse.swt.graphics.Color(org.eclipse.swt.widgets.Display.getDefault(), 255, 255, 255));
		sShell.setLayout(new FillLayout(SWT.VERTICAL));
		
		Text helpText = new Text(sShell, SWT.MULTI | SWT.READ_ONLY | SWT.WRAP | SWT.V_SCROLL);
		helpText.setEditable(false);
						
		helpText.setText
			("Faile saab serverisse uploadida drag and dropiga." +
			"Seejuures peaksid need tekkima ka kliendi vastavatesse kataloogidesse.");
		

		sShell.setSize(new Point(300,180));
		sShell.setLocation(x, y);	
	}
}