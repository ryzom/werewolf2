/**
 * \file COrientTurnRightState.cpp
 * \date June 2005
 * \author Matt Raykowski, Henri Kuuste
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
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>

//
// Werewolf Includes
//
#include "COrientTurnRightState.h"
#include "COrientState.h"
#include "ISimulationState.h"
#include "CStateManager.h"
#include "IBaseSimulation.h"
#include "CMotionController.h"

//
// Namespaces
//

namespace WWCOMMON {

OF_REGISTER(CStateFactory, ISimulationState, COrientTurnRightState, uint32, CStateFactory::ST_OR_TURNRIGHT);

COrientTurnRightState::COrientTurnRightState() : COrientState(-1) {
//	nlinfo("Constructing a orient turn right state.");

	addTransitionable(CStateFactory::ST_OR_TURNLEFT);
	addTransitionable(CStateFactory::ST_OR_IDLE);

	// allow transistion to itself for DR updates
	// TODO henri:matt any better ideas?
	addTransitionable(CStateFactory::ST_OR_TURNRIGHT);
}

ISimulationState *COrientTurnRightState::Create() {
	return new COrientTurnRightState();
}

void COrientTurnRightState::enterState(ISimulationObj *sob) {
	COrientState::enterState(sob);
	CPerformer* performer = dynamic_cast<CPerformer*>(sob);
	nlinfo("Started turning right at yaw %.4f", performer->getCurrentKey()->getYaw());
}

void COrientTurnRightState::exitState(ISimulationObj *sob) {
	COrientState::exitState(sob);
	ISimulationObj::stateLayer* layer = sob->getStateLayer(getLayerId());
	CMotionController* controller = dynamic_cast<CMotionController*>(sob->getController("motion"));
	if(layer == NULL)
		return;
	if(!controller)
		return;
	CPerformer* performer = dynamic_cast<CPerformer*>(sob);
	nlinfo("Stopped turning right at %.4f, yaw %.4f, veloc %.3f. State lasted %.6f %.6f %.6f", getSimulation()->adjustedTime(), performer->getCurrentKey()->getYaw(), performer->getCurrentKey()->getYawSpeed(), layer->Duration, controller->Duration, layer->Duration - controller->Duration);
}

}; // END OF NAMESPACE WWCOMMON

