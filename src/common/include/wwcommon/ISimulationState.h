/**
 * \file ISimulationState.h
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

#ifndef __ISIMULATIONSTATE_H__
#define __ISIMULATIONSTATE_H__

//
// Standard Includes
//
#include <list>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>

//
// Werewolf Includes
//

//
// Namespaces
//

namespace WWCOMMON {

class ISimulationObj;

class ISimulationState {
public:
	/// Default ctor.
	ISimulationState();
	/// Default virtual dtor.
	virtual ~ISimulationState();

	/*
	 * \brief Provides the ID pertaining to the type of this state.
	 *
	 * \return uint32 type ID.
	 * \sa eStateType
	 */
	virtual uint32 getStateId();
	virtual uint8 getLayerId() = 0;

	void addTransitionable(uint32 state);
	/// Check if this state can transition to the specified.
	bool canTransition(uint32 state);
	void enter(ISimulationObj *sob);
	void exit(ISimulationObj *sob, double dt);
	void handle(ISimulationObj *sob);
	void update(ISimulationObj *sob, double dt);

	virtual void block();
	virtual void unblock();

protected:
	// make sure to update duration in here
	virtual void handle(ISimulationObj *sob, float elapsed)=0;
	// This method should update everything related to this state (itself included) with the corrective delta time
	virtual void updateAll(ISimulationObj *sob, double dt)=0;

	void incrementDuration(ISimulationObj *sob, double dt);
	virtual void enterState(ISimulationObj *sob);
	virtual void exitState(ISimulationObj *sob);

	std::list<uint32> m_Transitions;
	bool m_Blocked;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __ISIMULATIONSTATE_H__
