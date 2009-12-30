/**
 * \file CNetworkTask.cpp
 * \date January 2005
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
#include "CEGUI.h"

//
// NeL Includes
//
#include <nel/net/callback_client.h>
#include <nel/net/login_client.h>
#include <nel/misc/md5.h>
#include <nel/misc/system_info.h>

//
// Werewolf Includes
//
#include "tasks/CNetworkTask.h"
#include "tasks/C3DTask.h"
#include "tasks/CPreGameTask.h"
#include "tasks/CGameTask.h"
#include "tasks/CConfigTask.h"

#include "wwcommon/IGameEvent.h"
#include "wwcommon/CCharacterData.h"
#include "wwcommon/CGameChatEvent.h"
#include "CSimulationImpl.h"

//
// Namespaces
//

namespace WWCLIENT {

//
// Callbacks.
//
/**
 * \brief Catch chat messages from the server.
 */
void cbChat(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	std::string line;
	msgin.serial(line);
	nlinfo("Received chat message: %s", line.c_str());
	CGameTask::instance().recvChat(line);
}

void cbIdentification(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	uint32 id;
	msgin.serial(id);

	nlinfo("my online id is %u", id);
	CNetworkTask::instance().setSelfId(id);

	// send LGCL to get character list.
	NLNET::CMessage msgout("LGCL");
	CNetworkTask::instance().getClient()->send(msgout);
}

void cbLGCharList(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb) {
	uint8 nbChars;

	// get the number of characters sent.
	msgin.serial(nbChars);
	nlinfo("Received my character list of %d chars.", nbChars);
	for(uint i=0;i<nbChars;i++) {
		WWCOMMON::CCharacterData character;

		// serialize the character
		msgin.serial(character);

		nlinfo("Receiving character: %s %d", character.Name.c_str(), character.CharacterID);
		CPreGameTask::instance().insertCharacter(character);
	}
}

void cbCH_CR_ACK(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb) {
	nlinfo("receiving character creation acknowledgement.");
	std::string reason;
	WWCOMMON::CCharacterData charData;
	msgin.serial(reason);
	if(!reason.empty()) {
		nlwarning("Failed to create character on server!");
		return;
	}

	msgin.serial(charData);
	
	CPreGameTask::instance().insertCharacter(charData);

	CEGUI::WindowManager::getSingleton().getWindow("PreGameTask/SelectChar")->show();
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/PreGameTask/CreateChar")->hide();
}

void cbSimEventServer(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb) {
	WWCOMMON::IGameEvent *gameEvent;
	msgin.serialPolyPtr(gameEvent);
	WWCOMMON::CGameEventServer::instance().postEvent(gameEvent);
}

/**
 * \brief Receive a ping response.
 *
 * Ping responses has one piece:
 *   * Server time stamp.
 */
void cbPingAck(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb) {
	double serverTime, localTime;
	msgin.serial(localTime);
	msgin.serial(serverTime);

	getClientSimulation()->recvPing(serverTime, localTime);
}

//
// Callback Arrays
//
//NLNET::TCallbackItem LoginCallbackArray[] = {
//	{ "VLP", cbVerifyLoginPassword },
//	{ "SCS", cbShardChooseShard },
//};

NLNET::TCallbackItem ClientCallbackArray[] = {
	{ "CHAT", cbChat },
	{ "IDENTIFICATION", cbIdentification },
	{ "LGCL", cbLGCharList },
	{ "CH_CR_ACK", cbCH_CR_ACK },
	{ "EVT",		cbSimEventServer },
	{ "PINGACK", cbPingAck },
};

//
// Class
//
void CNetworkTask::init() {
	//m_VLP=false;
	m_ConnectShard=false;
	//m_ShardConnectReason="";
	//m_VLPReason="";

	// network state.
	m_NetworkState=eOffline;
}

void CNetworkTask::update() {
	if(m_NetworkState != eOnline)
		return;

	m_Client->update();
}

void CNetworkTask::render() {
	;
}

void CNetworkTask::release() {
	if(m_Client==0)
		return;

	if(m_Client->connected())
		m_Client->disconnect();

	delete m_Client;
	m_Client=0;
}

void CNetworkTask::stop() {
	;
}

NLNET::CCallbackClient *CNetworkTask::getClient() {
	return m_Client;
}

std::string CNetworkTask::connect() {
	// connect to the frontend.
	//release(); // drop the callback client.

	try {
		// recreate the callback client for use with the FES
		m_Client=new NLNET::CCallbackClient();
		m_Client->addCallbackArray(ClientCallbackArray, sizeof(ClientCallbackArray)/sizeof(ClientCallbackArray[0]));
		//m_Client->connect(m_ShardIP);

		nlinfo("Using Login Client to connect to shard: '%s'",getShardIp().asIPString().c_str());
		std::string res = NLNET::CLoginClient::connectToShard(m_LoginCookie, getShardIp().asString(), *m_Client);
		if(res.empty()) {
			nlinfo("Successfully connected to the FS!");
		} else {
			nlwarning("Unable to connect to FS: %s", res.c_str());
		}

		// continue to poll until an "IDENTIFICATION" message is received.
		while(m_Client->connected() && (m_NetworkState != eOnline) ) {
			m_Client->update();
			NLMISC::nlSleep(10);
		}
		nlinfo("Received identification. We are now online!!");
	} catch(NLNET::ESocket &e) {
		// something failed, delete the callback client and retry.
		release();
		nlwarning("Connected refused to LS: %s", e.what());
		return "Connection to Login Server lost.";
	}
	return "";
}

void CNetworkTask::setSelfId(uint32 id) {
	m_SelfNetworkId=id;
	m_NetworkState=eOnline;
}

// sending game messages
void CNetworkTask::sendChat(std::string chatLine) {
	nlinfo("Sending chat message to server: %s", chatLine.c_str());
	//NLNET::CMessage msgout("CHAT");
	//msgout.serial(chatLine);
	//m_Client->send(msgout);

	nlinfo("Creating cgamechatevent.");
	WWCOMMON::CGameChatEvent *evt = new WWCOMMON::CGameChatEvent();
	evt->SourceSobID = getClientSimulation()->getSelfSob()->getSobId();
	evt->TargetSobID = 0; // Public.
	evt->ChatChannel = 0; // Public.
	evt->ChatMessage = chatLine;
	
	nlinfo("sending game chat event to game event server.");
	WWCOMMON::CGameEventServer::instance().postEvent(evt);
}

void CNetworkTask::send(NLNET::CMessage msgout) {
	m_Client->send(msgout);
}

}; // END NAMESPACE WWCLIENT

