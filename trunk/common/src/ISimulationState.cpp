/**
 * \file ISimulationState.cpp
 * \date March 2006
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
#include "ISimulationState.h"
#include "CStateManager.h"
#include "IBaseSimulation.h"

//
// Namespaces
//

namespace WWCOMMON {

ISimulationState::ISimulationState() : m_Blocked(false) {
	;
}

ISimulationState::~ISimulationState() {
	;
}

uint32 ISimulationState::getStateId() {
	return CStateFactory::ST_NONE;
}


void ISimulationState::addTransitionable(uint32 state) {
	m_Transitions.push_back(state);
}

bool ISimulationState::canTransition(uint32 state) {
	if(m_Blocked) {
		nlinfo("State transition blocked!");
		return false;
	}

	for(std::list<uint32>::iterator itr=m_Transitions.begin(); itr != m_Transitions.end() ; itr++) {
		if( (*itr) == state )
			return true;
	}

	nlinfo("Cannot transition to state: %d", state);
	return false;
}

void ISimulationState::enter(ISimulationObj *sob) {
	ISimulationObj::stateLayer* layer = sob->getStateLayer(getLayerId());
	if(layer == NULL)
		return;
//	nlinfo("Duration before enter %.6f %.6f %.6f", layer->Duration, controller->Duration, layer->Duration - controller->Duration);
	layer->Duration = 0;
	enterState(sob);
}

void ISimulationState::exit(ISimulationObj *sob, double dt) {
	ISimulationObj::stateLayer* layer = sob->getStateLayer(getLayerId());
	if(layer == NULL)
		return;
//	nlinfo("Duration before exit %.6f %.6f %.6f", layer->Duration, controller->Duration, layer->Duration - controller->Duration);
	updateAll(sob, dt);
	exitState(sob);
}

void ISimulationState::handle(ISimulationObj *sob) {
	ISimulationObj::stateLayer* layer = sob->getStateLayer(getLayerId());
	if(layer == NULL)
		return;
	float elapsed = (float)(::getSimulation()->deltaTime() - layer->CorrectionDt);	// get the time that's elapsed since the last frame.
	handle(sob, elapsed);
	layer->CorrectionDt = 0;
};

void ISimulationState::update(ISimulationObj *sob, double dt) {
//	nlinfo("Updating state with correction %.6f", dt);
	handle(sob, (float)dt);
	ISimulationObj::stateLayer* layer = sob->getStateLayer(getLayerId());
	if(layer)
		layer->CorrectionDt = dt;
};

void ISimulationState::block() {
	m_Blocked=true;
}

void ISimulationState::unblock() { 
	m_Blocked=false;
}

void ISimulationState::incrementDuration(ISimulationObj* sob, double dt) {
	ISimulationObj::stateLayer* layer = sob->getStateLayer(getLayerId());
	if(layer)
		layer->Duration += dt;
};

void ISimulationState::enterState(ISimulationObj *sob) {
	; // do nothing by default
};

void ISimulationState::exitState(ISimulationObj *sob) {
	; // do nothing by default
};

}; // END OF NAMESPACE WWCOMMON
