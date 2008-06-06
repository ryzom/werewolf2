/**
 * \file wxDisplayer.h
 * \date February 2005
 * \author Henri Kuuste
 */

/* Copyright, 2004 Werewolf
 *
 * This file is part of Werewolf.
 * Werewolf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Werewolf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Werewolf; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#ifdef NL_OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif
#include <nel/net/callback_client.h>
//#include <nel/net/udp_sock.h>
//#include <nel/net/login_cookie.h>
//#include <nel/misc/system_info.h> 
//#include <nel/net/login_client.h>
#include <nel/misc/displayer.h>
#include <nel/misc/path.h> 

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

//
// Werewolf Includes
//	
//#include "../Shared/CState.h"
//#include "../Shared/ISingleton.h"
#include "wxDisplayer.h"
#include "dlgDebug.h"


//
// Namespaces
//

CWxDisplayer::CWxDisplayer(dlgDebug *dlgDebug, bool eraseLastLog, const char *displayerName, bool raw) 
: NLMISC::IDisplayer (displayerName), _NeedHeader(true), _LastLogSizeChecked(0), _Raw(raw) {
	setParam(dlgDebug,eraseLastLog);
}

CWxDisplayer::CWxDisplayer() : IDisplayer (""), _NeedHeader(true), _LastLogSizeChecked(0), _Raw(false) {
	;
}

CWxDisplayer::~CWxDisplayer() {
	;
}

void CWxDisplayer::setParam (dlgDebug *dlgDebug, bool eraseLastLog) {
	m_DlgDebug=dlgDebug;
	//dlgDebug->dlgDbgText->WriteText("test");
}
 
void CWxDisplayer::doDisplay ( const NLMISC::CLog::TDisplayInfo& args, const char *message ) {
	bool needSpace = false;
	std::string str;

	if(m_DlgDebug==NULL)
		return;

	if (args.Date != 0 && !_Raw) {
		str += dateToHumanString(args.Date);
		needSpace = true;
	}

	if (args.LogType != NLMISC::CLog::LOG_NO && !_Raw) {
		if (needSpace) { str += " "; needSpace = false; }
		str += logTypeToString(args.LogType);
		needSpace = true;
	}

	// Write thread identifier
	if ( args.ThreadId != 0 && !_Raw) {
		if (needSpace) { str += " "; needSpace = false; }
		str += NLMISC::toString(args.ThreadId);
		needSpace = true;
	}
	if (!args.ProcessName.empty() && !_Raw) {
		if (needSpace) { str += " "; needSpace = false; }
		str += args.ProcessName;
		needSpace = true;
	}

	if (args.FileName != NULL && !_Raw) {
		if (needSpace) { str += " "; needSpace = false; }
		str += NLMISC::CFile::getFilename(args.FileName);
		needSpace = true;
	}

	if (args.Line != -1 && !_Raw) {
		if (needSpace) { str += " "; needSpace = false; }
		str += NLMISC::toString(args.Line);
		needSpace = true;
	}
      
	if (args.FuncName != NULL && !_Raw) {
		if (needSpace) { str += " "; needSpace = false; }
		str += args.FuncName;
		needSpace = true;
	}

	if (needSpace) { str += " : "; needSpace = false; }
	str += message;

	m_DlgDebug->dlgDbgText->AppendText(str.c_str());	
}
