/**
 * \file CClientMotionHandler.cpp
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
#include "wwcommon/CClientMotionHandler.h"
#include "wwcommon/IBaseSimulation.h"

//
// Namespaces
//
namespace WWCOMMON {
OF_REGISTER(CSobHandlerFactory, ISobHandler, CClientMotionHandler, uint32, CSobHandlerFactory::H_CLIENT_MOTION);

CClientMotionHandler::CClientMotionHandler() : CMotionHandler() {
	m_HandledEvents.push_back(EVENT_ID(CSobDREvent));
}

bool CClientMotionHandler::handleSobEvent(NLMISC::CSmartPtr<ISobEvent> event, ISimulationObj* subject) {
	bool result = CMotionHandler::handleSobEvent(event, subject);
	if(event->getId() == EVENT_ID(CSobDREvent)) {
		handleDREvent(dynamic_cast<CSobDREvent*>(event.getPtr()), dynamic_cast<CPerformer*>(subject));
	}
	return result;
}

void CClientMotionHandler::handleDREvent(CSobDREvent* event, CPerformer* subject) {
	double time = getSimulation()->adjustedTime() - event->DrTime;
	if(time > 0) {
		nlinfo("LAAAAAAG!");
	}

	CPerformer::CorrectInterpKey key (event->Position, event->Yaw, event->YawVelocity, event->DrTime);

	// key it
	subject->getCorrectionInterp()->addKey(key);
}

}; // END OF NAMESPACE WWCOMMON