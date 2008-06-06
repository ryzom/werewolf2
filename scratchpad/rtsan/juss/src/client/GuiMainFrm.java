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

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.TreeEvent;
import org.eclipse.swt.events.TreeListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.program.Program;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tray;
import org.eclipse.swt.widgets.TrayItem;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.swt.widgets.Widget;

import shared.CConfig;
import shared.CMetadata;
import shared.FileManager;
import shared.JussFile;
import shared.SearchCriteria;

/**
 * Klass peaakna loomiseks. Vajab käivitamiseks SWT jar faile ja dll'e.
 * Linuxi keskkonnas oleks soovitav kasutada GTK jar faile. 
 * Testitud versiooniga 3.0.1.
 * @author Kaido Kalda
 * @version 1.53
 * 
 * @uml.dependency supplier="client.GuiNewProjectFrm" stereotypes="Omondo::Access"
 * @uml.dependency supplier="client.GuiHelpAboutFrm" stereotypes="Omondo::Access"
 * @uml.dependency supplier="client.GuiHelpFrm" stereotypes="Omondo::Access"
 * @uml.dependency supplier="client.gui_newProjectFrm.init" stereotypes="Omondo::Access"
 */
public class GuiMainFrm {

	/**
	 * 
	 * @uml.property name="sShell"
	 * @uml.associationEnd javaType="Shell" multiplicity="(0 1)"
	 */
	private Shell sShell;

	/**
	 * 
	 * @uml.property name="c_title"
	 * @uml.associationEnd javaType="Composite" multiplicity="(0 1)"
	 */
	private Composite c_title;

	/**
	 * 
	 * @uml.property name="l_titleRightImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_titleRightImg;

	/**
	 * 
	 * @uml.property name="l_titleMiddleImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_titleMiddleImg;

	/**
	 * 
	 * @uml.property name="l_titleLeftImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_titleLeftImg;

	/**
	 * 
	 * @uml.property name="l_menuFile"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_menuFile;

	/**
	 * 
	 * @uml.property name="l_trayButtonImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_trayButtonImg;

	/**
	 * 
	 * @uml.property name="l_closeButtonImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_closeButtonImg;

	/**
	 * 
	 * @uml.property name="l_menuView"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_menuView;

	/**
	 * 
	 * @uml.property name="l_menuOpt"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_menuOpt;

	/**
	 * 
	 * @uml.property name="l_menuHelp"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_menuHelp;

	/**
	 * 
	 * @uml.property name="l_menuEnd"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_menuEnd;

	/**
	 * 
	 * @uml.property name="l_topNurkImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_topNurkImg;

	/**
	 * 
	 * @uml.property name="l_botNurkImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_botNurkImg;

	/**
	 * 
	 * @uml.property name="l_synImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_synImg;

	/**
	 * 
	 * @uml.property name="l_searchImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_searchImg;

	/**
	 * 
	 * @uml.property name="l_projectImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_projectImg;

	/**
	 * 
	 * @uml.property name="l_topRight"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_topRight;

	/**
	 * 
	 * @uml.property name="l_botRight"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_botRight;

	/**
	 * 
	 * @uml.property name="l_logoImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_logoImg;

	/**
	 * 
	 * @uml.property name="l_pbText"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_pbText;

	/**
	 * 
	 * @uml.property name="l_statusText"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_statusText;

	/**
	 * 
	 * @uml.property name="l_line"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_line;

	/**
	 * 
	 * @uml.property name="mainFileMenu"
	 * @uml.associationEnd javaType="Menu" multiplicity="(0 1)"
	 */
	private Menu mainFileMenu;

	/**
	 * 
	 * @uml.property name="mainViewMenu"
	 * @uml.associationEnd javaType="Menu" multiplicity="(0 1)"
	 */
	private Menu mainViewMenu;

	/**
	 * 
	 * @uml.property name="mainOptMenu"
	 * @uml.associationEnd javaType="Menu" multiplicity="(0 1)"
	 */
	private Menu mainOptMenu;

	/**
	 * 
	 * @uml.property name="mainHelpMenu"
	 * @uml.associationEnd javaType="Menu" multiplicity="(0 1)"
	 */
	private Menu mainHelpMenu;

	/**
	 * 
	 * @uml.property name="c_tabComposite"
	 * @uml.associationEnd javaType="Composite" multiplicity="(0 1)"
	 */
	private Composite c_tabComposite;




	
	private static final int PROJECT = 0;
	private static final int SEARCH = 1;
	private static final int META = 2;
	private static final int TRAFFIC = 3;

	/**
	 * 
	 * @uml.property name="tabPictures"
	 * @uml.associationEnd javaType="Label[]" multiplicity="(0 -1)" dimension="1"
	 */
	private Label[] tabPictures;

	/**
	 * 
	 * @uml.property name="tabLabels"
	 * @uml.associationEnd javaType="Label[]" multiplicity="(0 -1)" dimension="1"
	 */
	private Label[] tabLabels;

	/**
	 * 
	 * @uml.property name="tabs"
	 * @uml.associationEnd javaType="Composite[]" multiplicity="(0 -1)" dimension="1"
	 */
	private Composite[] tabs;

	/**
	 * 
	 * @uml.property name="treeSash"
	 * @uml.associationEnd javaType="SashForm" multiplicity="(0 1)"
	 */
	private SashForm treeSash;

	/**
	 * 
	 * @uml.property name="c_transfer"
	 * @uml.associationEnd javaType="Composite" multiplicity="(0 1)"
	 */
	private Composite c_transfer;

	/**
	 * 
	 * @uml.property name="c_status"
	 * @uml.associationEnd javaType="Composite" multiplicity="(0 1)"
	 */
	private Composite c_status;

	/**
	 * 
	 * @uml.property name="c_logo"
	 * @uml.associationEnd javaType="Composite" multiplicity="(0 1)"
	 */
	private Composite c_logo;

	/**
	 * 
	 * @uml.property name="tree"
	 * @uml.associationEnd javaType="Tree" multiplicity="(0 1)"
	 */
	private Tree tree;

	/**
	 * 
	 * @uml.property name="tree1"
	 * @uml.associationEnd javaType="Tree" multiplicity="(0 1)"
	 */
	private Tree tree1;

	/**
	 * 
	 * @uml.property name="l_pbImg"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_pbImg;

	/**
	 * 
	 * @uml.property name="g_search"
	 * @uml.associationEnd javaType="Group" multiplicity="(0 1)"
	 */
	private Group g_search;

	/**
	 * 
	 * @uml.property name="g_results"
	 * @uml.associationEnd javaType="Group" multiplicity="(0 1)"
	 */
	private Group g_results;

	/**
	 * 
	 * @uml.property name="text_search"
	 * @uml.associationEnd javaType="Text" multiplicity="(0 1)"
	 */
	private Text text_search;

	/**
	 * 
	 * @uml.property name="but_search"
	 * @uml.associationEnd javaType="Button" multiplicity="(0 1)"
	 */
	private Button but_search;

	/**
	 * 
	 * @uml.property name="comb_search"
	 * @uml.associationEnd javaType="Combo" elementType="shared.JussFile" multiplicity=
	 * "(0 -1)"
	 */
	private Combo comb_search;

	/**
	 * 
	 * @uml.property name="table_results"
	 * @uml.associationEnd javaType="Table" multiplicity="(0 1)"
	 */
	private Table table_results;

	/**
	 * 
	 * @uml.property name="l_searchName"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_searchName;

	/**
	 * 
	 * @uml.property name="l_searchMethod"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_searchMethod;





	private boolean StartMove = false;
	private int mouseX;
	private int mouseY;
	
	private boolean resizeStartMove = false;
	private int resize_mouseX;
	private int resize_mouseY;

	/**
	 * 
	 * @uml.property name="g_MetaGeneral"
	 * @uml.associationEnd javaType="Group" multiplicity="(0 1)"
	 */
	private Group g_MetaGeneral;

	/**
	 * 
	 * @uml.property name="g_MetaDescription"
	 * @uml.associationEnd javaType="Group" multiplicity="(0 1)"
	 */
	private Group g_MetaDescription;

	/**
	 * 
	 * @uml.property name="t_metaDescription"
	 * @uml.associationEnd javaType="Text" multiplicity="(0 1)"
	 */
	private Text t_metaDescription;

	/**
	 * 
	 * @uml.property name="l_metaFileName"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileName;

	/**
	 * 
	 * @uml.property name="l_metaFileSize"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileSize;

	/**
	 * 
	 * @uml.property name="l_metaFileDate"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileDate;

	/**
	 * 
	 * @uml.property name="l_metaFileNameText"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileNameText;

	/**
	 * 
	 * @uml.property name="l_metaFileSizeText"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileSizeText;

	/**
	 * 
	 * @uml.property name="l_metaFileDateText"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileDateText;

	/**
	 * 
	 * @uml.property name="l_metaFileVersion"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileVersion;

	/**
	 * 
	 * @uml.property name="l_metaFileStatus"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileStatus;

	/**
	 * 
	 * @uml.property name="l_metaFileVersionText"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileVersionText;

	/**
	 * 
	 * @uml.property name="l_metaFileStatusText"
	 * @uml.associationEnd javaType="Label" multiplicity="(0 1)"
	 */
	private Label l_metaFileStatusText;

	/**
	 * 
	 * @uml.property name="status"
	 * @uml.associationEnd javaType="gui_status" multiplicity="(0 1)"
	 */
	private GuiStatus status;

	/**
	 * 
	 * @uml.property name="conf"
	 * @uml.associationEnd javaType="CConfig" multiplicity="(0 1)"
	 */
	private CConfig conf;

	/**
	 * 
	 * @uml.property name="recent"
	 * @uml.associationEnd javaType="java.util.List" elementType="java.lang.String" multiplicity=
	 * "(0 -1)"
	 */
	private java.util.List recent = new java.util.ArrayList();





	private int shellX;
	private int shellY;
	private int doneCounter;

	/**
	 * 
	 * @uml.property name="b_keywords"
	 * @uml.associationEnd javaType="Button" multiplicity="(0 1)"
	 */
	private Button b_keywords;

	/**
	 * 
	 * @uml.property name="b_description"
	 * @uml.associationEnd javaType="Button" multiplicity="(0 1)"
	 */
	private Button b_description;

	/**
	 * 
	 * @uml.property name="b_filename"
	 * @uml.associationEnd javaType="Button" multiplicity="(0 1)"
	 */
	private Button b_filename;

	/**
	 * 
	 * @uml.property name="b_comments"
	 * @uml.associationEnd javaType="Button" multiplicity="(0 1)"
	 */
	private Button b_comments;

	/**
	 * 
	 * @uml.property name="b_search_by_creator"
	 * @uml.associationEnd javaType="Button" multiplicity="(0 1)"
	 */
	private Button b_search_by_creator;

	private Composite c1;
	private Composite c2;
	private Label l_sizer;
	private Rectangle resize;
	/**
	 * Teostab peaakna alglaadimise. 
	 * 
	 * @param	tray	Tray, mida peaaken kasutada saaks
	 * @param	trayItem	TrayItem, mida peaaken kasutada saaks
	 * @param	old_shell	aken, mis antud akna välja kutsus
	 * @param 	gui_conf	konfiguratsioon, mida peaaken kasutab
	 * @see	client.GuiLoaderFrm
	 */

	public static void init(Tray tray, TrayItem trayItem, Shell old_shell,
			CConfig gui_conf) {

		old_shell.dispose();
		Display display = Display.getDefault();
		final GuiMainFrm thisClass = new GuiMainFrm();

		thisClass.conf = gui_conf;
		thisClass.createSShell();
		thisClass.sShell.open();

		thisClass.createFileMenu();
		thisClass.createViewMenu();
		thisClass.createOptMenu();
		thisClass.createHelpMenu();
		thisClass.createFolderTreeMenu();
		thisClass.createFileTreeMenu();
		thisClass.startTransferTimer();

		thisClass.createTrayMenu(tray, trayItem);
		thisClass.l_statusText.setText("Done...");
		
		while (!thisClass.sShell.isDisposed()) {
			if (!display.readAndDispatch())
				display.sleep();
		}
		
		thisClass.upDateConf();
		JussClient.shutdown();
		display.dispose();
		System.exit(1);
	}

	/**
	 * Luuakse parema hiireklõpsu menüü faili puule
	 */
	
	private void createFileTreeMenu() {

		final Menu fileMenu = new Menu(sShell, SWT.PUSH);

		final MenuItem fileOpen = new MenuItem(fileMenu, SWT.PUSH);
		fileOpen.setText("Open");
		
		createCommonRMBMenu(fileMenu);
		
		final MenuItem fileUploadExisting = new MenuItem(fileMenu, SWT.PUSH);
		fileUploadExisting.setText("Upload This File");

		fileUploadExisting.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e)
			{	
				TreeItem[] items = tree.getSelection();
				if(items.length < 1)
					return;
				CConnection con = getConnection(items[0]);
				JussFile jf = con.getFile(getFileId(tree1.getSelection()[0]));
				
				String localFilePath = "";
				
				Long parent = jf.getParent();
				while(parent != null)
				{
					final JussFile dir = con.getFile(parent);
					
					if(!dir.getName().equals("/"))
						localFilePath = dir.getName() + "/" + localFilePath;
					parent = con.getFile(parent).getParent();
				}
				
				localFilePath = con.getRoot() + "/" + localFilePath + jf.getName();
				
				File fileToAdd = new File(localFilePath);
				
				Rectangle r1 = sShell.getBounds();
				int sX = r1.x + (r1.width / 2) - (347 / 2);
				int sY = r1.y + (r1.height / 2) - (275 / 2);

				String[] settings = GuiImportFileFrm.init(fileToAdd, 0, sX, sY, sShell);
				
				doUpload(settings, fileToAdd);
				
			}
		});
		final MenuItem fileMeta = new MenuItem(fileMenu, SWT.PUSH);
		fileMeta.setText("Meta");

		final MenuItem fileDownload = new MenuItem(fileMenu, SWT.PUSH);
		fileDownload.setText("Download");
		
		final MenuItem deleteProject = new MenuItem(fileMenu, SWT.PUSH);
		deleteProject.setText("Delete");
		
		deleteProject.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e)
			{
				System.out.println("Delete: " + tree1.getSelection()[0].getText());
			}
		});
		
		fileOpen.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e)
			{
				doFileOpen();
			}
		});
		
		fileDownload.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				TreeItem[] items = tree1.getSelection();
				if(items.length < 1)
					return;
				doDownload(items[0]);
			}
		});
		
		fileMeta.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				TreeItem[] items = tree1.getSelection();
				if(items.length < 1)
					return;
				showMeta(items[0]);
			}
		});
		
		tree1.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e)
			{
				if (e.button == 3 && tree1.getItemCount() > 0) {
					final TreeItem[] t = tree1.getSelection();
					
					if(tree1.getSelectionCount() > 0)
						if (t[0].getParentItem() == null)
						{
							TreeItem[] items = tree.getSelection();
							if(items.length < 1)
								return;
							CConnection con = getConnection(items[0]);
							JussFile jf = con.getFile(getFileId(t[0]));
							
							if(!jf.getExists())
							{
								fileUploadExisting.setEnabled(false);
								fileDownload.setEnabled(true);
							}
							else
							{
								fileUploadExisting.setEnabled(true);
								fileDownload.setEnabled(false);
							}
							
							fileMenu.setVisible(true);
				}
			}
			}
		});
	}
	/**
	 * Meetod, mis avab etteantud faili kasutades faili laientit
	 * vastava programmi leidmiseks.
	 */
	private void doFileOpen() {
		TreeItem[] t = tree1.getSelection();
		if(t.length < 1)
			return;
		TreeItem[] t2 = tree.getSelection();
		if(t2.length < 1)
			return;
		CConnection con = getConnection(t2[0]);
		Long id = getFileId(t[0]);
		JussFile jf = con.getFile(id);
		if(!jf.getExists())
			return;
		String ext = jf.getName();
		ext = ext.substring(ext.lastIndexOf('.') + 1);
		Program p = Program.findProgram(ext);
		if(p != null) p.execute(con.getRoot() + con.getDBManager().getStringPath(jf));
		else
		{
			MessageBox messageBox = new MessageBox(sShell, SWT.ICON_WARNING	| SWT.OK);
			messageBox.setText("Can't open file!");
			messageBox.setMessage("No program associated with that kind of file type!");

			int response = messageBox.open();
		}
	}
	/**
	 * Teostab faili allalaadimise kliendi kataloogi.
	 * @param	item	selle järgi leitakse allalaaditav fail puust.
	 */
	private void doDownload(TreeItem item) {
		TreeItem[] items = tree.getSelection();
		if(items.length < 1)
			return;
		CConnection con = getConnection(items[0]);
		JussClient.downloadFile(getFileId(item), con.getRoot());
	}
	/**
	 * Avatakse Meta aken ning asendatakse väljad faili Meta informatsiooniga.
	 * @param	item	selle järgi leitakse faili informatsioon.
	 */
	private void showMeta(TreeItem item) {
		TreeItem[] items = tree.getSelection();
		if(items.length < 1)
			return;
		CConnection con = getConnection(items[0]);
		Long id = getFileId(item);
		CMetadata meta = con.getMetadata(id);
		System.out.println(meta.getDescription());
		JussFile jf = con.getFile(id);
		
		l_metaFileNameText.setText(jf.getName());
		l_metaFileNameText.pack();
		
		Rectangle rec1 = l_metaFileNameText.getBounds();
		int kaugus = rec1.width + rec1.x;
		
		if(kaugus > 240)
		{
			l_metaFileVersion.setBounds(kaugus + 10, 15, 70, 15);
			l_metaFileStatus.setBounds(kaugus + 10, 35, 70, 15);
			l_metaFileVersionText.setBounds(80 + kaugus, 15, 140, 15);
			l_metaFileStatusText.setBounds(80 + kaugus, 35, 140, 15);
		}
		
		l_metaFileVersionText.setText("" + jf.getVersion());
		l_metaFileStatusText.setText((jf.getExists() ? "Local"
				: "Remote"));
		l_metaFileSizeText.setText(jf.getHumanReadableSize());
		
		l_metaFileDateText.setText(jf.getModtime().toString());
		t_metaDescription.setText(meta.getDescription());
		setTab(META);
	}
	/**
	 * Tekitatakse parema hiireklõpsu menüü kataloogi puule.
	 */
	private void createFolderTreeMenu() {

		final Menu folderMenu = new Menu(sShell, SWT.PUSH);

		final Menu projectMenu = new Menu(sShell, SWT.PUSH);

		createCommonRMBMenu(projectMenu);
		createCommonRMBMenu(folderMenu);
		createFolderRMBMenu(projectMenu);
		createFolderRMBMenu(folderMenu);
		
		final MenuItem deleteProject = new MenuItem(folderMenu, SWT.PUSH);
		deleteProject.setText("Delete");
		
		deleteProject.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e)
			{
				System.out.println("Delete: " + tree.getSelection()[0].getText());
			}
		});
		
		final MenuItem meta = new MenuItem(folderMenu, SWT.PUSH);
		meta.setText("Meta");

		meta.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				TreeItem[] items = tree.getSelection();
				if(items.length < 1)
					return;
				showMeta(items[0]);
			}
		});

		final MenuItem closeProject = new MenuItem(projectMenu, SWT.PUSH);
		closeProject.setText("Close");

		closeProject.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				l_statusText.setText("Closing project...");
				TreeItem item = tree.getSelection()[0];
				JussClient.closeConnection(getConnection(item).getRoot());
				item.dispose();
				clear(tree1);
				initSearch();
			}
		});
		tree.addTreeListener(new TreeListener() {
			public void treeCollapsed(TreeEvent e) {
				TreeItem a = (TreeItem) (e.item);

				if (a.getParentItem() != null)
					a.setImage(ImageRepository.getImage("folderClosed"));
			}

			public void treeExpanded(TreeEvent e) {
				TreeItem a = (TreeItem) (e.item);

				if (a.getParentItem() != null)
					a.setImage(ImageRepository.getImage("folderOpen"));
			}
		});

		tree.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {

				if (e.button == 3 && tree.getItemCount() > 0) {
					final TreeItem[] t = tree.getSelection();
					
					if (t[0].getParentItem() == null)
						projectMenu.setVisible(true);
					else
						folderMenu.setVisible(true);
				}
			}
		});

		tree.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				TreeItem t = (TreeItem) e.item;
				getContext(t);
			}
		});
	}
	/**
	 * Abimeetod korduvate menüü elementide loomiseks kataloogipuul
	 * (klõps projektil või kaustal).
	 * @param	m	viit kataloogipuu menüüle.
	 * @see	createFolderTreeMenu
	 */
	private void createFolderRMBMenu(Menu m) {
		final MenuItem newDir = new MenuItem(m, SWT.PUSH);
		newDir.setText("Make new directory");

		newDir.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				makeDirectory();
			}
		});

	}
	/**
	 * Abimeetod korduvate menüü elementide loomiseks
	 * (klõps kataloogi- või failipuus).
	 * @param	m	viit puu menüüle.
	 * @see	createFolderTreeMenu
	 * @see	createFilesTreeMenu
	 */
	private void createCommonRMBMenu(Menu m) {
		final MenuItem refreshProject = new MenuItem(m, SWT.PUSH);
		refreshProject.setText("Refresh");
		
		final MenuItem fileUpload = new MenuItem(m, SWT.PUSH);
		fileUpload.setText("Upload New File");
		
		fileUpload.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e)
			{
				String[] filterExtensions = {"*.*"};
				FileDialog fileDialog = new FileDialog(sShell, SWT.OPEN);
				fileDialog.setText("Upload file...");
				fileDialog.setFilterExtensions(filterExtensions);
				String selectedFile = fileDialog.open();
				
				if(selectedFile == null) return;
				File fileToAdd = new File(selectedFile);
				
				Rectangle r1 = sShell.getBounds();
				int sX = r1.x + (r1.width / 2) - (347 / 2);
				int sY = r1.y + (r1.height / 2) - (275 / 2);

				String[] settings = GuiImportFileFrm.init(fileToAdd, 0, sX, sY, sShell);
				
				doUpload(settings, fileToAdd);
				
			}
		});

		refreshProject.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				doFolderRefresh();
			}
		});
	}
	/**
	 * Meetod kausta loomiseks projekti või teise kausta alla.
	 */
	private void makeDirectory() {
		TreeItem[] items  = tree.getSelection();
		if(items.length < 1)
			return;
		TreeItem item = tree.getSelection()[0];
		CConnection con = getConnection(item);

		Rectangle r1 = sShell.getBounds();
		int sX = r1.x + (r1.width / 2) - (347 / 2);
		int sY = r1.y + (r1.height / 2) - (275 / 2);

		String[] settings = GuiImportFileFrm.init(null, 1, sX, sY, sShell);

		if (settings[0] == null)
			return;
		else {
			CMetadata meta = new CMetadata("juss", settings[0]);
			meta.setKeywords(settings[1]);
			con.createFolder(getId(item), settings[2], meta);
			getContext(tree.getSelection()[0]);
		}
	}
	
	/**
	 * Värskendab failipuu elemente.
	 */
	private void doFolderRefresh() {
		TreeItem[] items  = tree.getSelection();
		if(items.length < 1)
			return;
		TreeItem item = tree.getSelection()[0];
		getContext(item);
	}
	/**
	 * Loob "File" menüü
	 */
	private void createFileMenu() {
		final MenuItem miFileOpen = new MenuItem(mainFileMenu, SWT.PUSH);
		miFileOpen.setText("Open");

		final MenuItem miFileSep = new MenuItem(mainFileMenu, SWT.SEPARATOR);
		final MenuItem miFileRecent = new MenuItem(mainFileMenu, SWT.CASCADE);
		
		final Menu FileRecentMenu = new Menu(sShell, SWT.DROP_DOWN);
		miFileRecent.setMenu(FileRecentMenu);
		
		int i = 0;

		while (conf.getStringValue("recent" + i) != null) {
			String recentStr = conf.getStringValue("recent" + i);

			if (!recent.contains(recentStr)) {
				recent.add(recentStr);

				final MenuItem miFileSubRecent = new MenuItem(FileRecentMenu,
						SWT.PUSH);
				miFileSubRecent.setText(conf.getStringValue("recent" + i));

				miFileSubRecent.addListener(SWT.Selection, new Listener() {
					public void handleEvent(Event e) {
						l_statusText.setText("Opening project...");
						String conName = JussClient.openConnection(miFileSubRecent
								.getText());
						addConnection(miFileSubRecent.getText(), conName);
						
					}
				});
			}
			i++;
		}

		if (i == 0) {
			miFileRecent.setText("No recent files...");
			miFileRecent.setEnabled(false);
		}
		else
		{
			miFileRecent.setText("Recent files");
			miFileRecent.setEnabled(true);
		}

		final MenuItem miFileSep1 = new MenuItem(mainFileMenu, SWT.SEPARATOR);

		final MenuItem miFileExit = new MenuItem(mainFileMenu, SWT.PUSH);
		miFileExit.setText("Exit");

		miFileOpen.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				DirectoryDialog directoryDialog = new DirectoryDialog(sShell);
				directoryDialog.setText("Select Project Path...");

				String location = directoryDialog.open();
				l_statusText.setText("Opening project...");
				
				if (location != null) {
					byte[] convertPath = location.getBytes();
					String temp = new String();

					for (int i = 0; i < convertPath.length; i++) {
						if (convertPath[i] == 92)
							convertPath[i] = 47;
						if (temp == null)
							temp = String.valueOf((char) convertPath[i]);
						else
							temp = temp + String.valueOf((char) convertPath[i]);
					}

					String conName = JussClient.openConnection(temp);
					addConnection(temp, conName);
				}
			}
		});

		miFileExit.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				l_statusText.setText("Shutting down...");
				
				MessageBox messageBox = new MessageBox(sShell, SWT.ICON_QUESTION
						| SWT.YES | SWT.NO);
				messageBox.setText("Shutting down!");
				messageBox.setMessage("Are you sure that you want to exit this program?");

				int response = messageBox.open();
				if(response == SWT.NO)
				{
					l_statusText.setText("Idle...");
					return;
				}
				
				sShell.dispose();
			}
		});

		Rectangle rec = sShell.getBounds();
		mainFileMenu.setLocation(rec.x, rec.y + 42);
	}
	/**
	 * Uuendatakse konfiguratsioonifaili programmist väljumisel.
	 */
	private void upDateConf() {
		for (int i = 0; i < 5 && i < recent.size(); i++) {
			conf.setValue("recent" + i, (String) recent.get(recent.size() - i
					- 1));
		}

		conf.setValue("pointX", shellX);
		conf.setValue("pointY", shellY);
		conf.save();
	}
	/**
	 * Abieetod faili nimekirja saamiseks andmebaasist.
	 * @param	item	failipuu element, mille järgi saadakse nimekiri.
	 * @see	getContext
	 * 
	 */
	private void getContext(TreeItem item) {
		getContext(item, true);
	}
	
	/**
	 * Meetod faili nimekirja saamiseks andmebaasist.
	 * @param	item	failipuu element, mille järgi saadakse nimekiri.
	 * @param	first	kontrollitakse kas puu on avatud.
	 * @see	getContext
	 */
	
	private void getContext(TreeItem item, boolean first) {
		
		l_statusText.setText("Getting context...");
		
		CConnection con = getConnection(item);
		// ajutine lahendus TODO: FIX IT!
		con.getDBManager().closeSession();
		java.util.List list = con.getListing(getId(item), true, FileManager.FILES | FileManager.FOLDERS);
		boolean expanded = item.getExpanded();

		clear(item);

		if (first) {
			clear(tree1);
		}

		for (int i = 0; i < list.size(); i++) {
			JussFile listItem = (JussFile) (list.get(i));
			TreeItem ti;
			if (!listItem.isFolder()) {
				if (!first)
					continue;
				ti = new TreeItem(tree1, SWT.NONE);
				ti.setImage(ImageRepository.getImage("file"));
				if (listItem.getExists())
					ti.setForeground(new Color(Display.getCurrent(), 255, 128,
							20));
				
				CMetadata meta = con.getMetadata(listItem.getId());
				
				TreeItem tiCreator = new TreeItem(ti, SWT.NONE);
				tiCreator.setImage(ImageRepository.getImage("info"));
				tiCreator.setText("Creator: " + meta.getCreator());
				
				TreeItem tiDate = new TreeItem(ti, SWT.NONE);
				tiDate.setImage(ImageRepository.getImage("info"));
				tiDate.setText("Date: " + meta.getModTime());
				
				TreeItem tiVer = new TreeItem(ti, SWT.NONE);
				tiVer.setImage(ImageRepository.getImage("info"));
				tiVer.setText("Version: " + listItem.getVersion());

			} else {
				ti = new TreeItem(item, SWT.NONE);
				ti.setImage(ImageRepository.getImage("folderClosed"));
			}
			ti.setText(listItem.getName());
			ti.setData(listItem.getId());
			if (listItem.isFolder()) {
				if (first)
					getContext(ti, false);
			}
		}
		item.setExpanded(expanded);
		
		l_statusText.setText("Done...");
	}
	/**
	 * Meetod elemendi kustutamiseks puust.
	 * @param	item	puu element
	 * @see	clear()
	 */
	private void clear(TreeItem item) {
		clear(item.getItems());
	}
	/**
	 * Meetod puu tühjendamiseks.
	 * @param	tree	kas faili- või kataloogipuu.
	 * @see	clear()
	 */
	private void clear(Tree tree) {
		clear(tree.getItems());
	}
	/**
	 * Meetod elementide kustutamiseks puust.
	 * @param	items	elementide masiiv -> alamelemendid.
	 * @see	clear()
	 */
	private void clear(TreeItem[] items) {
		for (int i = 0; i < items.length; i++) {
			items[i].dispose();
		}
	}
	/**
	 * Tagastab faili id, võttes selle etteantud elemendilt.
	 * @param	item	puu element.
	 * @return	faili id.
	 */
	private Long getFileId(TreeItem item) {
		Object temp = item.getData();
		if (temp instanceof Long)
			return (Long) temp;
		return null;
	}
	/**
	 * Tagastab kataloogi id, võttes selle etteantud elemendilt.
	 * @param	item	puu element.
	 * @return kataloogi id.
	 */
	private Long getId(TreeItem item) {
		if (item.getParentItem() == null)
			return new Long(1);
		else {
			Object temp = item.getData();
			if (temp instanceof Long)
				return (Long) temp;
			return null;
		}
	}
	/**
	 * Tagastab projektile omase CConnection klassi.
	 * @param	item	puu element (kõige esimene -> projekt).
	 * @return	projekti ühenduse informatriooni kandev klass (CConnection).
	 * @see	client.CConnection
	 */
	private CConnection getConnection(TreeItem item) {
		TreeItem temp;
		do {
			temp = item;
			item = temp.getParentItem();
		} while (item != null);
		return (CConnection) temp.getData();
	}
	/**
	 * Projekti avamise põhimeetod. Samas meetodis toimub ka projekti sisselogimine.
	 * @param	path	projekti asukoht kohalikul kettal.
	 * @param	conName	projekti nimi.
	 */
	private void addConnection(String path, String conName) {
		path = FileManager.fixSlashes(path);
		
		if (conName == null) {
			MessageBox messageBox = new MessageBox(sShell, SWT.ICON_WARNING
					| SWT.OK);
			messageBox.setText("Error while opening project");
			messageBox.setMessage("Selected path not valid!");

			int response = messageBox.open();
			return;
		}

		CConnection con = JussClient.getConnection(path);

		if (con.isValid())
			System.out.println("Opened connection " + conName);
		else {
			MessageBox messageBox = new MessageBox(sShell, SWT.ICON_WARNING
					| SWT.OK);
			messageBox.setText("Error");
			messageBox
					.setMessage("Error while opening project! Please try again.");
			int response = messageBox.open();

			return;
		}

		Rectangle r1 = sShell.getBounds();
		int sX = r1.x + (r1.width / 2) - (300 / 2);
		int sY = r1.y + (r1.height / 2) - (180 / 2);

		TreeItem ti = new TreeItem(tree, SWT.NONE);
		ti.setText(con.getName());
		ti.setData(con);
		ti.setExpanded(false);
		ti.setImage(ImageRepository.getImage("offline"));
		TreeItem[] tl = { ti };
		tree.setSelection(tl);
		getContext(ti);
		initSearch();
		if (!recent.contains(con.getRoot()))
			recent.add(con.getRoot());
		doLogin(ti);
	}
	/**
	 * Luuakse "View" menüü.
	 */
	private void createViewMenu() {
		final MenuItem miViewTabOpt = new MenuItem(mainViewMenu, SWT.PUSH);
		miViewTabOpt.setText("Tab options");
		miViewTabOpt.setEnabled(false);

		final MenuItem miViewSep = new MenuItem(mainViewMenu, SWT.SEPARATOR);
		final MenuItem miViewPref = new MenuItem(mainViewMenu, SWT.PUSH);
		miViewPref.setText("Preferences");

		miViewTabOpt.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				/* TODO
				 * siia tuleb rohkem valikuid, kuidas akna v2limust muuta
				 */
			}
		});

		miViewPref.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				//open pref window
			}
		});

		Rectangle rec = sShell.getBounds();
		mainViewMenu.setLocation(rec.x + 89, rec.y + 42);
	}
	/**
	 * Luuakse "Options" menüü.
	 */
	private void createOptMenu() {
		final MenuItem miViewTabOpt = new MenuItem(mainOptMenu, SWT.PUSH);
		miViewTabOpt.setText("options");
		miViewTabOpt.setEnabled(false);

		Rectangle rec = sShell.getBounds();
		mainOptMenu.setLocation(rec.x + 180, rec.y + 42);
	}
	/**
	 * Luuakse "Help" menüü.
	 */
	private void createHelpMenu() {
		final MenuItem miHelpContents = new MenuItem(mainHelpMenu, SWT.PUSH);
		miHelpContents.setText("Help Contents");

		Rectangle rec = sShell.getBounds();
		mainHelpMenu.setLocation(rec.x + 267, rec.y + 42);

		miHelpContents.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				Rectangle r1 = sShell.getBounds();
				int sX = r1.x + (r1.width / 2) - (300 / 2);
				int sY = r1.y + (r1.height / 2) - (180 / 2);

				GuiHelpFrm.init(sX, sY);
			}
		});
		
		final MenuItem miHelpAbout= new MenuItem(mainHelpMenu, SWT.PUSH);
		miHelpAbout.setText("About");

		miHelpAbout.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				Rectangle r1 = sShell.getBounds();
				int sX = r1.x + (r1.width / 2) - (300 / 2);
				int sY = r1.y + (r1.height / 2) - (180 / 2);

				GuiHelpAboutFrm.init(sX, sY, sShell);
			}
		});
	}
	/**
	 * Luuakse menüü süsteemi tray'le.
	 */
	private void createTrayMenu(Tray tray, TrayItem trayItem) {
		final Menu trayMenu = new Menu(sShell, SWT.POP_UP);

		final MenuItem miShow = new MenuItem(trayMenu, SWT.PUSH);
		miShow.setText("Show");

		miShow.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				sShell.setVisible(true);
			}
		});

		final MenuItem miExit = new MenuItem(trayMenu, SWT.PUSH);
		miExit.setText("Exit");

		miExit.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e) {
				l_statusText.setText("Shutting down...");
				
				MessageBox messageBox = new MessageBox(sShell, SWT.ICON_QUESTION
						| SWT.YES | SWT.NO);
				messageBox.setText("Shutting down!");
				messageBox.setMessage("Are you sure that you want to exit this program?");

				int response = messageBox.open();
				if(response == SWT.NO)
				{
					l_statusText.setText("Idle...");
					return;
				}
				
				sShell.dispose();
			}
		});

		trayItem.addListener(SWT.MenuDetect, new Listener() {
			public void handleEvent(Event event) {
				if (sShell.getVisible())
					miShow.setEnabled(false);
				else
					miShow.setEnabled(true);

				Point point = sShell.getDisplay().getCursorLocation();

				trayMenu.setLocation(point.x, point.y);
				trayMenu.setVisible(true);
			}
		});
	}
	/**
	 * Peamine meetod akna laadimiseks. Tekitab kõik elemendid aknale
	 * ning lisatakse kuularid.
	 */
	private void createSShell() {
		sShell = new Shell(SWT.BOLD);

		tabs = new Composite[TRAFFIC + 1];
		tabLabels = new Label[TRAFFIC + 1];
		tabPictures = new Label[TRAFFIC + 1];

		c_tabComposite = new Composite(sShell, SWT.NONE);
		tabs[PROJECT] = new Composite(sShell, SWT.NONE);
		tabs[SEARCH] = new Composite(sShell, SWT.NONE);
		tabs[META] = new Composite(sShell, SWT.NONE);
		tabs[TRAFFIC] = new ScrolledComposite(sShell, SWT.H_SCROLL | SWT.V_SCROLL | SWT.BORDER);
		
		treeSash = new SashForm(tabs[PROJECT],SWT.NONE);
		
		c_transfer = new Composite(tabs[TRAFFIC], SWT.NONE);
		c_status = new Composite(sShell, SWT.NONE);
		FillLayout fillLayout = new FillLayout();
	   
		g_search = new Group(tabs[SEARCH], SWT.NONE);
		g_results = new Group(tabs[SEARCH], SWT.NONE);
		g_MetaGeneral = new Group(tabs[META], SWT.NONE);
		g_MetaDescription = new Group(tabs[META], SWT.NONE);
		
		c_title = new Composite(sShell, SWT.NONE);
		
		l_trayButtonImg = new Label(c_title, SWT.NONE);
		l_closeButtonImg = new Label(c_title, SWT.NONE);
		l_titleRightImg = new Label(c_title, SWT.NONE);
		l_titleLeftImg = new Label(c_title, SWT.NONE);
		l_titleMiddleImg = new Label(c_title, SWT.NONE);
		
		
		l_menuFile = new Label(sShell, SWT.NONE);
		l_menuView = new Label(sShell, SWT.NONE);
		l_menuOpt = new Label(sShell, SWT.NONE);
		l_menuHelp = new Label(sShell, SWT.NONE);
		l_menuEnd = new Label(sShell, SWT.NONE);
		l_synImg = new Label(sShell, SWT.NONE);
		l_searchImg = new Label(sShell, SWT.NONE);
		l_projectImg = new Label(sShell, SWT.NONE);
		l_botNurkImg = new Label(sShell, SWT.NONE);
		l_topNurkImg = new Label(sShell, SWT.NONE);
		
		l_botRight = new Label(sShell, SWT.NONE);
		l_pbImg = new Label(c_status, SWT.NONE);
		l_pbText = new Label(c_status, SWT.NONE);
		l_statusText = new Label(c_status, SWT.NONE);

		tabLabels[PROJECT] = new Label(c_tabComposite, SWT.CENTER);
		tabLabels[SEARCH] = new Label(c_tabComposite, SWT.CENTER);
		tabLabels[META] = new Label(c_tabComposite, SWT.CENTER);
		tabLabels[TRAFFIC] = new Label(c_tabComposite, SWT.CENTER);
		tabPictures[PROJECT] = new Label(c_tabComposite, SWT.NONE);
		tabPictures[SEARCH] = new Label(c_tabComposite, SWT.NONE);
		tabPictures[META] = new Label(c_tabComposite, SWT.NONE);
		tabPictures[TRAFFIC] = new Label(c_tabComposite, SWT.NONE);

		l_metaFileName = new Label(g_MetaGeneral, SWT.NONE);
		l_metaFileSize = new Label(g_MetaGeneral, SWT.NONE);
		l_metaFileDate = new Label(g_MetaGeneral, SWT.NONE);
		l_metaFileNameText = new Label(g_MetaGeneral, SWT.NONE);
		l_metaFileSizeText = new Label(g_MetaGeneral, SWT.NONE);
		l_metaFileDateText = new Label(g_MetaGeneral, SWT.NONE);
		l_metaFileVersion = new Label(g_MetaGeneral, SWT.NONE);
		l_metaFileStatus = new Label(g_MetaGeneral, SWT.NONE);
		l_metaFileVersionText = new Label(g_MetaGeneral, SWT.NONE);
		l_metaFileStatusText = new Label(g_MetaGeneral, SWT.NONE);

		t_metaDescription = new Text(g_MetaDescription, SWT.BORDER
				| SWT.V_SCROLL | SWT.WRAP);

		mainFileMenu = new Menu(sShell, SWT.POP_UP);
		mainViewMenu = new Menu(sShell, SWT.POP_UP);
		mainOptMenu = new Menu(sShell, SWT.POP_UP);
		mainHelpMenu = new Menu(sShell, SWT.POP_UP);

		c_logo = new Composite(sShell , SWT.NONE);
		c_logo.setLayout(new FormLayout());
		c_logo.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		c_logo.setBounds(320,21,258,56);
		
		l_line = new Label(c_logo, SWT.NONE);
		l_topRight = new Label(c_logo, SWT.NONE);
		l_logoImg = new Label(c_logo, SWT.NONE);
		
		FormData d1 = new FormData();
		d1.left = new FormAttachment(0,0);
		d1.right = new FormAttachment(100,0);
		
		l_line.setLayoutData(d1);
		l_line.setImage(ImageRepository.getImage("line"));
		
		d1 = new FormData();
		d1.top = new FormAttachment(0,6);
		d1.right = new FormAttachment(100, -10);
		d1.height = 47;
		d1.width = 42;
		
		l_logoImg.setLayoutData(d1);
		l_logoImg.setImage(ImageRepository.getImage("mainLogo"));
		
		d1 = new FormData();
		d1.top = new FormAttachment(0,18);
		d1.right = new FormAttachment(100, -136);
		d1.height = 38;
		d1.width = 122;
		
		l_topRight.setLayoutData(d1);
		l_topRight.setImage(ImageRepository.getImage("nurkTopRight"));
		
		c_logo.layout();

		tree = new Tree(treeSash, SWT.NONE);
		tree1 = new Tree(treeSash, SWT.NONE);

		text_search = new Text(g_search, SWT.BORDER);
		but_search = new Button(g_search, SWT.NONE);
		l_searchName = new Label(g_search, SWT.NONE);
		l_searchMethod = new Label(g_search, SWT.NONE);
		
		b_keywords = new Button(g_search, SWT.CHECK);
		b_description = new Button(g_search, SWT.CHECK);
		b_filename = new Button(g_search, SWT.CHECK);
		b_comments = new Button(g_search, SWT.CHECK);
		b_search_by_creator = new Button(g_search, SWT.CHECK);

		table_results = new Table(g_results, SWT.BORDER);

		TableColumn tc_resultName = new TableColumn(table_results, SWT.LEFT);
		TableColumn tc_resultSize = new TableColumn(table_results, SWT.LEFT);
		TableColumn tc_resultDate = new TableColumn(table_results, SWT.LEFT);
		TableColumn tc_resultVersion = new TableColumn(table_results, SWT.LEFT);

		c1 = new Composite(sShell, SWT.NONE);		   

		c1.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		c1.setBounds(0, 0, 578, 320);
		c2 = new Composite(sShell, SWT.NONE);		   
		c2.setBackground(new Color(Display.getDefault(), 0, 0, 0));
		c2.setBounds(0, 0, 579, 321);
		
		l_sizer = new Label(sShell, SWT.NONE);
		l_sizer.setBounds(new Rectangle(578 - 18, 320 - 18, 18, 18));
		l_sizer.setImage(ImageRepository.getImage("sizer"));
		
		comb_search = new Combo(g_search, SWT.READ_ONLY);

		tabs[META]
				.setBackground(new Color(Display.getDefault(), 224, 223, 227));
		tabs[META].setBounds(new Rectangle(95, 77, 475, 195));
		
		tabs[META].setLayout(new FormLayout());
		
		d1 = new FormData();
		d1.left = new FormAttachment(0,1);
		d1.right = new FormAttachment(100,-1);
		d1.top = new FormAttachment(0, 1);
		d1.height = 75;
		
		g_MetaGeneral.setLayoutData(d1);
		g_MetaGeneral.setBackground(new Color(Display.getDefault(), 255, 255,
				255));
		g_MetaGeneral.setText("General");
		
		d1 = new FormData();
		d1.left = new FormAttachment(0,1);
		d1.right = new FormAttachment(100,-1);
		d1.top = new FormAttachment(0, 92);
		d1.bottom = new FormAttachment(100, -1);
		d1.height = 125;
		
		g_MetaDescription.setLayoutData(d1);
		g_MetaDescription.setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		g_MetaDescription.setText("Description");
		
		FillLayout fillLayout1 = new FillLayout();		   
		
		g_MetaDescription.setLayout(fillLayout1);
		fillLayout1.marginHeight = 1;
		fillLayout1.marginWidth = 1;

		tabs[META].layout();
		
		l_metaFileName.setBounds(new Rectangle(10, 15, 70, 15));
		l_metaFileName.setText("Name:");
		l_metaFileName.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));
		l_metaFileName.setBackground(new Color(Display.getDefault(), 255, 255,
				255));

		l_metaFileSize.setBounds(new Rectangle(10, 35, 70, 15));
		l_metaFileSize.setText("Size:");
		l_metaFileSize.setBackground(new Color(Display.getDefault(), 255, 255,
				255));
		l_metaFileSize.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		l_metaFileDate.setBounds(new Rectangle(10, 55, 70, 15));
		l_metaFileDate.setText("Date:");
		l_metaFileDate.setBackground(new Color(Display.getDefault(), 255, 255,
				255));
		l_metaFileDate.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		l_metaFileNameText.setBounds(new Rectangle(80, 15, 140, 15));
		l_metaFileNameText.setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		l_metaFileNameText.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.BOLD));

		l_metaFileSizeText.setBounds(new Rectangle(80, 35, 140, 15));
		l_metaFileSizeText.setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		l_metaFileSizeText.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		l_metaFileDateText.setBounds(new Rectangle(80, 55, 210, 15));
		l_metaFileDateText.setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		l_metaFileDateText.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		l_metaFileVersion.setBounds(new Rectangle(240, 15, 70, 15));
		l_metaFileVersion.setText("Version:");
		l_metaFileVersion.setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		l_metaFileVersion.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		l_metaFileStatus.setBounds(new Rectangle(240, 35, 70, 15));
		l_metaFileStatus.setText("Status:");
		l_metaFileStatus.setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		l_metaFileStatus.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		l_metaFileVersionText.setBounds(new Rectangle(310, 15, 140, 15));
		l_metaFileVersionText.setBackground(new Color(Display.getDefault(),
				255, 255, 255));
		l_metaFileVersionText.setFont(new Font(Display.getDefault(), "Verdana",
				8, SWT.NORMAL));

		l_metaFileStatusText.setBounds(new Rectangle(310, 35, 140, 15));
		l_metaFileStatusText.setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		l_metaFileStatusText.setFont(new Font(Display.getDefault(), "Verdana",
				8, SWT.NORMAL));

		t_metaDescription.setBounds(new Rectangle(10, 15, 227, 80));
		t_metaDescription.setEditable(false);
		
		g_MetaDescription.layout();
		
		c_tabComposite.setBounds(95, 57, 99, 20);
		tabs[PROJECT].setBounds(95, 77, 475, 195);
		

		fillLayout1 = new FillLayout();		   
		tabs[PROJECT].setLayout(fillLayout1);
		fillLayout1.marginHeight = 1;
		fillLayout1.marginWidth = 1;
		fillLayout1.spacing = 1;
		tabs[PROJECT].layout();

		sShell.setSize(578, 320);

		if (conf.getIntValue("pointX").intValue() != 0
				&& conf.getIntValue("pointY").intValue() != 0)
			sShell.setLocation(conf.getIntValue("pointX").intValue(), conf
					.getIntValue("pointY").intValue());
		else {
			Rectangle rec = Display.getCurrent().getBounds();
			shellX = (rec.width - 578) / 2;
			shellY = (rec.height - 320) / 2;
			sShell.setLocation(shellX, shellY);
		}

		sShell.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		sShell.setText("Java Unified Sharing System");
		sShell.setImage(ImageRepository.getImage("tray16Icon"));

		c_title.setBounds(new Rectangle(0, 0, 577, 21));
		c_title.setLayout(new FormLayout());
		
		FormData d = new FormData();
		d.left = new FormAttachment(0,0);
		d.top = new FormAttachment(0,-1);
		d.height = 22;
		d.width = 196;
		
		l_titleRightImg.setLayoutData(d);
		l_titleRightImg.setImage(ImageRepository.getImage("titleRight"));

		d = new FormData();
		d.right = new FormAttachment(100,0);
		d.top = new FormAttachment(0,-1);
		d.height = 22;
		d.width = 381;
		
		l_titleLeftImg.setLayoutData(d);
		l_titleLeftImg.setImage(ImageRepository.getImage("titleLeft"));
		
		d = new FormData();
		d.right = new FormAttachment(100,0);
		d.left = new FormAttachment(0,0);
		d.top = new FormAttachment(0,-1);
		
		l_titleMiddleImg.setLayoutData(d);
		l_titleMiddleImg.setImage(ImageRepository.getImage("titleMiddle"));
		
		d = new FormData();
		d.right = new FormAttachment(100, -24);
		d.top = new FormAttachment(0,-1);
		
		l_trayButtonImg.setLayoutData(d);
		l_trayButtonImg.setImage(ImageRepository.getImage("trayUp"));

		d = new FormData();
		d.right = new FormAttachment(100,0);
		d.top = new FormAttachment(0,-1);
		
		l_closeButtonImg.setLayoutData(d);
		l_closeButtonImg.setImage(ImageRepository.getImage("closeUp"));
		
		c_title.layout();
		
		
		l_menuFile.setBounds(new Rectangle(0, 21, 89, 21));
		l_menuFile.setImage(ImageRepository.getImage("menuFile"));

		l_menuView.setBounds(new Rectangle(89, 21, 88, 21));
		l_menuView.setImage(ImageRepository.getImage("menuView"));

		l_menuOpt.setBounds(new Rectangle(177, 21, 88, 21));
		l_menuOpt.setImage(ImageRepository.getImage("menuOptions"));

		l_menuHelp.setBounds(new Rectangle(265, 21, 88, 21));
		l_menuHelp.setImage(ImageRepository.getImage("menuHelp"));
  		
		l_menuEnd.setBounds(new Rectangle(353, 21, 70, 21));
		l_menuEnd.setImage(ImageRepository.getImage("menuEnd"));

		l_topNurkImg.setBounds(new Rectangle(70, 36, 90, 85));
		l_topNurkImg.setImage(ImageRepository.getImage("nurkTop"));

		l_botNurkImg.setBounds(new Rectangle(15, 202, 73, 74));
		l_botNurkImg.setImage(ImageRepository.getImage("nurkBottom"));

		l_synImg.setBounds(new Rectangle(0, 178, 81, 68));
		l_synImg.setImage(ImageRepository.getImage("synA"));

		
		l_searchImg.setBounds(new Rectangle(0, 110, 81, 68));
		l_searchImg.setImage(ImageRepository.getImage("searchA"));

		l_projectImg.setBounds(new Rectangle(0, 42, 81, 68));
		l_projectImg.setImage(ImageRepository.getImage("proA"));

		l_botRight.setBounds(new Rectangle(429, 287, 149, 33));
		l_botRight.setImage(ImageRepository.getImage("mainNurk"));

		l_statusText.setBounds(new org.eclipse.swt.graphics.Rectangle(0,0,149,12));
		l_statusText.setText("Starting up...");
		l_statusText.setBackground(new Color(Display.getDefault(), 255, 255,
				255));
		l_statusText.setFont(new Font(Display.getDefault(), "Verdana", 6,
				SWT.BOLD));

		c_tabComposite.setBackground(new Color(Display.getDefault(), 255, 255,
				255));
		c_tabComposite.setBounds(new Rectangle(95, 57, 275, 20));

		tabPictures[PROJECT].setBounds(new Rectangle(0, 0, 85, 20));
		tabPictures[PROJECT].setImage(ImageRepository.getImage("tab3"));

		tabPictures[SEARCH].setBounds(new Rectangle(60, 0, 85, 20));
		tabPictures[SEARCH].setImage(ImageRepository.getImage("tab3"));

		tabPictures[META].setBounds(new Rectangle(120, 0, 85, 20));
		tabPictures[META].setImage(ImageRepository.getImage("tab3"));

		tabPictures[TRAFFIC].setBounds(new Rectangle(180, 0, 85, 20));
		tabPictures[TRAFFIC].setImage(ImageRepository.getImage("tab1"));

		tabLabels[PROJECT].setBounds(new Rectangle(15, 6, 44, 15));
		tabLabels[PROJECT].setText("Projects");
		tabLabels[PROJECT].setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		tabLabels[PROJECT].setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));
		tabLabels[PROJECT].setForeground(new Color(Display.getDefault(), 25,
				41, 255));

		tabLabels[SEARCH].setBounds(new Rectangle(85, 6, 44, 15));
		tabLabels[SEARCH].setText("Search");
		tabLabels[SEARCH].setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		tabLabels[SEARCH].setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		tabLabels[META].setBounds(new Rectangle(145, 6, 44, 15));
		tabLabels[META].setText("Meta");
		tabLabels[META].setBackground(new Color(Display.getDefault(), 255, 255,
				255));
		tabLabels[META].setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		tabLabels[TRAFFIC].setBounds(new Rectangle(205, 6, 44, 15));
		tabLabels[TRAFFIC].setText("Traffic");
		tabLabels[TRAFFIC].setBackground(new Color(Display.getDefault(), 255,
				255, 255));
		tabLabels[TRAFFIC].setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		tabs[SEARCH].setBounds(new Rectangle(95, 77, 475, 195));
		
		tabs[SEARCH].setLayout(new FormLayout());
		
		d1 = new FormData();
		d1.left = new FormAttachment(0,1);
		d1.right = new FormAttachment(100,-1);
		d1.top = new FormAttachment(0, 1);
		d1.height = 90;
		
		g_search.setLayoutData(d1);
		g_search.setText("Search");
		g_search.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		
		d1 = new FormData();
		d1.left = new FormAttachment(0,1);
		d1.right = new FormAttachment(100,-1);
		d1.top = new FormAttachment(0, 107);
		d1.bottom = new FormAttachment(100, -1);
		
		g_results.setLayoutData(d1);
		
		g_results.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		g_results.setText("Results");
		
		fillLayout1 = new FillLayout();		   
		g_results.setLayout(fillLayout1);
		fillLayout1.marginHeight = 10;
		fillLayout1.marginWidth = 10;
		
		tabs[SEARCH].layout();

		text_search.setBounds(new Rectangle(10, 32, 157, 25));

		but_search.setBounds(new Rectangle(171, 32, 73, 25));
		but_search.setText("Search");

		l_searchName.setBounds(new Rectangle(10, 16, 75, 14));
		l_searchName.setText("Name:");
		l_searchName.setBackground(new Color(Display.getDefault(), 255, 255,
				255));
		l_searchName.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));

		l_searchMethod.setBounds(new Rectangle(270, 16, 75, 14));
		l_searchMethod.setText("Look in:");
		l_searchMethod.setBackground(new Color(Display.getDefault(), 255, 255,
				255));
		l_searchMethod.setFont(new Font(Display.getDefault(), "Verdana", 8,
				SWT.NORMAL));
		
		b_keywords.setBounds(10, 65, 90, 20);
		b_keywords.setText("Keywords");
		b_keywords.setSelection(true);
		b_keywords.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		
		b_description.setBounds(100, 65, 90, 20);
		b_description.setText("Description");
		b_description.setSelection(true);
		b_description.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		
		b_filename.setBounds(190, 65, 90, 20);
		b_filename.setText("Filename");
		b_filename.setSelection(true);
		b_filename.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		
		b_comments.setBounds(280, 65, 90, 20);
		b_comments.setText("Comments");
		b_comments.setSelection(false);
		b_comments.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		
		b_search_by_creator.setBounds(370, 65, 90, 20);
		b_search_by_creator.setText("By creator");
		b_search_by_creator.setSelection(false);
		b_search_by_creator.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		
		tabs[TRAFFIC].setBounds(new Rectangle(95, 77, 475, 195));
		tabs[TRAFFIC].setBackground(new Color(Display.getDefault(), 255, 255, 255));
		
		c_transfer.setBackground(new Color(Display.getDefault(), 224, 223, 227));
		((ScrolledComposite)tabs[TRAFFIC]).setContent(c_transfer);
		
		c_status.setBounds(5,280,149,40);
		c_status.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		
	    fillLayout.marginHeight = 1;
	    fillLayout.marginWidth = 1;
	    fillLayout.spacing = 1;
	    fillLayout.type = SWT.VERTICAL;
	    c_transfer.setLayout(fillLayout);
	    
		table_results.setHeaderVisible(true);

		tc_resultName.setText("Name");
		tc_resultName.setWidth(200);

		tc_resultSize.setText("Size");
		tc_resultSize.setWidth(80);

		tc_resultDate.setText("Date");
		tc_resultDate.setWidth(120);

		tc_resultVersion.setText("Version");
		tc_resultVersion.setWidth(50);

		comb_search.setBounds(new Rectangle(270, 32, 157, 25));

		but_search.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				l_statusText.setText("Searching...");
				doSearch();
			}
		});

		tabLabels[TRAFFIC].addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				setTab(TRAFFIC);
			}
		});

		tabLabels[META].addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				setTab(META);
			}
		});

		tabLabels[SEARCH].addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				setTab(SEARCH);
			}
		});

		tabLabels[PROJECT].addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				setTab(PROJECT);
			}
		});

		l_projectImg.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				setTab(PROJECT);
				Rectangle r1 = sShell.getBounds();
				int sX = r1.x + (r1.width / 2) - (347 / 2);
				int sY = r1.y + (r1.height / 2) - (291 / 2);

				String[] settings = GuiNewProjectFrm.init(sX, sY, sShell);

				if (settings[0] == null)
					return;

				String name = JussClient.newConnection(settings[0],
						settings[1], settings[2]);
				l_statusText.setText("Opening project...");
				addConnection(settings[1], name);

			}
		});

		l_searchImg.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				setTab(SEARCH);
			}
		});

		l_synImg.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				doFolderRefresh();
			}
		});

		l_menuHelp.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				mainHelpMenu.setVisible(true);
			}
		});

		l_menuOpt.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				mainOptMenu.setVisible(true);
			}
		});

		l_menuView.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				mainViewMenu.setVisible(true);
			}
		});

		l_menuFile.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {

				mainFileMenu.setVisible(true);
			}
		});
		
		l_synImg.addMouseTrackListener(new MouseTrackAdapter() {
			public void mouseExit(MouseEvent e) {
				l_synImg.setImage(ImageRepository.getImage("synA"));
			}

			public void mouseEnter(MouseEvent e) {
				l_synImg.setImage(ImageRepository.getImage("synB"));
			}
		});

		l_searchImg.addMouseTrackListener(new MouseTrackAdapter() {
			public void mouseExit(MouseEvent e) {
				l_searchImg.setImage(ImageRepository.getImage("searchA"));
			}

			public void mouseEnter(MouseEvent e) {
				l_searchImg.setImage(ImageRepository.getImage("searchB"));
			}
		});

		l_projectImg.addMouseTrackListener(new MouseTrackAdapter() {
			public void mouseExit(MouseEvent e) {
				l_projectImg.setImage(ImageRepository.getImage("proA"));
			}

			public void mouseEnter(MouseEvent e) {
				l_projectImg.setImage(ImageRepository.getImage("proB"));
			}
		});

		l_menuHelp.addMouseTrackListener(new MouseTrackAdapter() {
			public void mouseExit(MouseEvent e) {
				l_menuHelp.setImage(ImageRepository.getImage("menuHelp"));
			}

			public void mouseEnter(MouseEvent e) {
				l_menuHelp.setImage(ImageRepository.getImage("menuHelpOver"));
			}
		});

		l_menuOpt.addMouseTrackListener(new MouseTrackAdapter() {
			public void mouseExit(MouseEvent e) {
				l_menuOpt
						.setImage(ImageRepository.getImage("menuOptions"));
			}

			public void mouseEnter(MouseEvent e) {
				l_menuOpt.setImage(ImageRepository.getImage("menuOptionsOver"));
			}
		});

		l_menuView.addMouseTrackListener(new MouseTrackAdapter() {
			public void mouseExit(MouseEvent e) {
				l_menuView.setImage(ImageRepository.getImage("menuView"));
			}

			public void mouseEnter(MouseEvent e) {
				l_menuView.setImage(ImageRepository.getImage("menuViewOver"));
			}
		});

		l_menuFile.addMouseTrackListener(new MouseTrackAdapter() {
			public void mouseExit(MouseEvent e) {
				l_menuFile.setImage(ImageRepository.getImage("menuFile"));
			}

			public void mouseEnter(MouseEvent e) {
				l_menuFile.setImage(ImageRepository.getImage("menuFileOver"));
			}
		});

		l_titleRightImg.addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if (StartMove == true) {
					Rectangle rec = sShell.getBounds();
					sShell.setLocation(rec.x + e.x - mouseX,
							rec.y + e.y - mouseY);

					rec = sShell.getBounds();

					shellX = rec.x;
					shellY = rec.y;

					upDateMenuLocations(rec.x, rec.y);
				}
			}
		});

		l_titleRightImg.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				StartMove = false;
			}

			public void mouseDown(MouseEvent e) {
				StartMove = true;
				mouseX = e.x;
				mouseY = e.y;
			}
		});

		l_titleMiddleImg.addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if (StartMove == true) {
					Rectangle rec = sShell.getBounds();
					sShell.setLocation(rec.x + e.x - mouseX,
							rec.y + e.y - mouseY);

					rec = sShell.getBounds();

					shellX = rec.x;
					shellY = rec.y;

					upDateMenuLocations(rec.x, rec.y);
				}
			}
		});

		l_titleMiddleImg.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				StartMove = false;
			}

			public void mouseDown(MouseEvent e) {
				StartMove = true;
				mouseX = e.x;
				mouseY = e.y;
			}
		});

		l_titleLeftImg.addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if (StartMove == true) {
					Rectangle rec = sShell.getBounds();
					sShell.setLocation(rec.x + e.x - mouseX,
							rec.y + e.y - mouseY);

					rec = sShell.getBounds();

					shellX = rec.x;
					shellY = rec.y;

					upDateMenuLocations(rec.x, rec.y);
				}
			}
		});

		l_titleLeftImg.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				StartMove = false;
			}

			public void mouseDown(MouseEvent e) {
				StartMove = true;
				mouseX = e.x;
				mouseY = e.y;
			}
		});

		
		l_closeButtonImg.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {

				l_closeButtonImg.setVisible(true);
				l_statusText.setText("Shutting down...");
				
				MessageBox messageBox = new MessageBox(sShell, SWT.ICON_QUESTION
						| SWT.YES | SWT.NO);
				messageBox.setText("Shutting down!");
				messageBox.setMessage("Are you sure that you want to exit this program?");

				int response = messageBox.open();
				if(response == SWT.NO)
				{
					l_statusText.setText("Idle...");
					return;
				}
				
				sShell.dispose();
			}

			public void mouseDown(MouseEvent e) {

				l_closeButtonImg.setVisible(false);
			}
		});

		l_trayButtonImg.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				l_trayButtonImg.setVisible(true);
				sShell.setVisible(false);
			}

			public void mouseDown(MouseEvent e) {
				l_trayButtonImg.setVisible(false);
			}
		});

		l_botRight.addMouseMoveListener(new MouseMoveListener()
		{ 
					public void mouseMove(MouseEvent e)
					{    
						
							Rectangle rec = l_botRight.getBounds();
							int x = rec.x + e.x - resize_mouseX;
							int y = rec.y + e.y - resize_mouseY;
							
							if(resizeStartMove == true && x > 428 && y > 287)
							{	
								l_sizer.setLocation(x + rec.width - 18, y + rec.height - 18);
								resize = l_sizer.getBounds();
								sShell.setSize(resize.x + resize.width, resize.y + resize.height);
							}
					}
		});

		l_botRight.addMouseListener(new MouseAdapter()
		{   
			public void mouseUp(MouseEvent e)
			{    
				resizeStartMove = false;
				if(resize != null) resizeFrm(resize);
			} 
					
			public void mouseDown(MouseEvent e)
			{    
				resizeStartMove = true;
				resize_mouseX = e.x;
				resize_mouseY = e.y;
			}
		});
		
		DropTarget target = new DropTarget(tree1, DND.DROP_MOVE | DND.DROP_COPY
				| DND.DROP_DEFAULT);
		final FileTransfer fileTransfer = FileTransfer.getInstance();

		Transfer[] types = new Transfer[] { fileTransfer };

		target.setTransfer(types);
		target.addDropListener(new DropTargetAdapter() {
			public void drop(DropTargetEvent event) {
				if (((String[]) event.data)[0] != null) {
					File fileToAdd = new File(((String[]) event.data)[0]);

					if (fileToAdd.isFile()) {
						Rectangle r1 = sShell.getBounds();
						int sX = r1.x + (r1.width / 2) - (347 / 2);
						int sY = r1.y + (r1.height / 2) - (275 / 2);

						String[] settings = GuiImportFileFrm.init(fileToAdd,
								0, sX, sY, sShell);
						
						doUpload(settings, fileToAdd);
					}
				}
			}
		});
	}
	/**
	 * Meetod faili üleslaadimiseks serverisse.
	 * @param	settings	andmemassiiv meta ja kirjelduse hoidmiseks. 
	 * @param	fileToAdd	informatsioon kohaliku faili kohta.
	 */
	private void doUpload(String[] settings, File fileToAdd)
	{
	if (settings[0] == null)
		return;

	CMetadata meta = new CMetadata("juss", settings[0]);
	meta.setKeywords(settings[1]);
	CConnection con = getConnection(tree.getSelection()[0]);

	TreeItem[] failid = tree1.getItems();
	boolean exist = false;
	
	for(int i = 0; i < failid.length; i++)
	{
		String name1 = fileToAdd.getName();
		String name2 = failid[i].getText();
		
		if(name2.equals(name1))
		{
			exist = true;
			break;
		}
	}
	
	if(exist)
	{
		MessageBox messageBox = new MessageBox(sShell, SWT.ICON_WARNING	| SWT.YES | SWT.NO);
		messageBox.setText("Overwrite?");
		messageBox.setMessage("Current file name already exists! If you want to overwrite current, press YES. If you want add new version, press NO.");
		int response = messageBox.open();
		
		if(response == SWT.YES)
			JussClient.uploadFile(fileToAdd.getPath(), getId(tree
			.getSelection()[0]), meta, "overwrite", con
			.getRoot());
		else
			JussClient.uploadFile(fileToAdd.getPath(), getId(tree
			.getSelection()[0]), meta, "version", con
			.getRoot());
		
		getContext(tree.getSelection()[0]);
		return;
	}
	
	JussClient.uploadFile(fileToAdd.getPath(), getId(tree
			.getSelection()[0]), meta, "overwrite", con
			.getRoot());
	
	getContext(tree.getSelection()[0]);
	}
	/**
	 * Korrastatakse elemendid otsingu akna valikmenüüs.
	 */
	private void initSearch() {
		comb_search.removeAll();
		TreeItem[] items = tree.getItems();
		for (int i = 0; i < items.length; i++) {
			CConnection con = getConnection(items[i]);
			comb_search.add(con.getName(), i);
		}
	
		try {
		comb_search.setText(comb_search.getItem(0));
		} catch (Exception e) {
			return;
		}

	}
	/**
	 * Teostab faili otsingu.
	 */
	private void doSearch() {
		if (comb_search.getSelectionIndex() == -1)
			return;
		TreeItem[] items = tree.getItems();		
		CConnection con = getConnection(items[comb_search.getSelectionIndex()]);
		
		long crit = 0;
		
		if(b_keywords.getSelection())
		{
			if(crit == 0) crit = SearchCriteria.KEYWORDS;
			else crit = crit | SearchCriteria.KEYWORDS;
		}
		
		if(b_description.getSelection())
		{
			if(crit == 0) crit = SearchCriteria.DESCRIPTION;
			else crit = crit | SearchCriteria.DESCRIPTION;
		}
		
		if(b_filename.getSelection())
		{
			if(crit == 0) crit = SearchCriteria.FILENAME;
			else crit = crit | SearchCriteria.FILENAME;
		}
		
		if(b_comments.getSelection())
		{
			if(crit == 0) crit = SearchCriteria.COMMENTS;
			else crit = crit | SearchCriteria.COMMENTS;
		}
		
		if(b_search_by_creator.getSelection())
		{
			if(crit == 0) crit = SearchCriteria.SEARCH_BY_CREATOR;
			else crit = crit | SearchCriteria.SEARCH_BY_CREATOR;
		}
		
		SearchCriteria c = new SearchCriteria(text_search.getText(), new Long(crit));
		
		java.util.List s_list = con.search(c);
		table_results.removeAll();
		Iterator i = s_list.iterator();
		while (i.hasNext()) {
			JussFile jf = (JussFile) i.next();
			TableItem item = new TableItem(table_results, SWT.NONE);
			String[] str = { jf.getName(), "NONE", jf.getModtime().toString(),
					"" + jf.getVersion() };
			item.setText(str);
		}
		l_statusText.setText("Done...");
	}
	/**
	 * Vahetatakse tab akent.
	 * @param	i	tab'i number.
	 */
	private void setTab(int i) {
		for (int j = PROJECT; j <= TRAFFIC; j++) {
			if (j < i) {
				tabPictures[j].setImage(ImageRepository.getImage("tab2"));
			} else if (j >= i) {
				if (j == TRAFFIC)
					tabPictures[j].setImage(ImageRepository.getImage("tab1"));
				else
					tabPictures[j].setImage(ImageRepository.getImage("tab3"));
			}

			if (j == i) {
				tabLabels[j].setForeground(new Color(Display.getDefault(), 25,
						41, 255));
				tabs[j].setVisible(true);
			} else {
				tabLabels[j].setForeground(new Color(Display.getDefault(), 0,
						0, 0));
				tabs[j].setVisible(false);
			}
		}
	}
	/**
	 * Luuakse "Traffic" aknasse uus element (transfer).
	 * @param	cmp	Composite tüüpi kandja, kuhu lisatakse genereeritav kandja.
	 * @return	genereeritud kandja.
	 */
	private Composite createComposite(Composite cmp)
	{
		final Composite composite = new Composite(cmp, SWT.NONE);
		final Label l_transferName = new Label(composite, SWT.NONE);
		final Label l_transferNameTxt = new Label(composite, SWT.NONE);
		final Label l_transferStatusImgGover = new Label(composite, SWT.NONE);
		final Label l_transferStatusImg = new Label(composite, SWT.NONE);
		final Label l_transferStatus = new Label(composite, SWT.NONE);
		final Label l_transferStatusText = new Label(composite, SWT.NONE);
		final Label l_transferTransferred = new Label(composite, SWT.NONE);
		
		composite.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_transferName.setBounds(new Rectangle(10,10,50,15));
		l_transferName.setText("Name:");
		l_transferName.setFont(new Font(Display.getDefault(), "Verdana", 8, org.eclipse.swt.SWT.NORMAL));
		l_transferName.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_transferNameTxt.setBounds(new Rectangle(60,10,300,15));
		l_transferNameTxt.setFont(new Font(Display.getDefault(), "Verdana", 8, org.eclipse.swt.SWT.BOLD));
		l_transferNameTxt.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_transferNameTxt.setData(new Long(1));
		
		l_transferStatusImgGover.setBounds(new Rectangle(10,30,231,11));
		l_transferStatusImgGover.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_transferStatusImgGover.setData(new Long(2));
		
		l_transferStatusImg.setBounds(new Rectangle(10,30,231,11));
		l_transferStatusImg.setImage(ImageRepository.getImage("barBig"));
		
		l_transferStatus.setBounds(new Rectangle(10,45,50,15));
		l_transferStatus.setText("Status:");
		l_transferStatus.setFont(new Font(Display.getDefault(), "Verdana", 8, org.eclipse.swt.SWT.NORMAL));
		l_transferStatus.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_transferStatusText.setBounds(new Rectangle(60,45,100,15));
		l_transferStatusText.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		l_transferStatusText.setData(new Long(3));
		
		l_transferStatusText.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_transferTransferred.setBounds(new Rectangle(10,65,230,15));
		l_transferTransferred.setBackground(new Color(Display.getDefault(), 255, 255, 255));
		l_transferTransferred.setFont(new Font(Display.getDefault(), "Verdana", 8, SWT.NORMAL));
		l_transferTransferred.setData(new Long(4));
		
		final Menu trafficManu = new Menu(sShell, SWT.PUSH);
		
		final MenuItem closeCurrent = new MenuItem(trafficManu, SWT.PUSH);
		closeCurrent.setText("Remove");

		final MenuItem closeAll = new MenuItem(trafficManu, SWT.PUSH);
		closeAll.setText("Clear compleated");

		closeAll.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event e)
			{
				final Control[] parent = tabs[TRAFFIC].getChildren();
				final Control[] children = ((Composite)parent[0]).getChildren();
				
				for(int i = 0; i < children.length; i++)
				{
					CTransfer data = ((CTransfer)children[i].getData());
					if(data.isDone())
					{
						children[i].dispose();
						((Composite)parent[0]).setSize(c_transfer.computeSize(SWT.DEFAULT, SWT.DEFAULT));
						((Composite)parent[0]).layout();
					}
				}
				JussClient.getTransferManager().removeFinishedTransfers();
			}
		});

		closeCurrent.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				CTransfer data = (CTransfer)composite.getData();
				if(data.isDone())
				{
					Composite parent = composite.getParent();
					composite.dispose();
			        
					parent.setSize(c_transfer.computeSize(SWT.DEFAULT, SWT.DEFAULT));
					parent.layout();
				}
			}
		});
		
		Control[] elements = composite.getChildren();
		
		for(int i = 0; i < elements.length; i++)
		{
			elements[i].addMouseListener(new MouseAdapter() {
				public void mouseUp(MouseEvent e)
				{
					if (e.button == 3) 
					{
						trafficManu.setVisible(true);
					}
				}
			});
		}
		
		composite.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e)
			{
				if (e.button == 3) 
				{
					trafficManu.setVisible(true);
				}
			}
		});
		
		return composite;
	}
	/**
	 * Alglaadija poolt kutsutud timer, mis kontrollib failide liiklust.
	 */
	private void startTransferTimer()
	{
    	Runnable runnable = new Runnable()
		{
			public void run()
			{
				if(sShell.isDisposed()) return;
				
				if(!l_statusText.getText().equals("Idle...") && doneCounter < 100) doneCounter++;
				else
				{
					l_statusText.setText("Idle...");
					doneCounter = 0;
				}
				
				final java.util.List transferList = JussClient.getTransferManager().getTransfers();
				
				final Control[] transfersContainer = tabs[TRAFFIC].getChildren();
				final Control[] visibleTransfers = ((Composite)transfersContainer[0]).getChildren();
				
				final ArrayList l = new ArrayList();
				
				for(int i = 0; i < visibleTransfers.length; i++)
				{
					l.add(((Composite)visibleTransfers[i]).getData());
				}
				
				Iterator i = transferList.iterator();
				
				while(i.hasNext())
				{
					CTransfer t = null;
					try
					{
						/**
						 * TODO:
						 * mingil p6hjusel viskab aegajalt siin errori:
						 * java.util.ConcurrentModificationException
						 * stack:
						 * at java.util.AbstractList$Itr.checkForComodification(Unknown Source)
						 * at java.util.AbstractList$Itr.next(Unknown Source)
						 */
						
						t = (CTransfer)i.next();
					}
					catch(Exception e)
					{
						Display.getDefault().timerExec(100, this);
						return;
					}
					
					if(!l.contains(t))
					{
				        final Composite comp = createComposite(c_transfer);
				        Widget[] labels = ((Widget[])comp.getChildren());
				        l_statusText.setText("Transferring...");
				        
				        for(int c = 0; c < labels.length; c++)
				        {
				        	Long id = ((Long)labels[c].getData());
				        	
				        	if(id != null)
				        	{
				        		switch(id.intValue())
								{
				        			case 1:
				        				((Label)labels[c]).setText(t.getFileName());
				        				((Label)labels[c]).pack();
				        				
				        				Point pnt1 = tabs[TRAFFIC].getSize();
				        				Point pnt2 = ((Label)labels[c]).getSize();
				        				
				        				if(pnt1.x > pnt2.x) ((Label)labels[c]).setSize(pnt1.x - 70, 15);
				        				else ((Label)labels[c]).setSize(pnt2.x + 10, 15);
				        				
				        				break;
									case 2:
										((Label)labels[c]).setBounds(10 + (int)(231 * t.getProgress()),30,231 - (int)(231 * t.getProgress()),11);
				        				break;
									case 3:
										if(t.isTransfering()) ((Label)labels[c]).setText("Transferring...");
										else if(t.isFinished()) ((Label)labels[c]).setText("Finished...");
										else if(t.isDone()) ((Label)labels[c]).setText("Done...");
										else if(t.isQueued()) ((Label)labels[c]).setText("Waiting...");
				        				break;
									case 4:
										if(t.isTransfering()) ((Label)labels[c]).setText((t.getDone() / 1024) + " Kb of " + (t.getSize() /1024) + " Kb transfered...");
										else ((Label)labels[c]).setText("");
				        				break;
								}
				        	}
				        }
				        comp.setData(t);
					}
					else
					{
						for(int f = 0; f < visibleTransfers.length; f++)
						{
							if(((CTransfer)((Composite)visibleTransfers[f]).getData()) == t)
							{
								Widget[] labels = ((Widget[])((Composite)visibleTransfers[f]).getChildren());
								
						        for(int c = 0; c < labels.length; c++)
						        {
						        	Long id = ((Long)labels[c].getData());
						        	
						        	if(id != null)
						        	{
						        		switch(id.intValue())
										{
											case 2:
												double prog = t.getProgress();
												((Label)labels[c]).setBounds(10 + (int)(231 * prog),30,231 - (int)(231 * prog),11);
						        				break;
											case 3:
												if(t.isTransfering()) ((Label)labels[c]).setText("Transferring...");
												else if(t.isFinished()) ((Label)labels[c]).setText("Finished...");
												else if(t.isDone())
												{
													l_statusText.setText("Transfer done...");
													((Label)labels[c]).setText("Done...");
													getContext(tree.getSelection()[0]);
												}
												else if(t.isQueued()) ((Label)labels[c]).setText("Waiting...");
						        				break;
											case 4:
												if(t.isTransfering()) ((Label)labels[c]).setText((t.getDone() / 1024) + " Kb of " + (t.getSize() /1024) + " Kb transfered...");
												else ((Label)labels[c]).setText("");
						        				break;
										}
						        	}
						        }
							}
						}
						
				        c_transfer.setSize(c_transfer.computeSize(SWT.DEFAULT, SWT.DEFAULT));
				        c_transfer.layout();
					}
				}
				
				Display.getDefault().timerExec(100, this);
      		}
		};
		
		Display.getDefault().timerExec(100, runnable);
	}
	/**
	 * Meetod, mis liigutab elemente akna suurendamisel.
	 * @param	rec	uus paigutus.
	 */
	
	private void resizeFrm(Rectangle rec)
	{
		Point pnt1 = l_botRight.getSize();
		Point pnt2 = sShell.getSize();
		l_botRight.setLocation(pnt2.x - pnt1.x, pnt2.y - pnt1.y);
		c1.setSize(pnt2);
		c2.setSize(pnt2.x +1, pnt2.y + 1);
		
		tabs[0].setSize(rec.x + rec.width - (95 + 8), rec.y + rec.height - (77 + 48));
		tabs[1].setSize(rec.x + rec.width - (95 + 8), rec.y + rec.height - (77 + 48));
		tabs[2].setSize(rec.x + rec.width - (95 + 8), rec.y + rec.height - (77 + 48));
		tabs[3].setSize(rec.x + rec.width - (95 + 8), rec.y + rec.height - (77 + 48));
		c_logo.setSize(rec.x + rec.width - 320, 56);
		c_title.setSize(rec.x + rec.width, 21);
	
		Rectangle rec1 = sShell.getBounds();
		c_status.setLocation(5, rec1.height - 40);
	}
	/**
	 * Uuendatakse menüüde asukohti akna liigutamisel.
	 * @param	x	uus akna asukoht x teljel.
	 * @param	y	uus akna asukoht y teljel.
	 */
	private void upDateMenuLocations(int x, int y)
	{
		mainFileMenu.setLocation(x, y + 42);
		mainViewMenu.setLocation(x + 89, y + 42);
		mainOptMenu.setLocation(x + 180, y + 42);
		mainHelpMenu.setLocation(x + 267, y + 42);
	}
	
	public void doLogin(TreeItem item) {
		CConnection con = getConnection(item);
		Rectangle r1 = sShell.getBounds();
		// this is bad style
		// TODO: FIX IT
		int sX = r1.x + (r1.width / 2) - (300 / 2);
		int sY = r1.y + (r1.height / 2) - (180 / 2);
		item.setImage(ImageRepository.getImage("offline"));
		do {
			String[] settings = GuiLogInFrm.init(sX, sY, sShell);
			if (settings[0] == null) {
				return;
			}

			if (con.login(settings[0], settings[1])) {
				item.setImage(ImageRepository.getImage("online"));
				getContext(item);
				return;
			} else {
				if(!getConfirmation("Login Failed", "Login failed - try again?")) {
					break;
				}
			}
		} while (true);
	}
	
	public boolean getConfirmation(String title, String message) {
		MessageBox mb = new MessageBox(sShell, SWT.ICON_WARNING | SWT.YES
				| SWT.NO);
		mb.setText(title);
		mb.setMessage(message);
		if (mb.open() == SWT.YES)
			return true;
		return false;
	}

}