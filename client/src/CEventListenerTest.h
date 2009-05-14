/**
 * \file CEventListenerTest.h
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

#ifndef __CEVENTLISTENERTEST_H__
#define __CEVENTLISTENERTEST_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/debug.h>

//
// Werewolf Includes
//
#include <wwcommon/IGameEventListener.h>
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/IGameEvent.h>
#include <wwcommon/CSobStateRequestEvent.h>
#include <wwcommon/ISimulationState.h>
#include <wwcommon/CStateManager.h>
#include <wwcommon/CSobMoveEvent.h>
#include <wwcommon/CSobOrientEvent.h>
#include "CSimulationImpl.h"

//
// Namespaces
//

namespace WWCLIENT {

class CEventListenerTest : public WWCOMMON::IGameEventListener {
public:
	CEventListenerTest() {
		std::vector<uint16> events;
		events.push_back(WWCOMMON::CSobStateRequestEvent::CSobStateRequestEventID);
		WWCOMMON::CGameEventServer::instance().addListener(this, WWCOMMON::CGameEventServer::POST_EVENT);
		WWCOMMON::CGameEventServer::instance().addListener(this, WWCOMMON::CGameEventServer::PRE_EVENT);
	}

	~CEventListenerTest() {
		WWCOMMON::CGameEventServer::instance().removeListener(this, WWCOMMON::CGameEventServer::ALL_TYPES);
	}

	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
		if(event->getId() == WWCOMMON::CSobStateRequestEvent::CSobStateRequestEventID) {
			const WWCOMMON::CSobStateRequestEvent* req = dynamic_cast<WWCOMMON::CSobStateRequestEvent*>(event.getPtr());
			nlinfo("PRE: Observed state request: %d %d", event->getId(), req->TargetState);
		} else {
			nlinfo("PRE: Observed event: %d", event->getId());
		}
		return true;
	}
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
		if(event->getId() == WWCOMMON::CSobStateRequestEvent::CSobStateRequestEventID) {
			const WWCOMMON::CSobStateRequestEvent* req = dynamic_cast<WWCOMMON::CSobStateRequestEvent*>(event.getPtr());
			nlinfo("POST: Observed state request: %d %d", event->getId(), req->TargetState);
		} else {
			nlinfo("POST: Observed event: %d", event->getId());
		}
		return true;
	}

	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
		return true;
	}
};

}; // END NAMESPACE WWCLIENT

#endif // __CEVENTLISTENERTEST_H__
