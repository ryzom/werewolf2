/**
 * \file CSobFactory.h
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

#ifndef __CSOBFACTORY_H__
#define __CSOBFACTORY_H__

//
// Standard Includes
//
#include <string>
#include <map>

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
#include "ISingleton.h"
#include "ISimulationObj.h"

//
// Namespaces
//

namespace WWCOMMON {

class CSobFactory : public ISingleton<CSobFactory> {
public:
	typedef ISimulationObj* (*CreateSob)();
	bool Register(std::string name, CreateSob create);
	
protected:
	/// Allow only the simulation classes to work with sobs.
	friend class IBaseSimulation;

	/// Create a sob object.
	ISimulationObj *createSob(std::string name);
	ISimulationObj *createSob(std::string name, uint32 id);

	/// Defines the type for the map of simulation objects.
	typedef std::map<std::string, CreateSob> TSobMap;
	/// A map of simulation objects registered for the factory.
	TSobMap m_SobMap;
	
private:
	ISimulationObj *makeSob(std::string name);

};

}; // END OF NAMESPACE WWCOMMON

#endif // __CSOBMANAGER_H__
