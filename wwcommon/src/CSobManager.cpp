/**
 * \file CSobManager.cpp
 * \date April 2005
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
#include <nel/misc/debug.h>

//
// Werewolf Includes
//	
#include "CSobManager.h"
#include "ISimulationObj.h"

//
// Namespaces
//

namespace WWCOMMON {

/// Find a Simulation Object in-simulation by ID.
ISimulationObj *CSobManager::find(uint32 id) const {
	TSobMap::const_iterator i = m_SobMap.find(id);
	if(i == m_SobMap.end())
		return NULL;

	return (i->second);
}

/// Add a simulation object to the simulation system.
bool CSobManager::addSob(ISimulationObj *sob) {
	// store the ID for use later.
	m_LastSobId = sob->getSobId();

	nlinfo("Adding Sob ID %d to map.",m_LastSobId);
	
	// add the sob to the map
	m_SobMap.insert(TSobMap::value_type(m_LastSobId, sob)).second;
	
	// notify listeners
	notify();

	// always return true, might need a better check later.
	return true;
}

/// Remove a simulation object from the simulation system (uses the Sob ID)
bool CSobManager::removeSob(ISimulationObj *sob) {
	return m_SobMap.erase(sob->getSobId()) == 1;
}

/// Store the Sob System.
bool CSobManager::store() {
	return false; /* TODO */
}

/// Load the Sob System.
bool CSobManager::restore() {
	return false; /* TODO */
}

}; // END OF NAMESPACE WWCOMMON