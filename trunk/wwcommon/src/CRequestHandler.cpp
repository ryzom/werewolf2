/**
 * \file CRequestHandler.cpp
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
#include "wwcommon/CRequestHandler.h"
#include "wwcommon/CStateManager.h"
#include "wwcommon/ISimulationState.h"
#include "wwcommon/CSobMoveEvent.h"
#include "wwcommon/CSobOrientEvent.h"
#include "wwcommon/CSobStrafeEvent.h"
#include "wwcommon/ISobStateChangeEvent.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/CPerformer.h"


//
// Namespaces
//
namespace WWCOMMON {

OF_REGISTER(CSobHandlerFactory, ISobHandler, CRequestHandler, uint32, CSobHandlerFactory::H_MOTION_REQUEST);

CRequestHandler::CRequestHandler() {
	setPriority(0);
	m_HandledEvents.push_back(CSobStateRequestEvent::CSobStateRequestEventID);
}

CRequestHandler::~CRequestHandler() {
	;
}

CRequestHandler::eventList* CRequestHandler::getEventList() {
	return &m_HandledEvents;
}

bool CRequestHandler::handleSobEvent(NLMISC::CSmartPtr<ISobEvent> event, ISimulationObj* subject) {
	if(event->getId() == CSobStateRequestEvent::CSobStateRequestEventID) {
		return handleStateRequestEvent(dynamic_cast<CSobStateRequestEvent&>(*event), subject);
	} else {
		return false;
	}
}

bool CRequestHandler::handleStateRequestEvent(CSobStateRequestEvent& event, ISimulationObj* subject) {
	ISimulationState* state = CStateFactory::instance().getState(event.TargetState);
	ISobStateChangeEvent* pNewEvent;
	if(state->getLayerId() == CStateFactory::ST_MOVEMENT) {
		pNewEvent = new CSobMoveEvent();
	} else if(state->getLayerId() == CStateFactory::ST_ORIENTATION) {
		pNewEvent = new CSobOrientEvent();
	} else if(state->getLayerId() == CStateFactory::ST_STRAFE) {
		pNewEvent = new CSobStrafeEvent();
	} else {
		return false;
	}

	ISimulationObj::stateLayer* layer = subject->getStateLayer(state->getLayerId());
	if(layer) {
		pNewEvent->StateDuration = layer->Duration;
	} else {
		pNewEvent->StateDuration = 0.0f;
	}
	pNewEvent->TargetState = event.TargetState;

	pNewEvent->SourceID = event.SourceID;
	pNewEvent->TargetID = event.TargetID;
	pNewEvent->Timestamp = getSimulation()->adjustedTime();

	CGameEventServer::instance().postEvent(pNewEvent);
	return true;
}

}; // END OF NAMESPACE WWCOMMON