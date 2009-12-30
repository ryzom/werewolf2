/**
 * \file CLocalObjectView.cpp
 * \date February 2006
 * \author Matt Raykowski
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
// System Includes
//
#include "stdpch.h"

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/message.h>

//
// Werewolf Includes
//
#include "CLocalObjectView.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/CSobManager.h"
#include "wwcommon/ISimulationObj.h"
#include <wwcommon/IGameEvent.h>
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CSobStateRequestEvent.h>
#include <wwcommon/CGameChatEvent.h>
#include "tasks/CNetworkTask.h"

//
// Callbacks
//

//
// Variables
//

//
// Class
//

namespace WWCLIENT {

CLocalObjectView::CLocalObjectView(WWCOMMON::ISimulationObj *owner) {
	// save the owner.
	m_OwnerSob = owner;

	// Create a vector of sob events we'll be sending.
	std::vector<uint16> sobEvents;
	sobEvents.push_back(EVENT_ID(CSobStateRequestEvent));

	WWCOMMON::CGameEventServer::instance().addListener(this, sobEvents, owner->getSobId(), WWCOMMON::CGameEventServer::POST_EVENT);
}

CLocalObjectView::~CLocalObjectView() {
	WWCOMMON::CGameEventServer::instance().removeListener(this, WWCOMMON::CGameEventServer::ALL_TYPES);
}

void CLocalObjectView::update() {

}

bool CLocalObjectView::observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CLocalObjectView::observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	//nlinfo("Received event type: %d", event->getClassName().c_str());

	WWCOMMON::IGameEvent *evt = event.getPtr();
	if(evt == NULL)
		return true;
	
	NLNET::CMessage msgout("EVT");
	msgout.serialPolyPtr(evt);
	CNetworkTask::instance().send(msgout);
	return true;
}

bool CLocalObjectView::observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

}; // END NAMESPACE WWCLIENT
