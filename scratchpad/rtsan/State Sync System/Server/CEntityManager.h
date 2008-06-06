/**
 * \file CEntityManager.h
 * \date March 2005
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

#ifndef __CENTITYMANAGER_H__
#define __CENTITYMANAGER_H__

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
#include "../Shared/IController.h"
#include "../Shared/ISingleton.h"
#include "../Shared/CEntityMap.h"
#include "../Shared/CMsgEntity.h"

//
// Namespaces
//

class CEntityManager : public ISingleton<CEntityManager> {
public:
	static const int EID_NONE = 0;

	/// Initialize the entity manager.
	void init();
	/// Process entity updates and update the entity manager.
	void update();

	/// Spawn a new entity.
	uint32 spawnEntity(uint32 objectID, uint32 source);
	bool unSpawnEntity(uint32 entityID);

	uint64 getEID();

	void broadcastSpawn(TEntityData *ent);

	void recvMessage(CMsgEntity msgent);
protected:
	/// The currently available Entity ID.
	uint64	m_CurrentEID;
	std::deque<uint64>	m_AvailableEID;
};

#endif // __CENTITYMANAGER_H__