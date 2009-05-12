/**
 * \file CControllerFactory.cpp
 * \date April 2005
 * \author Henri Kuuste
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

//
// Werewolf Includes
//	
#include "wwcommon/IController.h"
#include "wwcommon/ISimulationObj.h"
#include "wwcommon/CControllerFactory.h"

//
// Namespaces
//

namespace WWCOMMON {

bool CControllerFactory::registerController(CreateController create, std::string name) {
	return m_ControllerMap.insert( TControllerMap::value_type(name, create)).second;
}

IController *CControllerFactory::createController(std::string name, ISimulationObj *sob) {
	IController *ctrlr;

	// Find the controller by name, make sure we can find it.
	TControllerMap::const_iterator i = m_ControllerMap.find(name);
	if( i == m_ControllerMap.end()) {
		nlwarning("Controller creation failed: %s not found!", name.c_str());
		return NULL;
	}

	// create a controller...
	ctrlr=(i->second)(sob);

	// return the controller to the requestor.
	return ctrlr;
}

}; // END OF NAMESPACE WWCOMMON
