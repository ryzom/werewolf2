/////////////////////////////////////////////////////////////////////////////
// Name:        dlgCharList.h
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/17/05 16:10:34
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DLGCHARLIST_H_
#define _DLGCHARLIST_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "dlgCharList.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/listctrl.h"
////@end includes

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
#define ID_DLG_CHAR 10018
#define SYMBOL_DLGCHARLIST_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_DLGCHARLIST_TITLE _("Choose Character")
#define SYMBOL_DLGCHARLIST_IDNAME ID_DLG_CHAR
#define SYMBOL_DLGCHARLIST_SIZE wxSize(400, 300)
#define SYMBOL_DLGCHARLIST_POSITION wxDefaultPosition
#define ID_LIST_CHARLIST 10019
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
 * dlgCharList class declaration
 */

class dlgCharList: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( dlgCharList )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    dlgCharList( );
    dlgCharList( wxWindow* parent, wxWindowID id = SYMBOL_DLGCHARLIST_IDNAME, const wxString& caption = SYMBOL_DLGCHARLIST_TITLE, const wxPoint& pos = SYMBOL_DLGCHARLIST_POSITION, const wxSize& size = SYMBOL_DLGCHARLIST_SIZE, long style = SYMBOL_DLGCHARLIST_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DLGCHARLIST_IDNAME, const wxString& caption = SYMBOL_DLGCHARLIST_TITLE, const wxPoint& pos = SYMBOL_DLGCHARLIST_POSITION, const wxSize& size = SYMBOL_DLGCHARLIST_SIZE, long style = SYMBOL_DLGCHARLIST_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin dlgCharList event handler declarations

    /// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LIST_CHARLIST
    void OnListCharlistSelected( wxListEvent& event );

////@end dlgCharList event handler declarations

////@begin dlgCharList member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end dlgCharList member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin dlgCharList member variables
    wxListCtrl* charList;
////@end dlgCharList member variables
};

#endif
    // _DLGCHARLIST_H_
