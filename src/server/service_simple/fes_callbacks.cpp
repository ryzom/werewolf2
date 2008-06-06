/**
 * \file fes_callbacks.cpp
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
#include "CFESData.h"

#include <mysql.h>

//
// Class
//

/**
 * \brief Called when the service comes online.
 */
void cbServiceUp(const std::string &serviceName, uint16 sid, void *arg) {
	CWelcomeService *ws=(CWelcomeService *)NLNET::IService::getInstance();
	//OnlineServices.addInstance( serviceName );
	//fs->reportOnlineStatus( OnlineServices.getOnlineStatus() );
	nlinfo("service up");

	// get the shard id from the config file.
	sint32 shardId;
	if(NLNET::IService::getInstance()->ConfigFile.exists("ShardId")) {
		shardId = NLNET::IService::getInstance()->ConfigFile.getVar("ShardId").asInt();
	} else { // if it wasn't there, error and default.
		shardId = -1;
		nlerror ("ShardId variable must be valid (>0)");
	}

	// send shard id to service
	NLNET::CMessage msgout("R_SH_ID");
	msgout.serial(shardId);
	NLNET::CUnifiedNetwork::getInstance()->send(sid, msgout);
}


/**
 * \brief Called when the service goes offline.
 */
void cbServiceDown(const std::string &serviceName, uint16 sid, void *arg) {
	//OnlineServices.removeInstance( serviceName );
	//reportOnlineStatus( OnlineServices.getOnlineStatus() );
	nlinfo("service down.");
}

/**
 * \brief A new front end is connecting.
 */
void cbFESConnection (const std::string &serviceName, uint16 sid, void *arg) {
	FESList.push_back(CFESData((NLNET::TServiceId)sid));
	nldebug("new FES connection: sid %u", sid);
	//displayFES();

	bool	dummy;
	FESList.back().reportStateToLS(dummy);

	// Reset NbEstimatedUser to NbUser for all front-ends
	for (std::list<CFESData>::iterator it = FESList.begin(); it != FESList.end(); it++) {
		(*it).NbEstimatedUser = (*it).NbUser;
	}

//	if (!UsePatchMode.get()) {
		FESList.back().setToAcceptClients();
//	}
}


/**
 * \brief A frontend closed its connection.
 */
void cbFESDisconnection (const std::string &serviceName, uint16 sid, void *arg) {
	nldebug("new FES disconnection: sid %u", sid);

	for (std::list<CFESData>::iterator it = FESList.begin(); it != FESList.end(); it++) {
		if ((*it).SId == sid) {
			// send a message to the LS to say that all players from this FES are offline
			std::map<uint32, NLNET::TServiceId>::iterator itc = UserIdSockAssociations.begin();
			std::map<uint32, NLNET::TServiceId>::iterator nitc = itc;
			while (itc != UserIdSockAssociations.end()) {
				nitc++;
				if ((*itc).second == sid) {
					// bye bye little player
					uint32 userid = (*itc).first;
					nlinfo ("Due to a frontend crash, removed the player %d", userid);
					NLNET::CMessage msgout("CC");
					msgout.serial(userid);
					uint8 con = 0;
					msgout.serial(con);
					NLNET::CUnifiedNetwork::getInstance()->send("LS", msgout);
					UserIdSockAssociations.erase(itc);
				}
				itc = nitc;
			}

			bool dummy;
			(*it).reportStateToLS(dummy, false);

			// remove the FES
			FESList.erase (it);

			break;
		}
	}

//	displayFES ();
}

/**
 * \brief FES is informing WS about a client choosing it.
 *
 * S09: receive "SCS" message from FES and send the "SCS" message to the LS
 */
void cbFESShardChooseShard(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid) {
	CWelcomeService *ws=(CWelcomeService *)NLNET::IService::getInstance();

	// the WS answer a user authorize
	std::string reason, addr, cookieStr;
	NLNET::CLoginCookie cookie;

	// process incoming message.
	msgin.serial (reason);
	msgin.serial (cookie);
	cookieStr=cookie.setToString();

	nlinfo("Sending SCS message to LS: %s", reason.c_str());

	// construct outgoing message.
	NLNET::CMessage msgout ("SCS");
	msgout.serial(reason);
	msgout.serial(cookieStr);
	
	if (reason.empty()) { // no error.
		msgin.serial (addr);

		//nlinfo("Address received: %s", addr.c_str());
		// if we set the FontEndAddress in the welcome_service.cfg we use this address
		//if (ws->getFrontendAddress().empty()) {
			msgout.serial(addr);
		//} else {
		//	msgout.serial(ws->getFrontendAddress());
		//}
	}
	
	NLNET::CUnifiedNetwork::getInstance()->send("LS", msgout);
}

// 
/**
 * \brief This function is called when a FES accepts a new client or loses a connection to a client.
 *
 * S15: receive "CC" message from FES and send "CC" message to the "LS"
 */
void cbFESClientConnected(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid) {
	uint32 userid;
	uint8 con;

	// decode incoming message.
	msgin.serial(userid);
	msgin.serial(con);

	// build outgoing message.
	NLNET::CMessage msgout ("CC");
	msgout.serial(userid);
	msgout.serial(con);

	// inform the login service.
	NLNET::CUnifiedNetwork::getInstance()->send("LS", msgout);

	// add or remove the user number really connected on this shard
	for(std::list<CFESData>::iterator it = FESList.begin(); it != FESList.end(); it++) {
		if((*it).SId == sid) {
			if(con) {
				(*it).NbUser++;

				// the client connected, it's no longer pending
				if ((*it).NbPendingUsers > 0)
					(*it).NbPendingUsers--;
			} else {
				if ( (*it).NbUser != 0 )
					(*it).NbUser--;
			}
			break;
		}
	}

	if (con) {
		// we know that this user is on this FES
		UserIdSockAssociations.insert(std::make_pair (userid, sid));
	} else {
		// remove the user
		UserIdSockAssociations.erase(userid);
	}
}
