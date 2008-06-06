/**
 * \file CStateManager.h
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

#ifndef __CSTATEMANAGER_H__
#define __CSTATEMANAGER_H__

//
// Standard Includes
//
#include <map>

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
#include "ISingleton.h"

//
// Namespaces
//

namespace WWCOMMON {

// forward declaration to avoid looped includes.
class ISimulationState;

// TODO henri:henri possibly add a way to get the default state for a given layer
class CStateFactory : public ISingleton<CStateFactory> {
public:
	/// Defines the IDs for each state class.
	enum eTargetState {
		ST_NONE		= 0,
	
		/* Movement Layer */
		ST_MV_IDLE			= 10,	/*!< No forward/backward movement - idle and stopped. */
		ST_MV_FORWARD		= 11,	/*!< Moving forward. */
		ST_MV_BACKWARD		= 12,	/*!< Moving backward. */

		/* Strafe Layer */ // Bad solution - TODO: think of something better
		ST_ST_IDLE			= 13,   /*!< No sideways movement. */
		ST_ST_LEFT			= 14,	/*!< Strafing to facing-left. */
		ST_ST_RIGHT			= 15,	/*!< strafing to facing-right. */

		/* Orientation Layer */
		ST_OR_IDLE			= 20,	/*!< Character is not turning. */
		ST_OR_TURNLEFT		= 21,	/*!< Character is turning facing-left. */
		ST_OR_TURNRIGHT		= 22,	/*!< Character is turning facing-right. */

		/* Posture Layer */
		ST_PS_STANDING		= 30,	/*!< Character is standing. */
		ST_PS_SITTING		= 31,	/*!< Character is sitting. */
		ST_PS_WALKING		= 32,	/*!< Character is walking. */
		ST_PS_RUNNING		= 33,	/*!< Character is running. */

		/* Action Layer */
		ST_AC_IDLE			= 40,	/*!< Character is idle. */
		ST_AC_SPAWNING		= 41,	/*!< Character is spawning. This state blocks all for it's duration. */

		ST_STATE_END
	};


	enum eStateLayer {
		ST_MOVEMENT	= 0,
		ST_POSTURE	= 1,
		ST_ACTION	= 2,
		ST_STRAFE	= 3,
		ST_ORIENTATION	= 4,
		ST_LAYER_END
	};

	/// Type for the create method to instantiate a state.
	typedef ISimulationState* (*CreateState)();
	
	CStateFactory();
	~CStateFactory();

	/// Register a state type.
	bool Register(uint32 type, CreateState create);

	/// Get an object of the specified state.
	ISimulationState *getState(uint32 stateId);

	/// Get the default state for the given state layer.
	ISimulationState *getDefault(uint8 layerId);

protected:
	/// Defines the type for the map of simulation states.
	typedef std::map<uint32, ISimulationState *> TStateMap;
	/// Defines the type for the map of default states for layers.
	typedef uint32* TDefaultMap;

	/// A map of simulation states registered for the factory.
	TStateMap m_StateMap;
	TDefaultMap m_DefaultMap;
	
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CSTATEMANAGER_H__
