/**
 * \file CMoveFwdState.cpp
 * \date May 2005
 * \author Matt Raykowski
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
#include "CMoveFwdState.h"
#include "CMovementState.h"
#include "ISimulationState.h"
#include "CStateManager.h"
#include "IBaseSimulation.h"

//
// Namespaces
//

namespace WWCOMMON {

OF_REGISTER(CStateFactory, ISimulationState, CMoveFwdState, uint32, CStateFactory::ST_MV_FORWARD);

CMoveFwdState::CMoveFwdState() : CMovementState(1) {
//	nlinfo("Constructing a forward moving state.");

	addTransitionable(CStateFactory::ST_MV_IDLE);
	addTransitionable(CStateFactory::ST_MV_BACKWARD);

	// allow transistion to itself for DR updates
	// TODO henri:matt any better ideas?
	addTransitionable(CStateFactory::ST_MV_FORWARD);
}

ISimulationState *CMoveFwdState::Create() {
	return new CMoveFwdState();
}

}; // END OF NAMESPACE WWCOMMON

