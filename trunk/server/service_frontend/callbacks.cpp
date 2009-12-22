/**
 * \file callbacks.cpp
 * \date December 2004
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
#include "stdpch.h"
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/service.h>
#include <nel/misc/displayer.h>
#include <nel/net/login_server.h>
#include <nel/net/login_cookie.h>

//
// Werewolf Includes
//
#include "CFrontendService.h"
#include "CCharacterManager.h"
#include "CUserManager.h"

#include "CServerSimulation.h"

#include <wwcommon/CSobFactory.h>
#include <wwcommon/CSobManager.h>
#include <wwcommon/ISimulationObj.h>
#include <wwcommon/CPerformer.h>
#include <wwcommon/CSobStateRequestEvent.h>
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CGameUnspawnRequestEvent.h>

//
// Callbacks
//

//
// Variables
//

//
// Class
//

/**
 * \brief Callback for client connections.
 *
 * This is called when a new client connects to the frontend service. The cookie
 * contains the user ID for the client connection. This callback retrieves that and
 * asks the service to create a player object. So that know what player is connected
 * we set the connection's appId to the player. Sticking to the login system process
 * we then send the client an "IDENTIFICATION" message asking it to identify itself
 * to us.
 *
 * \param from The client socket that connected.
 * \param cookie The cookie representing that client's credentials.
 */
void onConnectionClient(NLNET::TSockId from, const NLNET::CLoginCookie &cookie) {
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	uint32 id = cookie.getUserId();

	nlinfo("The client with unique Id %u is connected", id);

	// Add new client to the list of player managed by this FrontEnd
	fs->addPlayer(id, from);
	from->setAppId((uint64)id);
	CPlayer *plr=fs->getPlayer(id);
	plr->User = CUserManager::instance().getUser(id);
	
	// Output: send the IDENTIFICATION number to the new connected client
	NLNET::CMessage msgout("IDENTIFICATION");
	msgout.serial(id);
	fs->sendMessage(msgout, from);

	// resume connectability, let client get character list now
	from->AuthorizedCallback="LGCL";

	nldebug( "SB: Sent IDENTIFICATION message to the new client");
}

/**
 * \brief Callback for client disconnections.
 *
 * This is called when a client disconnects from the frontend service. The callback
 * gets the player information from the socket's appId and has the CLoginServer disconnect
 * the client (invalidating the cookie) and removes the CPlayer from the frontend service.
 *
 * \note In the future it will probably also trigger destruction of the player's entity.
 *
 * \param from The client socket that is disconnecting.
 * \param arg No idea. So far always NULL.
 */
void onDisconnectClient(NLNET::TSockId from, void *arg ) {
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	//CServerSimulation *server=(CServerSimulation *)getSimulation();

	uint32 id=(uint32)from->appId();
	CPlayer *plr=fs->getPlayer(id);

	nlinfo( "A client with uniq Id %u has disconnected", id );

	// tell the login system that this client is disconnected
	NLNET::CLoginServer::clientDisconnected(id);

	// unspawn the sob.
	WWCOMMON::CGameUnspawnRequestEvent *evt = new WWCOMMON::CGameUnspawnRequestEvent();
	evt->SobID = plr->SobID;
	WWCOMMON::CGameEventServer::instance().postEvent(evt);

	// Get the connected player's ID.
	CCharacterManager::instance().setOffline(plr->CharacterID);
	plr->Connected=false;

	// remove the player from the local player list
	fs->removePlayer(id);
}

/**
 * \brief Receive chat messages from the client.
 *
 * Receive chat messages from a client and send it to all clients.
 *
 * \note Currently this is limited to chatting with other players on the same frontend.
 *
 * \param msgin The network message containing the chat message. A string is the only variable.
 * \param from The client socket that initiated the chat message.
 * \param clientcb The serverr that received the meessage.
 */
void cbChatClient(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb) {
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	std::string message;

	// Input from the client is stored.
	msgin.serial (message);

	// Prepare the message to send to the CHAT service
	NLNET::CMessage msgout ("CHAT");
	msgout.serial (message);
	fs->sendMessage(msgout);	// sends to all connected clients.
	nldebug("SB: Received CHAT message \"%s\" from client \"%s\" and sent to all clients.", message.c_str(), clientcb.hostAddress(from).asString().c_str());
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
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	uint32 uid=(uint32)from->appId();

	nlinfo("Received request for a character list.");
	// Part 1: number of elements
	// Part 2+: Each entity.
	NLNET::CMessage msgout("LGCL");

	// have the character manager generate a list of characters for this uid.
	CCharacterManager::TCharacterList charMap=CCharacterManager::instance().getListForPlayer(uid);
	CCharacterManager::TCharacterList::iterator itr=charMap.begin();

	// log the number found and pack it into the message
	uint8 nbChars=charMap.size();
	nlinfo("Building character list of %d chars.",nbChars);
	msgout.serial(nbChars);
	
	// go through the list and pack up the users.
	while(itr != charMap.end()) {
		msgout.serial( (*itr) );
		itr++;
	}

	// resume connectability, authorize for a spawning
	fs->sendMessage(msgout,from);
}

/**
 * \brief Receive the CH_CR - or character creation - request from the client.
 *
 * Receive the CH_CR - or character creation - request from the client. This request
 * message has two data points: CharacterType and CharacterName, respectively. The
 * character type specifies the EMD that this character uses. The name specifies the name
 * of the character.
 */
void cbCHCR(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netBase) {
	// Retrieve the user ID.
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	uint32 uid=(uint32)from->appId();

	// Extract the EMD and character name.
	std::string emd;
	std::string charName;
	msgin.serial(emd);
	msgin.serial(charName);

	// Populate a new character 
	WWCOMMON::CCharacterData *charData = new WWCOMMON::CCharacterData();
	charData->EmdType = emd;
	charData->Name = charName;
	charData->UserID = uid;
	charData->Online = false;
	
	charData = CCharacterManager::instance().addCharacter(charData);

	std::string reason;
	NLNET::CMessage msgout("CH_CR_ACK");
	// Validate that this addition succeeded.
	if(!charData)
		reason = "Unable to add new character.";
	msgout.serial(reason);
	msgout.serial(*charData);

	// Send character creation message back to the client.
	fs->sendMessage(msgout,from);
}

void cbSimEventClient(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb) {
	WWCOMMON::IGameEvent *gameEvent;
	msgin.serialPolyPtr(gameEvent);
	gameEvent->setPlayerID((uint32)from->appId());

//	nlinfo("Got event: %s", gameEvent->getClassName().c_str());
	WWCOMMON::CGameEventServer::instance().postEvent(gameEvent);
}

/**
 * \brief Sends a ping acknowledgement to the client with the timestamp sent to it and the server's time.
 */
void cbPingRequestClient(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb) {
	double serverTime, clientTime;
	msgin.serial(clientTime);
	serverTime=getSimulation()->time();

	nlinfo("Transmitting PING Acknowledgement to client at: %lf", serverTime);
	NLNET::CMessage msgout("PINGACK");
	msgout.serial(clientTime);
	msgout.serial(serverTime);
	clientcb.send(msgout,from);
}

