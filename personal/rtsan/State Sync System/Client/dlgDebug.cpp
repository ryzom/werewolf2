/////////////////////////////////////////////////////////////////////////////
// Name:        dlgDebug.cpp
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/11/05 11:23:14
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "dlgDebug.h"
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

#include "dlgDebug.h"
#include "wxDisplayer.h"
#include <nel/misc/displayer.h>
#include <nel/misc/debug.h>

////@begin XPM images
////@end XPM images

/*!
 * dlgDebug type definition
 */

IMPLEMENT_DYNAMIC_CLASS( dlgDebug, wxDialog )

/*!
 * dlgDebug event table definition
 */

BEGIN_EVENT_TABLE( dlgDebug, wxDialog )

////@begin dlgDebug event table entries
    EVT_CLOSE( dlgDebug::OnCloseWindow )

////@end dlgDebug event table entries

END_EVENT_TABLE()

/*!
 * dlgDebug constructors
 */

dlgDebug::dlgDebug( )
{
}

dlgDebug::dlgDebug( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * dlgDebug creator
 */

bool dlgDebug::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin dlgDebug member initialisation
    dlgDbgText = NULL;
////@end dlgDebug member initialisation

////@begin dlgDebug creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end dlgDebug creation
	CWxDisplayer *disp=new CWxDisplayer(this);
	NLMISC::InfoLog->addDisplayer(disp);
	NLMISC::WarningLog->addDisplayer(disp);
	NLMISC::DebugLog->addDisplayer(disp);
	NLMISC::ErrorLog->addDisplayer(disp);
	NLMISC::AssertLog->addDisplayer(disp);
    return TRUE;
}

/*!
 * Control creation for dlgDebug
 */

void dlgDebug::CreateControls()
{    
////@begin dlgDebug content construction

    dlgDebug* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    dlgDbgText = new wxTextCtrl( itemDialog1, ID_DLG_DBG_TEXT, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer2->Add(dlgDbgText, 1, wxGROW|wxALL, 5);

////@end dlgDebug content construction
}

/*!
 * Should we show tooltips?
 */

bool dlgDebug::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap dlgDebug::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin dlgDebug bitmap retrieval
    return wxNullBitmap;
////@end dlgDebug bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon dlgDebug::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin dlgDebug icon retrieval
    return wxNullIcon;
////@end dlgDebug icon retrieval
}
/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_DEBUG
 */

void dlgDebug::OnCloseWindow( wxCloseEvent& event )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_DEBUG in dlgDebug.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_DEBUG in dlgDebug. 
	Show(false);
}


