/**
 * \file CClient.cpp
 * \date February 2005
 * \author Henri Kuuste
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
#ifdef NL_OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

#include <string>
#include <cstdio>

//
// NeL Includes
//
//#include <nel/net/login_cookie.h> 

//
// Werewolf Includes
//
#include "../Shared/CEntityFactory.h"
#include "../Shared/CEntityMap.h"
#include "../Shared/IController.h"
#include "../Shared/CControllerFactory.h"
#include "CEntityTask.h"
#include "CClient.h"

//
// Namespaces
//

//
// Global Variables
//

void CEntityTask::init() {
	m_Self=NULL;
}

void CEntityTask::update() {
	;
}

void CEntityTask::recvMessage(CMsgEntity msgent) {
	// spawnings will always be one-offs.
	if(msgent.Type == CMsgEntity::ENT_MSG_SPAWN) {
		nlinfo("Received a spawn request!");
		std::string entType="default";
		//msgin.serial(entType); //get the type of object this is. maybe uint32 in the future for object id?

		if(msgent.TargetID == EID_NONE) {
			nlwarning("Unable to retrieve entity ID.");
			return;
		}

		// double check that the entity ID isn't in use.
		if(CEntityMap::instance().checkEntityMap(msgent.TargetID)) {
			nlwarning("Attempt to reuse an active entity ID.");
			return;
		}
		
		// create an entity object.
		CEntity *entity=CEntityFactory::instance().create(entType);
		if(entity==NULL) {
			nlwarning("Something went wrong during entity creation.");
			return;
		}
		// set the new entity to the target id.
		entity->setId(msgent.TargetID);

		// create the default actor controller
		IController *ctrlr=CControllerFactory::instance().createController("actorController",entity);

		// create the entity data class
		TEntityData *ent=new TEntityData(entity,msgent.OwnerID);
		ent->controllers.push_back(ctrlr);

		// add it to the map for others to see.
		if(CEntityMap::instance().addEntityMap(ent,msgent.TargetID) == false) {
			nlwarning("Failed to register entity into active list.");
			return;
		}

		// if the self is null and the source is my uid then it's my entity.
		if( (m_Self == NULL) && (msgent.OwnerID == CClient::instance().getUserID()) ) {
			nlinfo("Found my entity. Saving reference.");
			m_Self=ent;
		} else {
			nlinfo("Registering outside entity into list. Owner: %d My UID: %d", msgent.OwnerID, CClient::instance().getUserID());
		}
	} else { // standard messaging.
		nlinfo("Received non-spawning entity message. Cycle through controllers.");
	}
}



