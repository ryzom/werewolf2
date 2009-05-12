/**
 * \file CControllerFactory.h
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

#ifndef __CCONTROLLERFACTORY_H__
#define __CCONTROLLERFACTORY_H__

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
#include "ISingleton.h"
#include "IController.h"
#include "ISimulationObj.h"

//
// Namespaces
//

namespace WWCOMMON {

class CControllerFactory : public ISingleton<CControllerFactory> {
public:
	/// Typedef for the creation method defined on controllers.
	typedef IController* (*CreateController)(ISimulationObj *sob);

	/**
	 * \brief Called by controller implementations for auto-registration.
	 *
	 * \param create CreateController point to creation method.
	 * \param name The name of this controller type for requests.
	 *
	 * \return true if registration was successful.
	 */
	bool registerController(CreateController create, std::string name);

	/**
	 * \brief Called to request the creation of a controller.
	 *
	 * \param name The name of the controller type.
	 * \param sob The simulation object this controller will be attached to.
	 *
	 * \return IController pointer to the controller created.
	 */
	IController *createController(std::string name, ISimulationObj *sob);

private:
	/// Defines the type for the map of controllers.
	typedef std::map<std::string, CreateController> TControllerMap;
	/// A map of controllers registered for the factory.
	TControllerMap m_ControllerMap;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CCONTROLLERFACTORY_H__

