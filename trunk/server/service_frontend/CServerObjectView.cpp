/**
 * \file CServerObjectView.cpp
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

//
// Werewolf Includes
//
#include "CServerObjectView.h"
#include "CFrontendService.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/CSobManager.h"
#include "wwcommon/ISimulationObj.h"
#include <wwcommon/IGameEvent.h>
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CSobStateRequestEvent.h>
#include "CProximityManager.h"

//
// Callbacks
//

//
// Variables
//

const char* CServerObjectView::m_Name = "SOV";

//
// Class
//

CServerObjectView::CServerObjectView(WWCOMMON::ISimulationObj *owner) {
	// save the owner.
	m_OwnerSob = owner;
	std::vector<uint32> ids;
	ids.push_back(owner->getSobId());
	WWCOMMON::CGameEventServer::instance().addListener(this, ids, WWCOMMON::CGameEventServer::POST_EVENT);

	WWCOMMON::CPerformer *powner = dynamic_cast<WWCOMMON::CPerformer*>(owner);
	CProximityManager::instance().addSob(powner);
}

CServerObjectView::~CServerObjectView() {
	WWCOMMON::CGameEventServer::instance().removeListener(this, WWCOMMON::CGameEventServer::ALL_TYPES);
}

void CServerObjectView::update() {
}

void CServerObjectView::render() {
}

const char* CServerObjectView::getViewName() {
	return m_Name;
}

bool CServerObjectView::observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CServerObjectView::observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	// Our own unspawn
	if(event->getId() == EVENT_ID(CSobUnspawnEvent)) {
		if(!unspawn(dynamic_cast<WWCOMMON::CSobUnspawnEvent*>(event.getPtr())))
			return true;
	}
	return true;
}

bool CServerObjectView::spawn(WWCOMMON::CSobSpawnEvent *event) {
	if(event->TargetID != m_OwnerSob->getSobId()) {
		return false;
	}

	WWCOMMON::CPerformer *owner = dynamic_cast<WWCOMMON::CPerformer*>(m_OwnerSob);
	CProximityManager::sobGrid::List sobs = CProximityManager::instance().getSobsInRange(owner);
	CProximityManager::instance().postAddEvents(owner, &sobs);
	
	return true;	
}

bool CServerObjectView::add(WWCOMMON::CSobAddEvent *event) {
//	nldebug("Sob %d saw an add from %d to %d", m_OwnerSob->getSobId(), event->SourceID, event->TargetID);
	if(event->TargetID != m_OwnerSob->getSobId()) {
		return false;
	}

	std::vector<uint32> sobs;
	sobs.push_back(event->SourceID);
	WWCOMMON::CGameEventServer::instance().addListener(this, sobs, WWCOMMON::CGameEventServer::POST_EVENT);
	// TODO henri:everyone this bit is not very nice. But needed in order to send remove before the SOB is destroyed by CGameEventManager. Ponder a better solution.
	WWCOMMON::CGameEventServer::instance().addListener(this, EVENT_ID(CSobUnspawnEvent), event->SourceID, WWCOMMON::CGameEventServer::EVENT);

	return true;
}

bool CServerObjectView::unspawn(WWCOMMON::CSobUnspawnEvent *event) {
	if(event->TargetID == m_OwnerSob->getSobId()) {
		return false;
	}

	WWCOMMON::CSobRemoveEvent *evt = new WWCOMMON::CSobRemoveEvent();
	evt->SourceID = event->TargetID;
	evt->TargetID = m_OwnerSob->getSobId();
	WWCOMMON::CGameEventServer::instance().postEvent(evt);

	return true;
}

bool CServerObjectView::remove(WWCOMMON::CSobRemoveEvent *event) {
	if(event->TargetID != m_OwnerSob->getSobId()) {
		return false;
	}

	std::vector<uint32> sobs;
	sobs.push_back(event->SourceID);
	WWCOMMON::CGameEventServer::instance().removeListener(this, sobs, WWCOMMON::CGameEventServer::POST_EVENT);

	return true;
}

bool CServerObjectView::observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	// some other guy is spawning..should not happen..but just in case.
	if(event->getId() == EVENT_ID(CSobSpawnEvent)) {
		if(!spawn(dynamic_cast<WWCOMMON::CSobSpawnEvent*>(event.getPtr())))
			return true;

	// Add meant for another sob.
	} else if(event->getId() == EVENT_ID(CSobAddEvent)) {
		if(!add(dynamic_cast<WWCOMMON::CSobAddEvent*>(event.getPtr())))
			return true;

	// Remove meant for another sob.
	} else if(event->getId() == EVENT_ID(CSobRemoveEvent)) {
		if(!remove(dynamic_cast<WWCOMMON::CSobRemoveEvent*>(event.getPtr())))
			return true;

	} else if(event->getId() == EVENT_ID(CSobUnspawnEvent)) {
		return true;
	} else if(event->getId() == EVENT_ID(CSobStateRequestEvent)) {
		return true;
	} 

	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();

	WWCOMMON::IGameEvent *evt = event.getPtr();
	if(evt == NULL)
		return true;

//	nlinfo("Sending event %s to %i", event->getClassName().c_str(), m_OwnerSob->getSobId());
	NLNET::CMessage msgout("EVT");
	msgout.serialPolyPtr(evt);
	fs->sendMessage(msgout, m_OwnerSob->getSobId());

	return true;
}
