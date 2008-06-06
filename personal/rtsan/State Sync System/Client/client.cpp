/////////////////////////////////////////////////////////////////////////////
// Name:        client.cpp
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/10/05 14:52:49
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "client.h"
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

#include "client.h"
#include "CClient.h"

////@begin XPM images
////@end XPM images

#define TIMER_ID			5
#define WW_NETWORK_UPDATE	100
/*!
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( ClientApp )
////@end implement app

/*!
 * ClientApp type definition
 */

IMPLEMENT_CLASS( ClientApp, wxApp )

/*!
 * ClientApp event table definition
 */

BEGIN_EVENT_TABLE( ClientApp, wxApp )

////@begin ClientApp event table entries
////@end ClientApp event table entries
EVT_TIMER(TIMER_ID,ClientApp::OnTimer) 
END_EVENT_TABLE()

/*!
 * Constructor for ClientApp
 */

ClientApp::ClientApp()
{
////@begin ClientApp member initialisation
////@end ClientApp member initialisation
}

/*!
 * Initialisation for ClientApp
 */

bool ClientApp::OnInit()
{    
////@begin ClientApp initialisation
    // Remove the comment markers above and below this block
    // to make permanent changes to the code.

#if wxUSE_XPM
    wxImage::AddHandler( new wxXPMHandler );
#endif
#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler( new wxJPEGHandler );
#endif
#if wxUSE_GIF
    wxImage::AddHandler( new wxGIFHandler );
#endif
    frmClient* mainWindow = new frmClient( NULL, ID_FRAME );
    mainWindow->Show(true);
////@end ClientApp initialisation
    SetTopWindow(mainWindow);
	CClient::instance().init();
	SetTimer(new wxTimer(this, TIMER_ID));
	GetTimer()->Start(WW_NETWORK_UPDATE);
    return true;
}

/*!
 * Catch timer events to process my NeL stuff.
 */
void ClientApp::OnTimer(wxTimerEvent &e) {
	//nlinfo("Timer update!");
	CClient::instance().update();
}

/*!
 * Cleanup for ClientApp
 */
int ClientApp::OnExit()
{    
////@begin ClientApp cleanup
    return wxApp::OnExit();
////@end ClientApp cleanup
}

