/////////////////////////////////////////////////////////////////////////////
// Name:        frmClient.cpp
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/10/05 14:54:14
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "frmClient.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "frmClient.h"
#include "CClient.h"

////@begin XPM images
////@end XPM images

/*!
 * frmClient type definition
 */

IMPLEMENT_CLASS( frmClient, wxFrame )

/*!
 * frmClient event table definition
 */

BEGIN_EVENT_TABLE( frmClient, wxFrame )

////@begin frmClient event table entries
    EVT_CLOSE( frmClient::OnCloseWindow )

    EVT_MENU( ID_MENU_FILE_CONNECT, frmClient::OnMenuFileConnectClick )

    EVT_MENU( ID_MENU_FILE_DISCON, frmClient::OnMenuFileDisconClick )

    EVT_MENU( ID_MENU_FILE_EXIT, frmClient::OnMenuFileExitClick )

    EVT_MENU( ID_MENU_ENT_LISTALL, frmClient::OnMenuEntListallClick )

    EVT_MENU( ID_MENU_WND_DEBUG, frmClient::OnMenuWndDebugClick )

    EVT_MENU( ID_TOOL_PING, frmClient::OnToolPingClick )

////@end frmClient event table entries

END_EVENT_TABLE()

/*!
 * frmClient constructors
 */

frmClient::frmClient( )
{
	m_ConnectDlg=NULL;
	m_DebugDlg=NULL;
}

frmClient::frmClient( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create( parent, id, caption, pos, size, style );
}

/*!
 * frmClient creator
 */

bool frmClient::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin frmClient member initialisation
    entityList = NULL;
////@end frmClient member initialisation

////@begin frmClient creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end frmClient creation
	//m_ConnectDlg=new dlgCntServer(NULL, ID_DIALOG, _("Connect To Server"));
	//m_DebugDlg = new dlgDebug(NULL, ID_DIALOG_DEBUG, _("Debug Output"));
	m_ConnectDlg=NULL;
	m_DebugDlg=NULL;
    return TRUE;
}

/*!
 * Control creation for frmClient
 */

void frmClient::CreateControls()
{    
////@begin frmClient content construction

    frmClient* itemFrame1 = this;

    wxSplitterWindow* itemSplitterWindow11 = new wxSplitterWindow( itemFrame1, ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

    wxPanel* itemPanel12 = new wxPanel( itemSplitterWindow11, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxStaticBox* itemStaticBoxSizer13Static = new wxStaticBox(itemPanel12, wxID_ANY, _("Visible Entities"));
    wxStaticBoxSizer* itemStaticBoxSizer13 = new wxStaticBoxSizer(itemStaticBoxSizer13Static, wxVERTICAL);
    itemPanel12->SetSizer(itemStaticBoxSizer13);

    entityList = new wxListCtrl( itemPanel12, ID_LISTCTRL, wxDefaultPosition, wxSize(100, 100), wxLC_REPORT );
    itemStaticBoxSizer13->Add(entityList, 1, wxGROW|wxALL, 5);

    wxPanel* itemPanel15 = new wxPanel( itemSplitterWindow11, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxStaticBox* itemStaticBoxSizer16Static = new wxStaticBox(itemPanel15, wxID_ANY, _("Entity Information"));
    wxStaticBoxSizer* itemStaticBoxSizer16 = new wxStaticBoxSizer(itemStaticBoxSizer16Static, wxHORIZONTAL);
    itemPanel15->SetSizer(itemStaticBoxSizer16);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer16->Add(itemBoxSizer17, 0, wxALIGN_TOP|wxALL, 5);
    wxStaticText* itemStaticText18 = new wxStaticText( itemPanel15, wxID_STATIC, _("Entity Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(itemStaticText18, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl19 = new wxTextCtrl( itemPanel15, ID_TEXTCTRL1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    itemBoxSizer17->Add(itemTextCtrl19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemSplitterWindow11->SplitVertically(itemPanel12, itemPanel15, 150);

    wxToolBar* itemToolBar20 = new wxToolBar( itemFrame1, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL );
    wxBitmap itemtool21Bitmap(itemFrame1->GetBitmapResource(wxT("ping.gif")));
    itemToolBar20->AddTool(ID_TOOL_PING, _("Ping Server"), itemtool21Bitmap, wxNullBitmap, wxITEM_NORMAL, _("Ping the Server"), _("Ping the Server"));
    wxBitmap itemtool22Bitmap(itemFrame1->GetBitmapResource(wxT("3waydiv.gif")));
    itemToolBar20->AddTool(ID_TOOL_SPW, _("Spawn Entity"), itemtool22Bitmap, wxNullBitmap, wxITEM_NORMAL, _("Spawn Entity"), _("Spawn Entity"));
    itemToolBar20->Realize();
    itemFrame1->SetToolBar(itemToolBar20);

    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* itemMenu3 = new wxMenu;
    itemMenu3->Append(ID_MENU_FILE_CONNECT, _("Connect"), _T(""), wxITEM_NORMAL);
    itemMenu3->Append(ID_MENU_FILE_DISCON, _("Disconnect"), _T(""), wxITEM_NORMAL);
    itemMenu3->Append(ID_MENU_FILE_EXIT, _("Exit"), _T(""), wxITEM_NORMAL);
    menuBar->Append(itemMenu3, _("File"));
    wxMenu* itemMenu7 = new wxMenu;
    itemMenu7->Append(ID_MENU_ENT_LISTALL, _("List All"), _T(""), wxITEM_NORMAL);
    menuBar->Append(itemMenu7, _("Entities"));
    wxMenu* itemMenu9 = new wxMenu;
    itemMenu9->Append(ID_MENU_WND_DEBUG, _("Debug Dialog"), _T(""), wxITEM_NORMAL);
    menuBar->Append(itemMenu9, _("Windows"));
    itemFrame1->SetMenuBar(menuBar);

////@end frmClient content construction
	entityList->InsertColumn(0, _T("EID"));
	entityList->InsertColumn(1, _T("Name"));
	entityList->InsertColumn(2, _T("Owner"));
	entityList->InsertColumn(3, _T("Position"));
}

/*!
 * Should we show tooltips?
 */

bool frmClient::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap frmClient::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin frmClient bitmap retrieval
    if (name == wxT("ping.gif"))
    {
        wxBitmap bitmap(_T("ping.gif"), wxBITMAP_TYPE_GIF);
        return bitmap;
    }
    else if (name == wxT("3waydiv.gif"))
    {
        wxBitmap bitmap(_T("3waydiv.gif"), wxBITMAP_TYPE_GIF);
        return bitmap;
    }
    return wxNullBitmap;
////@end frmClient bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon frmClient::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin frmClient icon retrieval
    return wxNullIcon;
////@end frmClient icon retrieval
}
/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_CONNECT
 */

void frmClient::OnMenuFileConnectClick( wxCommandEvent& event )
{
	if(m_ConnectDlg==NULL) {
		nlwarning("Connection dialog didn't exist upon request.");
		m_ConnectDlg=new dlgCntServer(NULL, ID_DIALOG, _("Connect To Server"));
	}
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_CONNECT in frmClient.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_CONNECT in frmClient. 
	if(!CClient::instance().isConnected()) {
		m_ConnectDlg->Show(true);
		//Disable();
	}
}


/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_FRAME
 */

void frmClient::OnCloseWindow( wxCloseEvent& event )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_FRAME in frmClient.
    // Before editing this code, remove the block markers.
    wxWindow* window = this;
    window->Destroy();
////@end wxEVT_CLOSE_WINDOW event handler for ID_FRAME in frmClient. 
	if(m_ConnectDlg!=NULL)
		m_ConnectDlg->Destroy();
	if(m_DebugDlg!=NULL)
		m_DebugDlg->Destroy();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_DISCON
 */

void frmClient::OnMenuFileDisconClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_DISCON in frmClient.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_DISCON in frmClient. 
	if(!CClient::instance().isConnected())
		CClient::instance().disconnect();
	else
		nlwarning("Not able to disconnect, not connected.");
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_EXIT
 */

void frmClient::OnMenuFileExitClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_EXIT in frmClient.
    // Before editing this code, remove the block markers.
    Destroy();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_EXIT in frmClient. 
	nlinfo("Request to exit client!");
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_WND_DEBUG
 */

void frmClient::OnMenuWndDebugClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_WND_DEBUG in frmClient.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_WND_DEBUG in frmClient. 
	if(m_DebugDlg==NULL) {
		nlwarning("Debug dialog didn't exist upon request.");
		m_DebugDlg = new dlgDebug(NULL, ID_DIALOG_DEBUG, _("Debug Output"));
	}
    
	if(m_DebugDlg->IsShown())
		m_DebugDlg->Show(false);
	else
		m_DebugDlg->Show(true);
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_PING
 */

void frmClient::OnToolPingClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_PING in frmClient.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_PING in frmClient. 
	if(CClient::instance().isConnected()) {
		NLNET::CMessage msgout("PING");
		CClient::instance().getClient()->send(msgout);
	}
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_ENT_LISTALL
 */

void frmClient::OnMenuEntListallClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_ENT_LISTALL in frmClient.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_ENT_LISTALL in frmClient. 

	//entityList
}


