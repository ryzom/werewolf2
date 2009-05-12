/**
 * \file CPerformerDRController.cpp
 * \date February 2006
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
#include "CPerformerDRController.h"
#include <wwcommon/CGameEventServer.h>
#include "CServerSimulation.h"
#include <wwcommon/CSobMoveEvent.h>
#include <wwcommon/CSobOrientEvent.h>
#include <wwcommon/CSobStrafeEvent.h>
#include <wwcommon/CSobDREvent.h>
#include <wwcommon/CStateManager.h>
#include <wwcommon/ISimulationState.h>

//
// Namespaces
//

#define IDLE_THRESHOLD 0.001

const char* CPerformerDRController::m_Name = "dead reckoning";

CPerformerDRController::CPerformerDRController(WWCOMMON::CPerformer* subject, double timeout) :
m_Subject(subject), m_Timeout(timeout), m_NextUpdateTime(0), m_ForceUpdate(false) {
	std::vector<uint16> events;
	events.push_back(EVENT_ID(CSobMoveEvent));
	events.push_back(EVENT_ID(CSobOrientEvent));
	events.push_back(EVENT_ID(CSobStrafeEvent));
	WWCOMMON::CGameEventServer::instance().addListener(this, events, subject->getSobId(), WWCOMMON::CGameEventServer::POST_EVENT);
}

CPerformerDRController::~CPerformerDRController() {
	WWCOMMON::CGameEventServer::instance().removeListener(this, WWCOMMON::CGameEventServer::ALL_TYPES);
}

void CPerformerDRController::update() {
	if(m_NextUpdateTime > getSimulation()->adjustedTime()) {
		return;
	}
/*
	if(!m_ForceUpdate && checkIdle()) {
		m_NextUpdateTime = getSimulation()->adjustedTime() + m_Timeout;
		return;
	}
*/
	sendDREvent();

	if(m_ForceUpdate)
		m_ForceUpdate = false;
}

const char* CPerformerDRController::getControllerName() {
	return m_Name;
}

bool CPerformerDRController::observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CPerformerDRController::observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CPerformerDRController::observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	sendDREvent();
	return true;
}

void CPerformerDRController::sendDREvent() {
	WWCOMMON::CSobDREvent* event = new WWCOMMON::CSobDREvent();
	event->TargetID = m_Subject->getSobId();
	event->SourceID = m_Subject->getSobId();
	fillDREvent(event);
	WWCOMMON::CGameEventServer::instance().postEvent(event);

	m_NextUpdateTime = getSimulation()->adjustedTime() + m_Timeout;
}

void CPerformerDRController::fillDREvent(WWCOMMON::CSobDREvent* event) {
	if(event == NULL)
		return;
	event->Position = m_Subject->getCurrentKey()->getPosition();
	event->Yaw = m_Subject->getCurrentKey()->getYaw();
	event->YawVelocity = m_Subject->getCurrentKey()->getYawSpeed();
	event->DrTime = getSimulation()->adjustedTime();
}

bool CPerformerDRController::checkIdle() {
	if((m_Subject->getCurrentKey()->getSpeed().norm() < IDLE_THRESHOLD) && (m_Subject->getCurrentKey()->getYawSpeed() < IDLE_THRESHOLD)) {
		return true;
	}
	return false;
}