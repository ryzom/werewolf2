/**
 * \file CEntityManager.cpp
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

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/net/buf_net_base.h> 

//
// Werewolf Includes
//	
#include "../Shared/IController.h"
#include "../Shared/ISingleton.h"
#include "../Shared/CEntityFactory.h"
#include "../Shared/CEntityMap.h"
#include "../Shared/CControllerFactory.h"
#include "../Shared/CMsgEntity.h"
#include "CEntityManager.h"
#include "CCharacterManager.h"
#include "CMainService.h"

//
// Namespaces
//

void CEntityManager::init() {
	m_CurrentEID=1;
}

void CEntityManager::update() {
	;
}

uint32 CEntityManager::spawnEntity(uint32 objectID, uint32 source) {
	IController *ctrlr;
	CEntity *entity;
	TEntityData *ent;
	uint64 entityID;

	// create a default entity (this example knows no other type.)
	entity=CEntityFactory::instance().create("default");
	if(entity==NULL) {
		nlwarning("Something went wrong during entity creation.");
		return EID_NONE;
	}

	// get an available entity id.
	entityID=getEID();
	if(entityID == EID_NONE) {
		nlwarning("Unable to retrieve entity ID.");
		return EID_NONE;
	}

	// double check that the entity ID isn't in use.
	if(CEntityMap::instance().checkEntityMap(entityID)) {
		nlwarning("Attempt to reuse an active entity ID.");
		return EID_NONE;
	}

	entity->setId(entityID);
	ctrlr=CControllerFactory::instance().createController("actorController",entity);

	ent=new TEntityData(entity,source);
	ent->controllers.push_back(ctrlr);

	if(CEntityMap::instance().addEntityMap(ent,entityID) == false) {
		nlwarning("Failed to register entity into active list.");
		return EID_NONE;
	}

	// finally if we've gone through all of the creation fine, flag the entity as online
	CCharacterManager::instance().setOnline(objectID);

	broadcastSpawn(ent);

	return entityID;
}

bool CEntityManager::unSpawnEntity(uint32 entityID) {
	IController *ctrlr;
	CEntity *entity;
	TEntityData *ent;

	if(!CEntityMap::instance().checkEntityMap(entityID)) {
		nlwarning("Attempt to unspawn to an inactive entity.");
		return false;
	}

	//TEntityData *ent=findEntityMap(entityID);
	CEntityMap::instance().removeEntityMap(entityID);
	m_AvailableEID.push_back(entityID);
	return true;
}

void CEntityManager::recvMessage(CMsgEntity msgent) {
	// get the service. we may need it for player-level info or sending a message.
	CMainService *service=(CMainService *)NLNET::IService::getInstance();

	// spawnings will always be one-offs.
	if(msgent.Type == CMsgEntity::ENT_MSG_SPAWN) {
		// have the manager spawn an instance. the callback verifies ownerid's authenticity.
		uint32 entid=spawnEntity(msgent.ObjectID,msgent.OwnerID);

		// if we got and EID_NONE, something went wrong in spawning.
		if(entid == CEntityManager::EID_NONE) {
			nldebug("Failed to spawn new entity on player %d using entity id %d",msgent.OwnerID,entid);
			return;
		}

		// register the entity ID on the player object.
		service->spawnOnPlayer(msgent.OwnerID, entid);
	} else {
		nlinfo("Sending message type %d to controllers for entity ID: %d", msgent.Type, msgent.TargetID);
		// all others go through controller system
		TEntityData *ent=CEntityMap::instance().findEntityMap(msgent.TargetID);
		TEntityData::TEntityControllers::iterator itr=ent->controllers.begin();
		while( itr != ent->controllers.end() ) {
			(*itr)->recv(msgent);
		}
	}
}

void CEntityManager::broadcastSpawn(TEntityData *ent) {
	// collect up relevant information.
	CMsgEntity msgent;
	msgent.Type=CMsgEntity::ENT_MSG_SPAWN;
	msgent.TargetID=ent->entity->id();
	msgent.OwnerID=ent->owner;

	// create the outgoing spawn message.
	NLNET::CMessage msgout("ENT");
	msgout.serial(msgent);

	CMainService *service=(CMainService *)NLNET::IService::getInstance();
	service->sendMessage(msgout,NLNET::InvalidSockId); // invalid sock id forces a broadcast.
}

uint64 CEntityManager::getEID() {
	uint64 eid=0;
	if(m_AvailableEID.size() == 0)
		eid=m_CurrentEID++;
	else {
		eid=m_AvailableEID.front();
		m_AvailableEID.pop_front();
	}
	return eid;
}
