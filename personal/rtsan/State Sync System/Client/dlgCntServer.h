/////////////////////////////////////////////////////////////////////////////
// Name:        dlgCntServer.h
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/10/05 15:24:22
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DLGCNTSERVER_H_
#define _DLGCNTSERVER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "dlgCntServer.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10007
#define SYMBOL_DLGCNTSERVER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_DLGCNTSERVER_TITLE _("Connect To Server")
#define SYMBOL_DLGCNTSERVER_IDNAME ID_DIALOG
#define SYMBOL_DLGCNTSERVER_SIZE wxSize(400, 300)
#define SYMBOL_DLGCNTSERVER_POSITION wxDefaultPosition
#define ID_TEXT_SRVR 10009
#define ID_TEXT_USER 10000
#define ID_TEXT_PASS 10001
#define ID_BTN_CONNECT 10008
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
 * dlgCntServer class declaration
 */

class dlgCntServer: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( dlgCntServer )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    dlgCntServer( );
    dlgCntServer( wxWindow* parent, wxWindowID id = SYMBOL_DLGCNTSERVER_IDNAME, const wxString& caption = SYMBOL_DLGCNTSERVER_TITLE, const wxPoint& pos = SYMBOL_DLGCNTSERVER_POSITION, const wxSize& size = SYMBOL_DLGCNTSERVER_SIZE, long style = SYMBOL_DLGCNTSERVER_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DLGCNTSERVER_IDNAME, const wxString& caption = SYMBOL_DLGCNTSERVER_TITLE, const wxPoint& pos = SYMBOL_DLGCNTSERVER_POSITION, const wxSize& size = SYMBOL_DLGCNTSERVER_SIZE, long style = SYMBOL_DLGCNTSERVER_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin dlgCntServer event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BTN_CONNECT
    void OnBtnConnectClick( wxCommandEvent& event );

////@end dlgCntServer event handler declarations

////@begin dlgCntServer member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end dlgCntServer member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin dlgCntServer member variables
    wxTextCtrl* wxServerText;
    wxTextCtrl* wxUserText;
    wxTextCtrl* wxPassText;
////@end dlgCntServer member variables
};

#endif
    // _DLGCNTSERVER_H_
