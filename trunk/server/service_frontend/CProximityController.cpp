/**
 * \file CProximityController.cpp
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
#include "CProximityController.h"
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CSobMoveEvent.h>
#include <wwcommon/CSobStrafeEvent.h>
#include "CServerSimulation.h"
#include <wwcommon/CStateManager.h>
#include <wwcommon/ISimulationState.h>

//
// Namespaces
//

const char* CProximityController::m_Name = "proximity";


CProximityController::CProximityController(WWCOMMON::CPerformer* subject, double idlePeriod)
	: m_Subject(subject), m_NextUpdateTime(0), m_IdlePeriod(idlePeriod) {
	std::vector<uint16> events;
	events.push_back(EVENT_ID(CSobMoveEvent));
	events.push_back(EVENT_ID(CSobStrafeEvent));
	WWCOMMON::CGameEventServer::instance().addListener(this, events, subject->getSobId(), WWCOMMON::CGameEventServer::POST_EVENT);

	updateIdle();

	m_Position = CProximityManager::instance().getSobPosition(subject);
}

CProximityController::~CProximityController() {
	WWCOMMON::CGameEventServer::instance().removeListener(this, WWCOMMON::CGameEventServer::ALL_TYPES);
}

const char* CProximityController::getControllerName() {
	return m_Name;
}

void CProximityController::update() {
	if(m_NextUpdateTime > getSimulation()->adjustedTime()) {
		return;
	}

	registerForUpdate();
}

bool CProximityController::observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CProximityController::observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CProximityController::observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	updateIdle();
	registerForUpdate();
	return true;
}

void CProximityController::registerForUpdate() {
	CProximityManager::sobGrid::Position newPos = CProximityManager::instance().getSobPosition(m_Subject);
	if(m_Position.x != newPos.x || m_Position.y != newPos.y) {
		CProximityManager::instance().registerSobForUpdate(m_Subject);
	}

	if(m_Idle) {
		m_NextUpdateTime = getSimulation()->adjustedTime() + m_IdlePeriod;
	} else {
		m_NextUpdateTime = getSimulation()->adjustedTime() + CProximityManager::instance().getUpdateFrequency(m_Subject);
	}
}

void CProximityController::updateIdle() {
	WWCOMMON::ISimulationState *motion = m_Subject->getState(WWCOMMON::CStateFactory::ST_MOVEMENT);
	WWCOMMON::ISimulationState *strafe = m_Subject->getState(WWCOMMON::CStateFactory::ST_STRAFE);

	m_Idle = ((motion == NULL) || (motion->getStateId() == WWCOMMON::CStateFactory::ST_MV_IDLE)) && ((strafe == NULL) || (strafe->getStateId() == WWCOMMON::CStateFactory::ST_ST_IDLE));
}

CProximityManager::sobGrid::Position CProximityController::getPosition() {
	return m_Position;
}

void CProximityController::setPosition(CProximityManager::sobGrid::Position pos) {
	m_Position = pos;
}
