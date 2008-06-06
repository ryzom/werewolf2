/////////////////////////////////////////////////////////////////////////////
// Name:        client.h
// Purpose:     
// Author:      Matt Raykowski
// Modified by: 
// Created:     03/10/05 14:52:49
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _CLIENT_H_
#define _CLIENT_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "client.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "frmClient.h"
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
////@end control identifiers

/*!
 * ClientApp class declaration
 */

class ClientApp: public wxApp
{    
    DECLARE_CLASS( ClientApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    ClientApp();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin ClientApp event handler declarations

////@end ClientApp event handler declarations

////@begin ClientApp member function declarations
////@end ClientApp member function declarations
	virtual void OnTimer(wxTimerEvent &e);
	wxTimer *m_Timer;
	wxTimer *GetTimer() const { return m_Timer ; }
    void SetTimer(wxTimer *value) { m_Timer = value ; }
////@begin ClientApp member variables
////@end ClientApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(ClientApp)
////@end declare app

#endif
    // _CLIENT_H_
