/**
 * \file CSobManager.h
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

#ifndef __CSOBMANAGER_H__
#define __CSOBMANAGER_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include "ISimulationObj.h"
#include "IObserver.h"

//
// Namespaces
//

namespace WWCOMMON {

class CSobManager : public ISubject<CSobManager> {
public:

	/// Find a Simulation Object in-simulation by ID.
	ISimulationObj *find(uint32 id) const;

	/// Add a simulation object to the simulation system.
	bool addSob(ISimulationObj *sob);
	/// Remove a simulation object from the simulation system (uses the Sob ID)
	bool removeSob(ISimulationObj *sob);

	// persistance stuff
	/**
	 * Store the Sob System.
	 *
	 * @todo This method has not yet been implemented.
	 */
	bool store();

	/**
	 * Load the Sob System.
	 *
	 * @todo This method has not yet been implemented.
	 */
	bool restore();

	/// Returns the ID of the last sob added.
	uint32 getLastSob() { return m_LastSobId; };

private:
	friend class IBaseSimulation;

	CSobManager() { m_LastSobId = 0; }
	virtual ~CSobManager() {}
	typedef std::map<uint32, ISimulationObj *> TSobMap;
	TSobMap m_SobMap;

	/// Holds the ID of the last sob added.
	uint32 m_LastSobId;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CSOBMANAGER_H__
