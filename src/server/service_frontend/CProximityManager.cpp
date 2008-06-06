/**
 * \file CProximityManager.cpp
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
#include <cmath>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "CProximityManager.h"
#include "CProximityController.h"
#include <wwcommon/CSobAddEvent.h>
#include <wwcommon/CSobRemoveEvent.h>
#include <wwcommon/CGameEventServer.h>


//
// Namespaces
//

CProximityManager::CProximityManager() : m_ProximityRange(100) {
	// TODO henri:henri create an init method to initialize range and the grid and stuff...
	m_Grid = new sobGrid(2000, 200);
	m_Origin.x = (uint)std::ceil(400/m_Grid->getCellWidth());
	m_Origin.y = (uint)std::ceil(400/m_Grid->getCellWidth());
}

CProximityManager::~CProximityManager() {
	delete m_Grid;
}

void CProximityManager::addSob(WWCOMMON::CPerformer *sob) {
	m_Grid->insert(sob, getSobPosition(sob));
}

void CProximityManager::removeSob(WWCOMMON::CPerformer* sob) {
	m_Grid->remove(sob, getSobPosition(sob));
}

CProximityManager::sobGrid::List CProximityManager::getSobsInRange(WWCOMMON::CPerformer *sob) {
	return m_Grid->getRange(getSobPosition(sob), m_ProximityRange);
}

void CProximityManager::registerSobForUpdate(WWCOMMON::CPerformer *sob) {
	m_SobsToUpdate.insert(sob);
}

void CProximityManager::update() {
	if(m_SobsToUpdate.empty())
		return;

	CProximityManager::sobGrid::Position* oldPosArray = new CProximityManager::sobGrid::Position[m_SobsToUpdate.size()];

	CProximityManager::sobList::iterator iter = m_SobsToUpdate.begin();
	for( ; iter != m_SobsToUpdate.end(); ++iter) {
		WWCOMMON::CPerformer *sob = (*iter);

		// get old range and send removes
		CProximityManager::sobGrid::List oldSobs = m_Grid->getOldRange(getStoredSobPosition(sob), getSobPosition(sob), m_ProximityRange);
		postRemoveEvents(sob, &oldSobs);

	}
	iter = m_SobsToUpdate.begin();
	uint i = 0;
	for( ; iter != m_SobsToUpdate.end(); ++iter) {
		WWCOMMON::CPerformer *sob = (*iter);
//		nldebug("Updating sob %d proximity.", sob->getSobId());

		// change position
		CProximityController *ctrl = dynamic_cast<CProximityController*>(sob->getController("proximity"));
		m_Grid->remove(sob, ctrl->getPosition());
		oldPosArray[i] = ctrl->getPosition();
//		nldebug("Moving from %d %d", ctrl->getPosition().x, ctrl->getPosition().y);
		ctrl->setPosition(getSobPosition(sob));
//		nldebug("Moving to   %d %d", ctrl->getPosition().x, ctrl->getPosition().y);
		m_Grid->insert(sob, ctrl->getPosition());

		++i;
	}
	iter = m_SobsToUpdate.begin();
	i = 0;
	for( ; iter != m_SobsToUpdate.end(); ++iter) {
		WWCOMMON::CPerformer *sob = (*iter);

		// get new range and send adds
		CProximityManager::sobGrid::List newSobs = m_Grid->getNewRange(oldPosArray[i], getSobPosition(sob), m_ProximityRange);
		postAddEvents(sob, &newSobs);

		++i;
	}

	delete[] oldPosArray;
	// clear the update list
	m_SobsToUpdate.clear();
}

double CProximityManager::getUpdateFrequency(WWCOMMON::CPerformer *sob) {
	return m_Grid->getCellWidth()/(2.0*sob->getMaxSpeed());
}

CProximityManager::sobGrid::Position CProximityManager::getSobPosition(WWCOMMON::CPerformer *sob) {
	CProximityManager::sobGrid::Position pos;
	pos.x = (uint)std::ceil(abs(sob->getPosition().x)/m_Grid->getCellWidth()) - m_Origin.x;
	pos.y = (uint)std::ceil(abs(sob->getPosition().y)/m_Grid->getCellWidth()) - m_Origin.y;
	return pos;
}

CProximityManager::sobGrid::Position CProximityManager::getStoredSobPosition(WWCOMMON::CPerformer *sob) {
	CProximityController *ctrl = dynamic_cast<CProximityController*>(sob->getController("proximity"));
	if(ctrl) {
		return ctrl->getPosition();
	}

	return getSobPosition(sob);
}

void CProximityManager::postAddEvents(WWCOMMON::CPerformer* sob, CProximityManager::sobGrid::List* list) {
	CProximityManager::sobGrid::List::iterator listIter = list->begin();
	for( ; listIter != list->end(); ++listIter) {
		CProximityManager::sobGrid::DataList::iterator sobIter = (*listIter)->begin();
		for( ; sobIter != (*listIter)->end(); ++sobIter) { 
			WWCOMMON::CPerformer *sob2 = (*sobIter);

			if(sob2->getSobId() == sob->getSobId())
				continue;

			nldebug("Found sob %d in range of sob %d", sob2->getSobId(), sob->getSobId());

			// send one to the other guy
			WWCOMMON::CSobAddEvent *evt = new WWCOMMON::CSobAddEvent();
			evt->SourceID = sob->getSobId();
			evt->TargetID = sob2->getSobId();
			evt->Position = sob->getPosition();
			evt->Speed = sob->getCurrentKey()->getSpeed();
			evt->Yaw = sob->getCurrentKey()->getYaw();
			evt->YawVelocity = sob->getCurrentKey()->getYawSpeed();
			evt->EMD = "gnu";
			evt->ActiveStateList = sob->getActiveStateList();
			WWCOMMON::CGameEventServer::instance().postEvent(evt);

			// send one to my owner
			evt = new WWCOMMON::CSobAddEvent();
			evt->SourceID = sob2->getSobId();
			evt->TargetID = sob->getSobId();
			evt->Position = sob2->getPosition();
			evt->Speed = sob2->getCurrentKey()->getSpeed();
			evt->Yaw = sob2->getCurrentKey()->getYaw();
			evt->YawVelocity = sob2->getCurrentKey()->getYawSpeed();
			evt->EMD = "gnu";
			evt->ActiveStateList = sob2->getActiveStateList();
			WWCOMMON::CGameEventServer::instance().postEvent(evt);
		}
	}
}

void CProximityManager::postRemoveEvents(WWCOMMON::CPerformer* sob, CProximityManager::sobGrid::List* list) {
	CProximityManager::sobGrid::List::iterator listIter = list->begin();
	for( ; listIter != list->end(); ++listIter) {
		CProximityManager::sobGrid::DataList::iterator sobIter = (*listIter)->begin();
		for( ; sobIter != (*listIter)->end(); ++sobIter) { 
			WWCOMMON::CPerformer *sob2 = (*sobIter);

			if(sob2->getSobId() == sob->getSobId())
				continue;

			nldebug("Sob %d left the range of sob %d", sob2->getSobId(), sob->getSobId());

			// send one to the other guy
			WWCOMMON::CSobRemoveEvent *evt = new WWCOMMON::CSobRemoveEvent();
			evt->SourceID = sob->getSobId();
			evt->TargetID = sob2->getSobId();
			WWCOMMON::CGameEventServer::instance().postEvent(evt);

			// send one to my owner
			evt = new WWCOMMON::CSobRemoveEvent();
			evt->SourceID = sob2->getSobId();
			evt->TargetID = sob->getSobId();
			WWCOMMON::CGameEventServer::instance().postEvent(evt);
		}
	}

}
