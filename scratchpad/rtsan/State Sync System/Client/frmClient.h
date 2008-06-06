/////////////////////////////////////////////////////////////////////////////
// Name:        frmClient.h
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/10/05 14:54:14
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FRMCLIENT_H_
#define _FRMCLIENT_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "frmClient.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/splitter.h"
#include "wx/listctrl.h"
#include "wx/toolbar.h"
////@end includes
#include "dlgCntServer.h"
#include "dlgDebug.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxListCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_FRAME 10000
#define SYMBOL_FRMCLIENT_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FRMCLIENT_TITLE _("State Client")
#define SYMBOL_FRMCLIENT_IDNAME ID_FRAME
#define SYMBOL_FRMCLIENT_SIZE wxSize(600, 400)
#define SYMBOL_FRMCLIENT_POSITION wxDefaultPosition
#define ID_MENU_FILE_CONNECT 10002
#define ID_MENU_FILE_DISCON 10011
#define ID_MENU_FILE_EXIT 10013
#define ID_MENU_ENT_LISTALL 10012
#define ID_MENU_WND_DEBUG 10014
#define ID_SPLITTERWINDOW 10003
#define ID_PANEL 10004
#define ID_LISTCTRL 10001
#define ID_PANEL1 10005
#define ID_TEXTCTRL1 10006
#define ID_TOOLBAR 10016
#define ID_TOOL_PING 10017
#define ID_TOOL_SPW 10010
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * frmClient class declaration
 */

class frmClient: public wxFrame
{    
    DECLARE_CLASS( frmClient )
    DECLARE_EVENT_TABLE()

	dlgCntServer	*m_ConnectDlg;
	dlgDebug		*m_DebugDlg;
public:
    /// Constructors
    frmClient( );
    frmClient( wxWindow* parent, wxWindowID id = SYMBOL_FRMCLIENT_IDNAME, const wxString& caption = SYMBOL_FRMCLIENT_TITLE, const wxPoint& pos = SYMBOL_FRMCLIENT_POSITION, const wxSize& size = SYMBOL_FRMCLIENT_SIZE, long style = SYMBOL_FRMCLIENT_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FRMCLIENT_IDNAME, const wxString& caption = SYMBOL_FRMCLIENT_TITLE, const wxPoint& pos = SYMBOL_FRMCLIENT_POSITION, const wxSize& size = SYMBOL_FRMCLIENT_SIZE, long style = SYMBOL_FRMCLIENT_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin frmClient event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_FRAME
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_CONNECT
    void OnMenuFileConnectClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_DISCON
    void OnMenuFileDisconClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_EXIT
    void OnMenuFileExitClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_ENT_LISTALL
    void OnMenuEntListallClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_WND_DEBUG
    void OnMenuWndDebugClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_PING
    void OnToolPingClick( wxCommandEvent& event );

////@end frmClient event handler declarations

////@begin frmClient member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end frmClient member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin frmClient member variables
    wxListCtrl* entityList;
////@end frmClient member variables
};

#endif
    // _FRMCLIENT_H_
