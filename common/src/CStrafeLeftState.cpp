/**
 * \file CStrafeLeftState.cpp
 * \date May 2005
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
#include "CStrafeLeftState.h"
#include "CStrafeState.h"
#include "ISimulationState.h"
#include "CStateManager.h"
#include "IBaseSimulation.h"

//
// Namespaces
//

namespace WWCOMMON {

OF_REGISTER(CStateFactory, ISimulationState, CStrafeLeftState, uint32, CStateFactory::ST_ST_LEFT);

CStrafeLeftState::CStrafeLeftState() : CStrafeState(-1) {
//	nlinfo("Constructing a strafe left state.");

	addTransitionable(CStateFactory::ST_ST_RIGHT);
	addTransitionable(CStateFactory::ST_ST_IDLE);

	// allow transistion to itself for DR updates
	// TODO henri:matt any better ideas?
	addTransitionable(CStateFactory::ST_ST_LEFT);
}

ISimulationState *CStrafeLeftState::Create() {
	return new CStrafeLeftState();
}

}; // END OF NAMESPACE WWCOMMON

