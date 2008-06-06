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

#ifndef __CENTITYMAP_H__
#define __CENTITYMAP_H__

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
#include "../Shared/ISingleton.h"
#include "../Shared/IController.h"
#include "../Shared/CEntity.h"

//
// Namespaces
//

/// Defines the data the manager stores regarding entities.
struct TEntityData {
	TEntityData(CEntity *ent, sint32 uid) : entity(ent), owner(uid) {}
	CEntity *entity;
	sint32 owner;
	typedef std::list<IController *> TEntityControllers;
	TEntityControllers controllers;
};

class CEntityMap : public ISingleton<CEntityMap> {
public:
	/**
	 * \brief Check is an entity is in the active list.
	 *
	 * \param entid The entity ID to search for.
	 * \return true if the entity is active, false if it is not.
	 */
	bool checkEntityMap(uint32 entid);
	/**
	 * \brief Removes an entity from the active list.
	 *
	 * \param entid The ID of the entity to drop.
	 */
	bool removeEntityMap(uint32 entid);
	/**
	 * \brief Adds a new entity into the active map.
	 *
	 * \param ent The TEntityData structure containing some information about the entity.
	 * \param entid The ID of the entity.
	 *
	 * \return bool true if the entity was successfully added.
	 */
	bool addEntityMap(TEntityData *ent, uint32 entid);
	/**
	 * \brief Find an entity in the active map by ID.
	 *
	 * \param entid The ID of the entity to find.
	 * \return TEntityData containing the entity data that corresponds to the entid
	 */
	TEntityData *findEntityMap(uint32 entid);

	/// Returns the number of entities in the map.
	uint32 nbActiveEntity();

private:
	typedef std::map<uint32, TEntityData *> TEntityMap;
	TEntityMap m_EntityMap;
};

#endif // __CENTITYMAP_H__
