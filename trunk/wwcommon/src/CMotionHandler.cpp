/**
 * \file CMotionHandler.cpp
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
#include "wwcommon/CMotionHandler.h"
#include "wwcommon/ISimulationState.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/CStateManager.h"

//
// Namespaces
//

namespace WWCOMMON {

OF_REGISTER(CSobHandlerFactory, ISobHandler, CMotionHandler, uint32, CSobHandlerFactory::H_MOTION);

CMotionHandler::CMotionHandler() {
	setPriority(0);
	m_HandledEvents.push_back(EVENT_ID(CSobMoveEvent));
	m_HandledEvents.push_back(EVENT_ID(CSobOrientEvent));
	m_HandledEvents.push_back(EVENT_ID(CSobStrafeEvent));
}

CMotionHandler::~CMotionHandler() {
	;
}

CMotionHandler::eventList* CMotionHandler::getEventList() {
	return &m_HandledEvents;
}

bool CMotionHandler::handleSobEvent(NLMISC::CSmartPtr<ISobEvent> event, ISimulationObj* subject) {
	if(event->getId() == CSobMoveEvent::CSobMoveEventID) {
		return changeState(dynamic_cast<ISobStateChangeEvent*>(event.getPtr()), subject);
	} else if(event->getId() == CSobOrientEvent::CSobOrientEventID) {
		return changeState(dynamic_cast<ISobStateChangeEvent*>(event.getPtr()), subject);
	} else if(event->getId() == CSobStrafeEvent::CSobStrafeEventID) {
		return changeState(dynamic_cast<ISobStateChangeEvent*>(event.getPtr()), subject);
	} else {
		return false;
	}
}

bool CMotionHandler::changeState(ISobStateChangeEvent* event, ISimulationObj* subject) {
	if(event == NULL)
		return false;
//	nlinfo("Handling motion event with timestamp %.4f at time %.4f", event->Timestamp, getSimulation()->adjustedTime());
	ISimulationState* targetState = CStateFactory::instance().getState(event->TargetState);
	return subject->changeState(targetState, event->StateDuration, event->Timestamp - (getSimulation()->adjustedTime() - getSimulation()->deltaTime()));
}


}; // END OF NAMESPACE WWCOMMON