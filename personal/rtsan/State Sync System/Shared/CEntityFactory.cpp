/**
 * \file CEntityFactory.cpp
 * \date June 2004
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
// System Includes
//
#include <map>

//
// NeL Includes
//
#include <nel/misc/debug.h>

//
// Werewolf Includes
//
#include "CEntityFactory.h"

#include "CEntity.h"

//
// Namespaces
//

//
// Class References
//

//
// Class
//

bool CEntityFactory::registerEntity(std::string typeId, CreateEntityCallback createFn) {
	return m_Callbacks.insert(CallbackMap::value_type(typeId, createFn)).second;
}

bool CEntityFactory::unregisterEntity(std::string typeId) {
	return m_Callbacks.erase(typeId)==1;
}

CEntity *CEntityFactory::create(std::string typeId) {
	CallbackMap::iterator i = m_Callbacks.find(typeId);
	if (i == m_Callbacks.end()) // not found
		nlwarning("Entity type not found: %s", typeId.c_str());

	// Invoke the creation function
	return (i->second)();
}