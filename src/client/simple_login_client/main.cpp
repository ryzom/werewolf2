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
#	include <windows.h>
#	undef min
#	undef max
	HINSTANCE ghInstance = 0;
#endif

#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/udp_sock.h>
#include <nel/net/login_cookie.h>
#include <nel/net/callback_client.h>
#include <nel/misc/system_info.h> 
#include <nel/net/login_client.h>

//
// Werewolf Includes
//	
#include "main.h"

//
// Namespaces
//
using namespace std;
using namespace NLMISC;

//
// global vars.
//
bool VerifyLoginPassword;
bool ShardChooseShard;
bool OnlineFES;
NLNET::CInetAddress ShardIP;
NLNET::CLoginCookie ShardCookie;
std::string VerifyLoginPasswordReason;
std::vector<CShard> ShardList;
uint gShard;

//
// Callbacks
//

/**
 * \brief A login verification message was received from the server.
 *
 * S04: receive the "VLP" message from LS
 */
void cbVerifyLoginPassword(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	msgin.serial(VerifyLoginPasswordReason);

	// if it returned with no reason, then you authenticated okay.
	if(VerifyLoginPasswordReason.empty()) {
		uint32 nbshard;
		msgin.serial(nbshard);

		// clear the list of shards here.
		ShardList.clear();
		VerifyLoginPasswordReason = "";

		// get the shard list
		for (uint i = 0; i < nbshard; i++) {
			CShard shard;
			msgin.serial(shard.ShardName, shard.ShardNbPlayers, shard.ShardId);
			ShardList.push_back(shard);
		}		
	}

	if(ShardList.size()==0) {
		nlinfo("No active shards found.");
	} else {
		for(uint i=0 ; i < ShardList.size() ; i++) {
			CShard shard=ShardList[i];
			gShard=shard.ShardId;
			nlinfo("Shard Found - Name: %s PlrCount: %d ID: %d",shard.ShardName.c_str(), shard.ShardNbPlayers, shard.ShardId);
		}
	}
	

	VerifyLoginPassword = true;
}

/**
 * \brief Catch a response from a shard about choosing it.
 *
 * S11: receive "SCS" message from LS
 */
void cbShardChooseShard(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	std::string scsReason, scsAddr, scsCookie;

	msgin.serial(scsReason);
	
	if (scsReason.empty()) {
		msgin.serial(scsCookie);
		msgin.serial(scsAddr);
		ShardIP=scsAddr;
		ShardCookie.setFromString(scsCookie);
		ShardChooseShard = true;
		nlinfo("Received SCS, login succeeded... cookie: %s addr: %s",scsAddr.c_str(),scsCookie.c_str());
	} else {
		nlinfo("Connection to chosen shard failed: %s", scsReason.c_str());
		ShardChooseShard = false;
	}
}


// FES calls.
void cbChat(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	std::string line;
	msgin.serial(line);
	nlinfo("Received chat message: %s", line.c_str());
}

void cbIdentification(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	uint32 id;
	msgin.serial(id);
	
	nlinfo("my online id is %u", id);

	OnlineFES=true;
	// send to the network my entity					
	//sendAddEntity (Self->Id, Self->Name, 1, Self->Position);
}
//
// Global Variables
//
NLNET::TCallbackItem CallbackArray[] = {
	{ "VLP", cbVerifyLoginPassword },
	{ "SCS", cbShardChooseShard },
};

NLNET::TCallbackItem ClientCallbackArray[] = {
//	{ "ADD_ENTITY", cbAddEntity },
//	{ "REMOVE_ENTITY", cbRemoveEntity },
//	{ "ENTITY_POS", cbEntityPos },
//	{ "HIT", cbHit },
	{ "CHAT", cbChat },
//	{ "SNOWBALL", cbSnowball },
	{ "IDENTIFICATION", cbIdentification },
};

int main(int argc, char **argv) {
	std::string login,password,sysinfo;
	login="mattr";
	password="banana";
	sysinfo="werewolf";
	gShard=0;
	ShardChooseShard = false;
	OnlineFES=false;

	// create a callback client and set it up.
	NLNET::CCallbackClient *cbClient;
	try {
		// create the client
		cbClient=new NLNET::CCallbackClient();
		cbClient->addCallbackArray(CallbackArray, sizeof(CallbackArray)/sizeof(CallbackArray[0]));

		// connect to the server.
		std::string addr = "localhost:49998";
		cbClient->connect(NLNET::CInetAddress(addr));
	} catch(NLNET::ESocket &e) {
		delete cbClient;
		cbClient = 0;

		nlwarning ("Connection refused to LS: %s", e.what ());
	}

	//
	// S02: create and send the "VLP" message
	//
	NLNET::CMessage msgout("VLP");
	msgout.serial (const_cast<std::string&>(login));
	msgout.serial (const_cast<std::string&>(password));
	msgout.serial(const_cast<std::string&>(sysinfo));

	std::string OS, Proc, Mem, Gfx;
	OS = NLMISC::CSystemInfo::getOS().c_str();
	Proc = NLMISC::CSystemInfo::getProc().c_str();
	Mem = NLMISC::toString(NLMISC::CSystemInfo::availablePhysicalMemory()) + " " + NLMISC::toString(NLMISC::CSystemInfo::totalPhysicalMemory());
	msgout.serial (OS);
	msgout.serial (Proc);
	msgout.serial (Mem);
	Gfx="Commandline, so none.";
	msgout.serial (Gfx);
	cbClient->send (msgout);

	VerifyLoginPassword = false;
	while(!VerifyLoginPassword) {
		if(!cbClient->connected())
			break;

		cbClient->update();
		NLMISC::nlSleep(10);
	}
	
	if(!VerifyLoginPassword) {
		cbClient->disconnect();
		delete cbClient;
		cbClient=0;
		nlerror("Failure in callback cycle.!");
		return 1;
	}

	if (!VerifyLoginPasswordReason.empty())	{
		cbClient->disconnect();
		delete cbClient;
		cbClient = 0;
		nlerror("Login failed: %s",VerifyLoginPasswordReason.c_str());
		return 1;
	}

	while(true) {
		if(!cbClient->connected())
			break;

		cbClient->update();
		NLMISC::nlSleep(10);

		if(gShard !=0 ) { // a shard was listed.
			NLNET::CMessage msgout("CS");
			msgout.serial(gShard);
			cbClient->send(msgout);
			gShard=0;
		}
		if(ShardChooseShard == true) {
			nlinfo("Logged in successfully.");
			break;
		}
	}

	// if we're broken we're on to the next stage. delete the callback client.
	cbClient->disconnect();
	delete cbClient;
	cbClient=0;

	// connect to the fs we chose.
	nlinfo("Logging in to: %s using cookie: %s",ShardIP.asString().c_str(),ShardCookie.setToString().c_str());
	try {
		// create the client
		cbClient=new NLNET::CCallbackClient();
		cbClient->addCallbackArray(ClientCallbackArray, sizeof(ClientCallbackArray)/sizeof(ClientCallbackArray[0]));

		// connect to the server.
		//std::string addr = "localhost:49998";
		cbClient->connect(ShardIP);

		NLNET::CMessage msgout("SV");
		msgout.serial(ShardCookie);
		cbClient->send(msgout);

		while(cbClient->connected() && !OnlineFES) {
			cbClient->update();
			NLMISC::nlSleep(10);
		}
		//std::string res = NLNET::CLoginClient::connectToShard(ShardCookie, ShardIP, *cbClient);
		//std::string res=NLNET::CLoginClient::connectToShard(ShardCookie, ShardIP.asString(), *cbClient);
	} catch(NLNET::ESocket &e) {
		delete cbClient;
		cbClient = 0;
		nlwarning ("Connection refused to LS: %s", e.what ());
	}
	
	
	int idx=0;
	while(true) {
		idx++;
		if(!cbClient->connected())
			break;

		cbClient->update();
		NLMISC::nlSleep(10);

		// we should be logged in i think.
		if(idx==100) {
			std::string str="test chat message.";
			idx=0;
			NLNET::CMessage msgout("CHAT");
			msgout.serial(str);
			cbClient->send(msgout);
		}
	}

	return 1;
}