/**
 * \file COrientState.h
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

#ifndef __CORIENTSTATE_H__
#define __CORIENTSTATE_H__

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
#include "CPerformer.h"
#include "CStateManager.h"

//
// Namespaces
//

namespace WWCOMMON {

class COrientState : public ISimulationState {
public:
	COrientState(int direction);
	virtual uint32 getStateId() {
		return CStateFactory::ST_NONE;	// this is not a valid class for state management.
	}

	virtual uint8 getLayerId() {
		return CStateFactory::ST_ORIENTATION;
	}

protected:
	virtual void enterState(ISimulationObj *sob);
	virtual void handle(ISimulationObj *sob, float elapsed);
	virtual void updateAll(ISimulationObj *sob, double dt);

	int m_direction;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CORIENTSTATE_H__
