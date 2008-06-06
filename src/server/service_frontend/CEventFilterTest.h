/**
 * \file CEventFilterTest.h
 * \date June 2006
 * \author Henri Kuuste
 */

/* Copyright, 2006 Werewolf
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

#ifndef __CEVENTFILTERTEST_H__
#define __CEVENTFILTERTEST_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include <wwcommon/IGameEventListener.h>
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CSobMoveEvent.h>
#include <wwcommon/ISingleton.h>

//
// Namespaces
//

class CEventFilterTest : public WWCOMMON::ISingleton<CEventFilterTest>, WWCOMMON::IGameEventListener {
public:
	CEventFilterTest() {
		WWCOMMON::CGameEventServer::instance().addListener(this, EVENT_ID(CSobMoveEvent), 1, WWCOMMON::CGameEventServer::PRE_EVENT);
	}
	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
		nldebug("Test filter got pre-event");
		return false;
	}
	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
		nldebug("Test filter got event");
		return true;
	}
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
		nldebug("Test filter got post-event");
		return true;
	}		
};

#endif // __CEVENTFILTERTEST_H__
