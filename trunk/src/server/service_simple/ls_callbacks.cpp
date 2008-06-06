/**
 * \file ls_callbacks.cpp
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
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>
#include <nel/net/service.h>
#include <nel/misc/log.h>
#include <nel/misc/displayer.h>
#include <nel/misc/window_displayer.h>
#include <nel/misc/path.h>
#include <nel/net/login_cookie.h>

//
// Werewolf Includes
//
#include "CWelcomeService.h"
//#include "callbacks.h"
#include "CFESData.h"

#include <mysql.h>

//
// Variables.
//
NLMISC::CVariable<sint> PlayerLimit("ws", "PlayerLimit", "Rough max number of players accepted on this shard (-1 for Unlimited)",
	5000, 0, true );

/**
 * OpenFrontEndThreshold
 * The FS balance algorithm works like this:
 * - select the least loaded frontend
 * - if this frontend has more than the OpenFrontEndThreshold
 *   - try to open a new frontend
 *   - reselect least loaded frontend
 */
NLMISC::CVariable<uint> OpenFrontEndThreshold("ws", "OpenFrontEndThreshold", "Limit number of players on all FS to decide to open a new FS", 
	800, 0, true );

//
// Class
//

/**
 * \brief Called when connecting to the Login Service fails.
 *
 * This is called when a connection to the Login Service fails or it
 * receives a "FAILED" message from it. This simply nlerror's the reason.
 */
void cbFailed(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid) {
	std::string reason;
	msgin.serial(reason);
	nlerror(reason.c_str());
}

/**
 * \brief Sends an identification message to the Login Service.
 */
void cbLSConnection(const std::string &serviceName, uint16 sid, void *arg) {
	sint32 shardId, nbplayers;
	
	// get the shard id from the config.
	if(NLNET::IService::getInstance()->ConfigFile.exists("ShardId")) {
		shardId = NLNET::IService::getInstance()->ConfigFile.getVar ("ShardId").asInt();
	} else { // or error and default it if it's not there.
		shardId = -1;
		nlerror ("ShardId variable must be valid (>0)");
	}

	/**
	 * \todo Get the number of players online for this shard.
	 */
	nbplayers=0;
	// send the shard id to the LS.
	NLNET::CMessage msgout ("WS_IDENT");
	msgout.serial(shardId);
	msgout.serial(nbplayers);
	NLNET::CUnifiedNetwork::getInstance()->send(sid, msgout);

	nlinfo ("Connected to %s-%hu and sent identification with shardId '%d'", serviceName.c_str(), sid, shardId);
	/*
	 * patching business disabled.
	NLNET::CMessage msgrpn("REPORT_NO_PATCH");
	NLNET::CUnifiedNetwork::getInstance()->send("LS", msgrpn);

	bool reportPatching = false;
	list<CFES>::iterator	itfs;
	for (itfs=FESList.begin(); itfs!=FESList.end(); ++itfs)
		(*itfs).reportStateToLS(reportPatching);
	*/
}

/**
 * \brief Receives a CS message from a Login Service.
 *
 * S07: receive the "CS" message from LS and send the "CS" message to the selected FES
 */
void cbLSChooseShard(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid) {
	// the LS warns me that a new client want to come in my shard
	uint totalNbUsers;
	bool authorizeUser = false;
	bool forceAuthorize = false;
	std::string userName, userPriv, userExtended, cookieStr;
	NLNET::CLoginCookie cookie;
	msgin.serial(cookieStr);
	msgin.serial(userName);

	cookie.setFromString(cookieStr);

	/**
	 * we're not using this stuff just yet.
	try	{
		msgin.serial (userPriv);
	} catch(NLMISC::Exception &) {
		nlwarning ("LS didn't give me the user privilege for user '%s', set to empty", userName.c_str());
	}

	try	{
		msgin.serial(userExtended);
	} catch(NLMISC::Exception &) {
		nlwarning ("LS didn't give me the extended data for user '%s', set to empty", userName.c_str());
	}
	 */

	CFESData *best = CFESData::findBestFES(totalNbUsers);

	// could not find a good FES or best FES has more players than balance limit
	if (best == NULL || best->getUsersCountHeuristic() >= OpenFrontEndThreshold) {
		// open a new frontend, ignore that patch mode doesn't exist for werewolf.
		CFESData::openNewFES();

		// reselect best FES (will return newly open FES, or previous if no more FES available)
		best = CFESData::findBestFES(totalNbUsers);

		// check there is a FES available
		if (best == NULL) {
			nlwarning("No front-ends were available for a new user.");
			// answer the LS that we can't accept the user
			NLNET::CMessage msgout ("SCS");
			std::string reason = "No front-end server available";
			msgout.serial(reason);
			msgout.serial(cookieStr);
			NLNET::CUnifiedNetwork::getInstance()->send(sid, msgout);
			return;
		}
	}

	if (userPriv == ":DEV:") {
		// devs have all privileges
		authorizeUser = true;
		forceAuthorize = true;
	}
	/*
	 * this is turned off until we have AES and crap working.
	} else if (ShardOpen != ClosedForAll) {
		const std::string&	allowedGroups = OpenGroups;
		bool				userInOpenGroups = (!userPriv.empty() && !allowedGroups.empty() && allowedGroups.find(userPriv) != std::string::npos);

		// open for all or user is privileged
		authorizeUser = (ShardOpen == OpenForAll || userInOpenGroups);
		// let authorized users to force access even if limit is reached
		forceAuthorize = userInOpenGroups;
	} */

	bool shardLimitReached=( (PlayerLimit.get()!=-1) && (totalNbUsers>=(uint)PlayerLimit.get()));

	if (/*!forceAuthorize && (!authorizeUser ||*/ shardLimitReached) {
		// answer the LS that we can't accept the user
		nlwarning("Telling the LS that our shard is full.");
		NLNET::CMessage msgout ("SCS");
		std::string reason;
		if (shardLimitReached)
			reason = "The shard is currently full, please try again later.";
		else
			reason = "The shard is closed.";
		msgout.serial (reason);
		msgout.serial (cookie);
		NLNET::CUnifiedNetwork::getInstance()->send(sid, msgout);
		return;
	}


	uint32 instanceId = 0xffffffff;
	uint32 charSlot = 0;
	NLNET::CMessage msgout ("CS");
	msgout.serial (cookie);
	msgout.serial (userName, userPriv, userExtended);
	msgout.serial (instanceId);
	msgout.serial (charSlot);
	NLNET::CUnifiedNetwork::getInstance()->send(best->SId, msgout);
	best->NbEstimatedUser++;
	best->NbPendingUsers++;
}

/**
 * \brief Received a message from the Login Service to disconnect a client.
 */
void cbLSDisconnectClient(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid) {
	// the LS tells me that i have to disconnect a client

	/*
	 * MTP has this commented out.
	 */
	uint32 userid;
	msgin.serial (userid);

	std::map<uint32, NLNET::TServiceId>::iterator it = UserIdSockAssociations.find(userid);
	if (it == UserIdSockAssociations.end()) {
		nlwarning("Can't disconnect the user %d, he is not found", userid);
	} else {
		NLNET::CMessage msgout("DC");
		msgout.serial(userid);
		NLNET::CUnifiedNetwork::getInstance()->send((*it).second, msgout);
	}
}

