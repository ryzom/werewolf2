/////////////////////////////////////////////////////////////////////////////
// Name:        dlgDebug.h
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/11/05 11:23:14
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DLGDEBUG_H_
#define _DLGDEBUG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "dlgDebug.cpp"
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
#define ID_DIALOG_DEBUG 10015
#define SYMBOL_DLGDEBUG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_DLGDEBUG_TITLE _("Debug Output")
#define SYMBOL_DLGDEBUG_IDNAME ID_DIALOG_DEBUG
#define SYMBOL_DLGDEBUG_SIZE wxSize(800, 200)
#define SYMBOL_DLGDEBUG_POSITION wxPoint(100, 400)
#define ID_DLG_DBG_TEXT 10000
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
 * dlgDebug class declaration
 */

class dlgDebug: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( dlgDebug )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    dlgDebug( );
    dlgDebug( wxWindow* parent, wxWindowID id = SYMBOL_DLGDEBUG_IDNAME, const wxString& caption = SYMBOL_DLGDEBUG_TITLE, const wxPoint& pos = SYMBOL_DLGDEBUG_POSITION, const wxSize& size = SYMBOL_DLGDEBUG_SIZE, long style = SYMBOL_DLGDEBUG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DLGDEBUG_IDNAME, const wxString& caption = SYMBOL_DLGDEBUG_TITLE, const wxPoint& pos = SYMBOL_DLGDEBUG_POSITION, const wxSize& size = SYMBOL_DLGDEBUG_SIZE, long style = SYMBOL_DLGDEBUG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin dlgDebug event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG_DEBUG
    void OnCloseWindow( wxCloseEvent& event );

////@end dlgDebug event handler declarations

////@begin dlgDebug member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end dlgDebug member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin dlgDebug member variables
    wxTextCtrl* dlgDbgText;
////@end dlgDebug member variables
};

#endif
    // _DLGDEBUG_H_
