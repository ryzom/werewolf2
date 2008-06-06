/**
 * \file CEntityMap.h
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

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include "CEntityMap.h"

//
// Namespaces
//

bool CEntityMap::checkEntityMap(uint32 entid) {
	TEntityMap::const_iterator i = m_EntityMap.find(entid);
	if(i==m_EntityMap.end()) {
		nlwarning("Unable to find entity ID %d in the active list.",entid);
		return false;
	}
	return true;
}

bool CEntityMap::removeEntityMap(uint32 entid) {
	return m_EntityMap.erase(entid) == 1;
}

bool CEntityMap::addEntityMap(TEntityData *ent, uint32 entid) {
	nlinfo("Adding entity ID %d to map.",entid);
	return m_EntityMap.insert(TEntityMap::value_type(entid, ent)).second;
}

TEntityData *CEntityMap::findEntityMap(uint32 entid) {
	TEntityMap::const_iterator i = m_EntityMap.find(entid);
	if(checkEntityMap(entid)) {
		nlwarning("Unable to find entity ID %d in the active list.",entid);
		return NULL;
	}
	return (i->second);
}

uint32 CEntityMap::nbActiveEntity() {
	return m_EntityMap.size();
}
