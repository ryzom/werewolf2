/**
 * \file CSobFactory.cpp
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
#include <string>

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
#include "wwcommon/CSobFactory.h"
#include "wwcommon/ISimulationObj.h"

//
// Namespaces
//

namespace WWCOMMON {

bool CSobFactory::Register(std::string name, CreateSob create) {
	m_SobMap.insert( TSobMap::value_type(name, create));
		return true;
	//else
	//	return false;
}

ISimulationObj *CSobFactory::createSob(std::string name) {
	ISimulationObj *sob = makeSob(name);
	sob->init();
	return sob;
}

ISimulationObj *CSobFactory::createSob(std::string name, uint32 id) {
	ISimulationObj *sob = makeSob(name);
	sob->setSobId(id);
	sob->init();
	return sob;
}

ISimulationObj *CSobFactory::makeSob(std::string name) {
	ISimulationObj *sob;

	// Find the controller by name, make sure we can find it.
	TSobMap::const_iterator i = m_SobMap.find(name);
	if( i == m_SobMap.end()) {
		nlwarning("Simulation Object creation failed: %s not found!", name.c_str());
		return NULL;
	}

	// create a controller...
	sob=(i->second)();

	// return the controller to the requestor.
	return sob;
}

}; // END OF NAMESPACE WWCOMMON
