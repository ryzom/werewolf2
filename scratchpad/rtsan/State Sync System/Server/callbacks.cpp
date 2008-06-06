/**
 * \file callbacks.cpp
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

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "CMainService.h"
#include "CEntityManager.h"
#include "CCharacterManager.h"
#include "../Shared/CMsgEntity.h"
#include "callbacks.h"

//
// Namespaces
//

/*****
 * Client Connection callbacks
 *****/

/**
 * \brief Accept a new connection and permit it to only verify login and pass.
 */
void cbClientConnection(NLNET::TSockId from, void *arg) {
	// make it so that this client must VLP first
	from->AuthorizedCallback="VLP";
}

/**
 * \brief Process the VLP login attempt.
 */
void cbVLP(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	CMainService *service=(CMainService *)NLNET::IService::getInstance();

	// get the user/pass and authroize.
	std::string user,pass;
	msgin.serial(user);
	msgin.serial(pass);

	// check authorization. -1 if failed.
	sint32 id=service->authorizePlayer(user,pass);
	if(id<0) {
		nlwarning("Authentication failed. Stalling connection.");
		return;
	}
	// add the player to the service.
	service->addPlayer(id,from);

	// log the connection
	const NLNET::CInetAddress &ia = netbase.hostAddress(from);
	nldebug("Client connection: %s %d", ia.asString().c_str(), id);

	// resume connectability, let client get character list now
	from->AuthorizedCallback="LGCL";

	// and send the identify notice.
	NLNET::CMessage msgout("IDENT");
	msgout.serial(id);
	service->sendMessage(msgout, from);
	from->setAppId((uint64)id);
}

/**
 * \brief Disconnects users and updates the database.
 */
void cbClientDisconnection(NLNET::TSockId from, void *arg) {
	CMainService *service=(CMainService *)NLNET::IService::getInstance();
	NLNET::CCallbackNetBase *cnb = service->getClientServer();

	uint32 id = (uint32)from->appId();

	const NLNET::CInetAddress &ia = cnb->hostAddress (from);
	nldebug("Client disconnection: %s %d", ia.asString().c_str(), id);
	
	service->removePlayer(id);
}

/**
 * \brief Used for troubleshooting connectivity, sends a "PING" back.
 */
void cbPing(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	nldebug("Ping from client %s", netbase.hostAddress(from).asString().c_str());
	NLNET::CMessage msgout("PING");
	netbase.send(msgout, from);
}

void cbStatus(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	nldebug("Got status from client %s", netbase.hostAddress(from).asString().c_str());
}

/**
 * \brief Entity Spawn Request.
 */
void cbSpawn(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	CMainService *service=(CMainService *)NLNET::IService::getInstance();
	uint32 id = (uint32)from->appId();

	// get the message
	CMsgEntity msgent;
	msgin.serial(msgent);

	// verify the type. no cheaters!
	if(msgent.Type != CMsgEntity::ENT_MSG_SPAWN) {
		nlwarning("Invalid message type received by spawn callback.");
		return;
	}

	// verify authenticity. Must not pretend to be someone else and must not try to inject an entity ID.
	// note: entity manager doesn't 
	if(msgent.OwnerID != id  && msgent.TargetID != CEntityManager::EID_NONE) {
		nlwarning("Player attempted to spawn using another player's ID! %d instead of %d", msgent.OwnerID, id);
		return;
	}

	CEntityManager::instance().recvMessage(msgent);

//	nldebug("Got spawn request for entity id %d from client %s",entid,netbase.hostAddress(from).asString().c_str());
}


/// Temporary holding structure for the character list.
struct TCharList {
	TCharList(std::string name, uint32 owner) : Name(name), ObjectID(owner) { }
	std::string Name;
	uint32 ObjectID;
};

/**
 * \brief Sends back a list of characters for a connected userid.
 */
void cbLGCharList(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	CMainService *service=(CMainService *)NLNET::IService::getInstance();
	uint32 uid=(uint32)from->appId();

	nlinfo("Received request for a character list.");
	// Part 1: number of elements
	// Part 2+: Each entity.
	NLNET::CMessage msgout("LGCL");

	std::vector<TCharList> msgList;
	CCharacterManager::TCharacterMap charMap=CCharacterManager::instance().sendCharacterMap();
	CCharacterManager::TCharacterMap::iterator itr=charMap.begin();
	while(itr != charMap.end()) {
		if( (*itr).userID == uid )
			msgList.push_back(TCharList( (*itr).name, (*itr).objectID ) );
		itr++;
	}

	uint8 nbChars=msgList.size();
	nlinfo("Building character list of %d chars.",nbChars);
	msgout.serial(nbChars);
	std::vector<TCharList>::iterator msgitr=msgList.begin();
	while(msgitr != msgList.end()) {
		msgout.serial( (*msgitr).Name );
		msgout.serial( (*msgitr).ObjectID );
		msgitr++;
	}

	// resume connectability, authorize for a spawning
	from->AuthorizedCallback="SPW";
	service->sendMessage(msgout,from);
}
