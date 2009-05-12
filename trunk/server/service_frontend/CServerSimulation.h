/**
 * \file CServerSimulation.h
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

#ifndef __CSERVERSIMULATION_H__
#define __CSERVERSIMULATION_H__

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
#include "wwcommon/ISimulationObj.h"
#include "wwcommon/CPerformer.h"
#include "wwcommon/CSobManager.h"
#include "wwcommon/IBaseSimulation.h"

//
// Namespaces
//

WWCOMMON::IBaseSimulation *getSimulation();

class CServerSimulation : public WWCOMMON::IBaseSimulation {
public:
	/// Processes a validated user login.
	bool userLogin(uint32 uid, uint32 objid);

	/// Processes a disconnecting user.
	bool userLogout(uint32 sobid);

	/// Attaches a user (uid) to a sob.
	virtual bool attachUser(uint32 uid, uint32 sobid);

	/// Detaches a user (uid) from a sob.
	virtual void detachUser(uint32 uid, uint32 sobid);

	/// Initializes the simulation.
	void init();

	/// Process tasks and updates.
	virtual void update();

	/// This gets a new sob object and initializes it with basic properties.
	WWCOMMON::ISimulationObj *initSob(std::string name);
};

#endif // __CSERVERSIMULATION_H__
