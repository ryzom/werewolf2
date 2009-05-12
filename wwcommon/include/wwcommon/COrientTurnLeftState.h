/**
 * \file COrientTurnLeftState.h
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

#ifndef __CORIENTTURNLEFTSTATE_H__
#define __CORIENTTURNLEFTSTATE_H__

//
// Standard Includes
//

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
#include "ISimulationState.h"
#include "COrientState.h"
#include "CStateManager.h"
#include "CFactoryRegistrar.h"

//
// Namespaces
//

namespace WWCOMMON {

class COrientTurnLeftState : public COrientState {
	OF_SETUP_REGISTRAR(CStateFactory, ISimulationState, COrientTurnLeftState, uint32);
public:
	COrientTurnLeftState();

	virtual uint32 getStateId() {
		return CStateFactory::ST_OR_TURNLEFT;
	}

	static ISimulationState *Create();
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CORIENTTURNLEFTSTATE_H__
