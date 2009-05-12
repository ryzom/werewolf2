/**
 * \file COrientTurnLeftState.cpp
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
#include "wwcommon/COrientTurnLeftState.h"
#include "wwcommon/COrientState.h"
#include "wwcommon/ISimulationState.h"
#include "wwcommon/CStateManager.h"
#include "wwcommon/IBaseSimulation.h"

//
// Namespaces
//

namespace WWCOMMON {

OF_REGISTER(CStateFactory, ISimulationState, COrientTurnLeftState, uint32, CStateFactory::ST_OR_TURNLEFT);

COrientTurnLeftState::COrientTurnLeftState() : COrientState(1) {
//	nlinfo("Constructing a orient turn left state.");

	addTransitionable(CStateFactory::ST_OR_TURNRIGHT);
	addTransitionable(CStateFactory::ST_OR_IDLE);

	// allow transistion to itself for DR updates
	// TODO henri:matt any better ideas?
	addTransitionable(CStateFactory::ST_OR_TURNLEFT);
}

ISimulationState *COrientTurnLeftState::Create() {
	return new COrientTurnLeftState();
}

}; // END OF NAMESPACE WWCOMMON

