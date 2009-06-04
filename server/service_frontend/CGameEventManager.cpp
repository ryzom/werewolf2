/**
 * \file CGameEventManager.cpp
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

//
// Werewolf Includes
//	
#include "CGameEventManager.h"
#include <wwcommon/CGameEventServer.h>
#include "CServerSimulation.h"
#include "CFrontendService.h"

//
// Namespaces
//

CGameEventManager::CGameEventManager() {
	std::vector<uint16> events;
	events.push_back(EVENT_ID(CGameSpawnRequestEvent));
	events.push_back(EVENT_ID(CGameUnspawnRequestEvent));
	WWCOMMON::CGameEventServer::instance().addListener(this, events, WWCOMMON::CGameEventServer::EVENT);

	events.clear();
	events.push_back(EVENT_ID(CSobUnspawnEvent));
	WWCOMMON::CGameEventServer::instance().addListener(this, events, WWCOMMON::CGameEventServer::POST_EVENT);
}

CGameEventManager::~CGameEventManager() {
	WWCOMMON::CGameEventServer::instance().removeListener(this, WWCOMMON::CGameEventServer::ALL_TYPES);
}

bool CGameEventManager::observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CGameEventManager::observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	if(event->getId() == EVENT_ID(CSobUnspawnEvent)) {
		unspawn(dynamic_cast<WWCOMMON::CSobUnspawnEvent*>(event.getPtr()));
	}

	return true;
}

bool CGameEventManager::observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	if(event->getId() == EVENT_ID(CGameSpawnRequestEvent)) {
		spawn(dynamic_cast<WWCOMMON::CGameSpawnRequestEvent*>(event.getPtr()));
	} else if(event->getId() == EVENT_ID(CGameUnspawnRequestEvent)) {
		postUnspawn(dynamic_cast<WWCOMMON::CGameUnspawnRequestEvent*>(event.getPtr()));
	}
	return true;
}

void CGameEventManager::spawn(WWCOMMON::CGameSpawnRequestEvent *event) {
	getServerSimulation()->userLogin(event->getPlayerID(), event->CharacterID);
}

void CGameEventManager::postUnspawn(WWCOMMON::CGameUnspawnRequestEvent *event) {
	WWCOMMON::CSobUnspawnEvent *evt = new WWCOMMON::CSobUnspawnEvent();
	evt->SourceID = event->SobID;
	evt->TargetID = event->SobID;
	WWCOMMON::CGameEventServer::instance().postEvent(evt);
}

void CGameEventManager::unspawn(WWCOMMON::CSobUnspawnEvent *event) {
	// TODO henri:everyone persist sob here
	getServerSimulation()->userLogout(event->TargetID);
}
