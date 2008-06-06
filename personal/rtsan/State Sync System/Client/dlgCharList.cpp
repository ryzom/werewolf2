/////////////////////////////////////////////////////////////////////////////
// Name:        dlgCharList.cpp
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/17/05 16:10:34
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "dlgCharList.h"
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

#include "dlgCharList.h"
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>
#include "CClient.h"

////@begin XPM images
////@end XPM images

/*!
 * dlgCharList type definition
 */

IMPLEMENT_DYNAMIC_CLASS( dlgCharList, wxDialog )

/*!
 * dlgCharList event table definition
 */

BEGIN_EVENT_TABLE( dlgCharList, wxDialog )

////@begin dlgCharList event table entries
    EVT_LIST_ITEM_SELECTED( ID_LIST_CHARLIST, dlgCharList::OnListCharlistSelected )

////@end dlgCharList event table entries

END_EVENT_TABLE()

/*!
 * dlgCharList constructors
 */

dlgCharList::dlgCharList( )
{
}

dlgCharList::dlgCharList( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * dlgCharList creator
 */

bool dlgCharList::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin dlgCharList member initialisation
    charList = NULL;
////@end dlgCharList member initialisation

////@begin dlgCharList creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end dlgCharList creation
    return TRUE;
}

/*!
 * Control creation for dlgCharList
 */

void dlgCharList::CreateControls()
{    
////@begin dlgCharList content construction

    dlgCharList* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    charList = new wxListCtrl( itemDialog1, ID_LIST_CHARLIST, wxDefaultPosition, wxSize(400, 100), wxLC_LIST  );
    itemBoxSizer2->Add(charList, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end dlgCharList content construction
}

/*!
 * Should we show tooltips?
 */

bool dlgCharList::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap dlgCharList::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin dlgCharList bitmap retrieval
    return wxNullBitmap;
////@end dlgCharList bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon dlgCharList::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin dlgCharList icon retrieval
    return wxNullIcon;
////@end dlgCharList icon retrieval
}
/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LIST_CHARLIST
 */

void dlgCharList::OnListCharlistSelected( wxListEvent& event )
{
////@begin wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LIST_CHARLIST in dlgCharList.
    // Before editing this code, remove the block markers.
//    event.Skip();
////@end wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LIST_CHARLIST in dlgCharList. 
	
	wxListItem info;
	info.SetId(event.m_itemIndex);
	info.SetColumn(0);
	info.SetMask(wxLIST_MASK_TEXT|wxLIST_MASK_DATA);
	if ( charList->GetItem(info) ) {
		nlinfo("Item Index Selected: (%d) name: %s eid: %d", event.m_itemIndex, info.GetText().c_str(), info.GetData());	
		CClient::instance().spawnEntity(info.GetData());
		Destroy();
	} else {
		nlwarning("Unable to retrieve item information.");
	}
}


