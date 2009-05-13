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
#include "CClientSimulation.h"

//
// Namespaces
//

namespace WWCLIENT {

//
// Callbacks.
//
/**
 * \brief A login verification message was received from the server.
 *
 * S04: receive the "VLP" message from LS
 */
void cbVerifyLoginPassword(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	CNetworkTask::instance().cbVLP(msgin);
}

/**
 * \brief Catch a response from a shard about choosing it.
 *
 * S11: receive "SCS" message from LS
 */
void cbShardChooseShard(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	CNetworkTask::instance().cbSCS(msgin);
}

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
		struct TCharacterData character;

		// serialize the character
		msgin.serial(character);

		nlinfo("Receiving character: %s %d", character.Name.c_str(), character.CharacterID);
		CPreGameTask::instance().insertCharacter(character);
	}
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
NLNET::TCallbackItem LoginCallbackArray[] = {
	{ "VLP", cbVerifyLoginPassword },
	{ "SCS", cbShardChooseShard },
};

NLNET::TCallbackItem ClientCallbackArray[] = {
	{ "CHAT", cbChat },
	{ "IDENTIFICATION", cbIdentification },
	{ "LGCL", cbLGCharList },
	{ "EVT",		cbSimEventServer },
	{ "PINGACK", cbPingAck },
};

//
// Class
//
void CNetworkTask::init() {
	m_VLP=false;
	m_ConnectShard=false;
	m_ShardConnectReason="";
	m_VLPReason="";

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

void CNetworkTask::cbVLP(NLNET::CMessage msgin) {
	// get the reason/error from the message.
	msgin.serial(m_VLPReason);

	// if it returned with no reason, then you authenticated okay.
	if(m_VLPReason.empty()) {
		uint32 nbshard;
		msgin.serial(nbshard);

		// clear the list of shards here.
		m_ShardList.clear();
		m_VLPReason = "";

		// get the shard list
		for (uint i = 0; i < nbshard; i++) {
			CShard shard;
			msgin.serial(shard.ShardName, shard.ShardNbPlayers, shard.ShardId);
			m_ShardList.push_back(shard);
		}		
	}

	if(m_ShardList.size()==0) {
		nlinfo("No active shards found.");
		m_VLPReason="No active shards are available.";
	}

	m_VLP = true;
	m_NetworkState=eVLP;
}

void CNetworkTask::cbSCS(NLNET::CMessage msgin) {
	std::string scsAddr, scsCookie;

	msgin.serial(m_ShardConnectReason);
	
	// no error during choosing shard.
	if (m_ShardConnectReason.empty()) {
		msgin.serial(scsCookie);
		msgin.serial(scsAddr);
		m_ShardIP=scsAddr;
		m_LoginCookie.setFromString(scsCookie);
		m_ConnectShard = true;
		m_NetworkState=eSCS;
		nlinfo("Received SCS, login succeeded... cookie: %s addr: %s",scsAddr.c_str(),scsCookie.c_str());
	} else {
		nlinfo("Connection to chosen shard failed: %s", m_ShardConnectReason.c_str());
		m_ConnectShard = false;
	}
}

std::string CNetworkTask::connectToLoginServer(std::string user, std::string pass) {
	std::string addr="localhost:49998"; // this should be from the config file...
	std::string sysinfo="werewolf"; // this is the client version/type.
	std::string cpass=NLMISC::getMD5((const uint8 *)pass.c_str(), pass.size()).toString();

	if(CConfigTask::instance().configFile().exists("LoginServer"))
		addr = CConfigTask::instance().configFile().getVar("LoginServer").asString();

	// We must have a login server to connect:
	nlassert(!addr.empty());

	// add default port if not set by the config file
	if(addr.find(":") == std::string::npos)
		addr += ":49998";

	// create the socket we're going to use.
	try {
		m_Client=new NLNET::CCallbackClient();
		m_Client->addCallbackArray(LoginCallbackArray, sizeof(LoginCallbackArray)/sizeof(LoginCallbackArray[0]));
		m_Client->connect(NLNET::CInetAddress(addr));
	} catch(NLNET::ESocket &e) {
		delete m_Client;
		m_Client=0;
		std::string reason="Connection to Login Server failed: ";
		reason=+e.what();
		return reason;
	}
	
	// send the login attempt:
	NLNET::CMessage msgout("VLP");
	msgout.serial(const_cast<std::string&>(user));
	msgout.serial(const_cast<std::string&>(cpass));
	msgout.serial(const_cast<std::string&>(sysinfo));
	std::string OS, Proc, Mem, Gfx;
	OS = NLMISC::CSystemInfo::getOS().c_str();
	Proc = NLMISC::CSystemInfo::getProc().c_str();
	Mem = NLMISC::toString(NLMISC::CSystemInfo::availablePhysicalMemory()) + " " + NLMISC::toString(NLMISC::CSystemInfo::totalPhysicalMemory());
	msgout.serial (OS);
	msgout.serial (Proc);
	msgout.serial (Mem);
	Gfx="Commandline, so none.";
	Gfx = C3DTask::instance().driver().getDriverInformation();
	Gfx += " / ";
	Gfx += C3DTask::instance().driver().getVideocardInformation();
	msgout.serial (Gfx);
	m_Client->send(msgout);


	// wait for the "VLP" message to come back to us from the LS.
	while(!m_VLP) {
		// make sure we're still connected.
		if(!m_Client->connected())
			break;

		NLMISC::nlSleep(10);
		m_Client->update();
	}

	// if VLP still isn't true, we lost connectivity.
	if(!m_VLP) {
		m_Client->disconnect();
		delete m_Client;
		m_Client=0;
		return "Connection to Login Server lost!";
	}

	// if there was an error during login.
	if(!m_VLPReason.empty()) {
		std::string reason="";
		m_Client->disconnect();
		delete m_Client;
		m_Client=0;
		reason="Login failed.";
		nlerror("Login failed: %s",m_VLPReason.c_str());
		return reason;
	}

	// we return now to intro task, who will display all of the shards available.
	return "";
}

std::string CNetworkTask::connectToShard(uint shardid) {
	std::string reason="";

	// tell the LS what shard we want to connect to.
	NLNET::CMessage msgout("CS");
	msgout.serial(shardid);
	m_Client->send(msgout);

	while(!m_ConnectShard) {
		if(!m_Client->connected()) {
			reason="Lost connection to Login Server.";
			break;
		}

		m_Client->update();
		NLMISC::nlSleep(10);
	}
	return reason;
}

std::string CNetworkTask::connect() {
	// connect to the frontend.
	release(); // drop the callback client.

	try {
		// recreate the callback client for use with the FES
		m_Client=new NLNET::CCallbackClient();
		m_Client->addCallbackArray(ClientCallbackArray, sizeof(ClientCallbackArray)/sizeof(ClientCallbackArray[0]));
		m_Client->connect(m_ShardIP);

		// send the service validation message with our cookie.
		NLNET::CMessage msgout("SV");
		msgout.serial(m_LoginCookie);
		m_Client->send(msgout);

		// continue to poll until an "IDENTIFICATION" message is received.
		while(m_Client->connected() && (m_NetworkState != eOnline) ) {
			m_Client->update();
			NLMISC::nlSleep(10);
		}
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
	NLNET::CMessage msgout("CHAT");
	msgout.serial(chatLine);
	m_Client->send(msgout);
}

void CNetworkTask::send(NLNET::CMessage msgout) {
	nlinfo("Sending message to server.");

	m_Client->send(msgout);
}

}; // END NAMESPACE WWCLIENT

