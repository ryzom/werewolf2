/**
 * \file CStrafeState.cpp
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
#include <nel/misc/common.h> // clamp() in here

//
// Werewolf Includes
//
#include "wwcommon/CStrafeState.h"
#include "wwcommon/ISimulationState.h"
#include "wwcommon/CStateManager.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/CMotionController.h"

//
// Namespaces
//

namespace WWCOMMON {

CStrafeState::CStrafeState(int direction) : ISimulationState() {
	NLMISC::clamp(direction, -1, 1);
	m_direction = direction;
//	nlinfo("Constructing a strafe state.");
}

void CStrafeState::handle(ISimulationObj *sob, float elapsed) {
	incrementDuration(sob, elapsed);

	CPerformer* actor = dynamic_cast<CPerformer*>(sob);

	NLMISC::CVector speed=actor->getCurrentKey()->getSpeed();				// holds the adjusted speed.	
	float maxSpeed=actor->getMaxSpeed();
	float acceleration=actor->getAcceleration();
	float brakingFactor=actor->getBrakingFactor();

	// calculate a forward motion change.
	speed.x = m_direction==0 ? /*speed.y*elapsed*deceleration*/ 0 : /* speed.y+acceleration*elapsed*m_direction */ m_direction*maxSpeed;

	// clamp speed to a reasonable value.
	NLMISC::clamp(speed.x, -maxSpeed, maxSpeed);

	actor->getCurrentKey()->setSpeed(speed);
}

void CStrafeState::updateAll(ISimulationObj *sob, double dt) {
	update(sob, dt);
	ISimulationState* state = sob->getState(CStateFactory::ST_MOVEMENT);
	if(state)
		state->update(sob, dt);
	state = sob->getState(CStateFactory::ST_ORIENTATION);
	if(state)
		state->update(sob, dt);
	CMotionController* controller = dynamic_cast<CMotionController*>(sob->getController("motion"));
	if(controller)
		controller->update(dt);
}

void CStrafeState::enterState(ISimulationObj* sob) {
	CMotionController* controller = dynamic_cast<CMotionController*>(sob->getController("motion"));
	if(controller)
		controller->Duration = 0;
}

}; // END OF NAMESPACE WWCOMMON
