/**
 * \file CClientEventManager.cpp
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

//
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//

//
// NeL Includes
//
#include <nel/net/message.h>

//
// Werewolf Includes
//	
#include "CClientEventManager.h"
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CGameSpawnRequestEvent.h>
#include <wwcommon/CGameUnspawnRequestEvent.h>
#include <wwcommon/CSobStateRequestEvent.h>
#include "tasks/CNetworkTask.h"
#include "tasks/CPreGameTask.h"
#include "CSimulationImpl.h"

//
// Namespaces
//

namespace WWCLIENT {

CClientEventManager::CClientEventManager() {
	std::vector<uint16> events;
	events.push_back(EVENT_ID(CGameSpawnRequestEvent));
	events.push_back(EVENT_ID(CGameUnspawnRequestEvent));
	events.push_back(EVENT_ID(CSobSpawnEvent));
	events.push_back(EVENT_ID(CSobAddEvent));
	events.push_back(EVENT_ID(CSobRemoveEvent));
	WWCOMMON::CGameEventServer::instance().addListener(this, events, WWCOMMON::CGameEventServer::EVENT);
}

CClientEventManager::~CClientEventManager() {
	WWCOMMON::CGameEventServer::instance().removeListener(this, WWCOMMON::CGameEventServer::ALL_TYPES);
}

bool CClientEventManager::observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CClientEventManager::observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	sendToServer(event);
	return true;
}

bool CClientEventManager::observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	if(event->getId() == EVENT_ID(CSobSpawnEvent)) {
		spawn(dynamic_cast<WWCOMMON::CSobSpawnEvent*>(event.getPtr()));
		return true;
	} else if(event->getId() == EVENT_ID(CSobAddEvent)) {
		add(dynamic_cast<WWCOMMON::CSobAddEvent*>(event.getPtr()));
		return true;
	} else if(event->getId() == EVENT_ID(CSobRemoveEvent)) {
		remove(dynamic_cast<WWCOMMON::CSobRemoveEvent*>(event.getPtr()));
		return true;
	}

	sendToServer(event);
	return true;
}

void CClientEventManager::sendToServer(WWCOMMON::CGameEventServer::EventPtr &event) {
	WWCOMMON::IGameEvent *evt = event.getPtr();
	if(evt == NULL)
		return;
	
	NLNET::CMessage msgout("EVT");
	msgout.serialPolyPtr(evt);
	CNetworkTask::instance().send(msgout);
}

void CClientEventManager::spawn(WWCOMMON::CSobSpawnEvent *event) {
	getClientSimulation()->spawnSelf(event);

	WWCOMMON::CGameEventServer::instance().addListener(this, EVENT_ID(CSobStateRequestEvent), event->TargetID, WWCOMMON::CGameEventServer::POST_EVENT);

	CPreGameTask::instance().startGame();
}

void CClientEventManager::add(WWCOMMON::CSobAddEvent *event) {
	getClientSimulation()->addSob(event);
}

void CClientEventManager::remove(WWCOMMON::CSobRemoveEvent* event) {
	getClientSimulation()->removeSob(event->SourceID);
}

void CClientEventManager::logOut() {
}

}; // END NAMESPACE WWCLIENT
