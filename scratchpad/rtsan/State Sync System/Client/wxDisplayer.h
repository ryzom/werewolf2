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

#ifndef __SSS_WXDISPLAYER_H__
#define __SSS_WXDISPLAYER_H__

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

#include "wx/dialog.h"

#include "dlgDebug.h"

//
// Werewolf Includes
//	
//#include "../Shared/CState.h"
//#include "../Shared/ISingleton.h"



//
// Namespaces
//

class CWxDisplayer : virtual public NLMISC::IDisplayer
{
public:
	CWxDisplayer(dlgDebug *dlgDebug, bool eraseLastLog = false, const char *displayerName = "", bool raw = false);
	CWxDisplayer();
	~CWxDisplayer ();
	void setParam (dlgDebug *dlgDebug, bool eraseLastLog = false);
 
protected:
	virtual void doDisplay ( const NLMISC::CLog::TDisplayInfo& args, const char *message );

private:
	dlgDebug		*m_DlgDebug;
	bool            _NeedHeader;
	uint            _LastLogSizeChecked;
	bool            _Raw;
};

#endif // __SSS_WXDISPLAYER_H__