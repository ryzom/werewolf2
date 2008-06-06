/////////////////////////////////////////////////////////////////////////////
// Name:        dlgCntServer.cpp
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/10/05 15:24:22
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "dlgCntServer.h"
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

#include "dlgCntServer.h"
#include "CClient.h"

////@begin XPM images
////@end XPM images

/*!
 * dlgCntServer type definition
 */

IMPLEMENT_DYNAMIC_CLASS( dlgCntServer, wxDialog )

/*!
 * dlgCntServer event table definition
 */

BEGIN_EVENT_TABLE( dlgCntServer, wxDialog )

////@begin dlgCntServer event table entries
    EVT_BUTTON( ID_BTN_CONNECT, dlgCntServer::OnBtnConnectClick )

////@end dlgCntServer event table entries

END_EVENT_TABLE()

/*!
 * dlgCntServer constructors
 */

dlgCntServer::dlgCntServer( )
{
}

dlgCntServer::dlgCntServer( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * dlgCntServer creator
 */

bool dlgCntServer::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin dlgCntServer member initialisation
    wxServerText = NULL;
    wxUserText = NULL;
    wxPassText = NULL;
////@end dlgCntServer member initialisation

////@begin dlgCntServer creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end dlgCntServer creation
    return TRUE;
}

/*!
 * Control creation for dlgCntServer
 */

void dlgCntServer::CreateControls()
{    
////@begin dlgCntServer content construction

    dlgCntServer* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, _("Server"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxServerText = new wxTextCtrl( itemDialog1, ID_TEXT_SRVR, _T("localhost"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(wxServerText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("Username"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxUserText = new wxTextCtrl( itemDialog1, ID_TEXT_USER, _T("mattr"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(wxUserText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, wxID_STATIC, _("Password"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxPassText = new wxTextCtrl( itemDialog1, ID_TEXT_PASS, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(wxPassText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton12 = new wxButton( itemDialog1, ID_BTN_CONNECT, _("Connect"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemButton12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end dlgCntServer content construction
}

/*!
 * Should we show tooltips?
 */

bool dlgCntServer::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap dlgCntServer::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin dlgCntServer bitmap retrieval
    return wxNullBitmap;
////@end dlgCntServer bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon dlgCntServer::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin dlgCntServer icon retrieval
    return wxNullIcon;
////@end dlgCntServer icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BTN_CONNECT
 */

void dlgCntServer::OnBtnConnectClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BTN_CONNECT in dlgCntServer.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BTN_CONNECT in dlgCntServer. 
	std::string srv, usr, pass;
	srv=wxServerText->GetValue();
	usr=wxUserText->GetValue();
	pass=wxPassText->GetValue();
	nlinfo("Connecting with %s / %s / %s", srv.c_str(), usr.c_str(), pass.c_str());
	CClient::instance().connectUsing(srv,usr,pass);
    CClient::instance().connect();
	this->Hide();
}


