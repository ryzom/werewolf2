/**
 * \file CFrontendService.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include "stdpch.h"
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/service.h>
#include <nel/misc/displayer.h>
#include <nel/misc/path.h>

//
// Werewolf Includes
//
//#ifdef NL_OS_WINDOWS
	// include this so that we link correctly to wwcommon
//	#include <wwcommon/stdpragma.h>
//#endif // NL_OS_WINDOWS

#include <wwcommon/CGameEventRegistrar.h>

#include "CFrontendService.h"
#include "CCharacterManager.h"
#include "CServerSimulation.h"
#include "CGameEventManager.h"
#include "CEventFilterTest.h"
#include "CProximityManager.h"

// callbacks
#include "callbacks.h"

#include <mysql.h>

//
// Callbacks
//
/**
 * \brief Contains all callbacks from client
 */
NLNET::TCallbackItem ClientCallbackArray[] = {
	{ "LGCL",			cbLGCharList			},
	{ "EVT",			cbSimEventClient		},
	{ "CHAT",			cbChatClient	},
	{ "PINGREQ",		cbPingRequestClient		},
};

/**
 * Contains *all* callbacks from the shard
 */
//NLNET::TUnifiedCallbackItem CallbackArray[] = {
//	{ "CHAT",			cbChatService		},
//	{ "ADD_ENTITY",		cbAddService		},
//	{ "ENTITY_POS",		cbPosService		},
//	{ "REMOVE_ENTITY",	cbRemoveService		},
//	{ "SNOWBALL",		cbSnowballService	},
//	{ "HIT",			cbHitService		},
//};

//
// Variables
//

//
// Class
//

//
// Get the log and config dirs
//
#ifndef WW_CONFIG
#define WW_CONFIG ""
#endif // WW_CONFIG

#ifndef WW_LOGS
#define WW_LOGS ""
#endif // WW_LOGS

// I hate doing this, we should patch NeL to explicitly use its own namespace.
using namespace NLMISC;

// handy sql function
std::string sqlQuery(const std::string &query, sint32 &nbRow, MYSQL_ROW &firstRow, MYSQL_RES *&result) {
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	MYSQL *db=fs->getDatabase();

	nlassert(db);
	nlinfo("sqlQuery: '%s'", query.c_str());
	sint ret = mysql_query(db, query.c_str());
	if(ret != 0) {
		nlwarning("mysql_query('%s') failed: '%s'", query.c_str(),  mysql_error(db));
		return NLMISC::toString("mysql_query('%s') failed: '%s'", query.c_str(),  mysql_error(db));
	}

	if(query.find("select") == 0 || query.find("SELECT") == 0) {
		// store result on select query
		result = mysql_store_result(db);
		if(result == 0) {
			nlwarning("mysql_store_result() failed from query '%s': %s", query.c_str(),  mysql_error(db));
			return NLMISC::toString("mysql_store_result() failed from query '%s': %s", query.c_str(),  mysql_error(db));
		}

		nbRow = (sint32)mysql_num_rows(result);
		if(nbRow > 0) {
			firstRow = mysql_fetch_row(result);
			if(firstRow == 0) {
				nlwarning("mysql_fetch_row (%s) failed: %s", query.c_str (),  mysql_error(db));
				return NLMISC::toString("mysql_fetch_row (%s) failed: %s", query.c_str (),  mysql_error(db));
			}
		} else {
			firstRow = 0;
		}
	} else {
		nlinfo("Processing non-SELECT query.");
	}

	return "";
}


NLNET_SERVICE_MAIN(CFrontendService, "FS", "service_frontend", 0, /*CallbackArray*/ NLNET::EmptyCallbackArray, WW_CONFIG, WW_LOGS)

void CFrontendService::init() {
	nlinfo("Initializing frontend service.");
	//CServerSimulation *server=(CServerSimulation *)getSimulation();

	try
	{
		NLMISC_REGISTER_CLASS(CSimulationImpl);
	}
	catch(NLMISC::ERegisteredClass &e)
	{
		nlinfo("CSimulationImpl is already registered.");
	}

	// Register game/sob events.
	WWCOMMON::registerEvents();

	OF_REGISTER(WWCOMMON::CSobFactory, CActor, "sobActor");

	// set up data path
	NLMISC::CPath::addSearchPath("server-data", true, false);

	// Create the server where the client must connect into
	// In a real game, it should be an UDP server with specific protocol to manage packet lost and so on.
	m_ClientServer = new NLNET::CCallbackServer();
	nlassert(m_ClientServer!=0);

	m_ClientServer->addCallbackArray(ClientCallbackArray, sizeof(ClientCallbackArray)/sizeof(ClientCallbackArray[0]));

	// Set the callbacks for the client disconnection of the Frontend
	m_ClientServer->setDisconnectionCallback(onDisconnectClient, 0);

	nlinfo("Initializing client connection server.");
	m_ClientServer->init(37000);
	nlinfo("Client connection server initialization complete.");
	nlinfo(" *** Listening as: %s", m_ClientServer->listenAddress().asIPString().c_str());

	// Connect the frontend to the login system
	NLNET::CLoginServer::init(*m_ClientServer, onConnectionClient);

	/*
	 * Set the callback function when the Position service reconnect to the
	 * frontend
	 */
	//NLNET::CUnifiedNetwork::getInstance ()->setServiceUpCallback ("POS", onReconnectPosition, 0);

	/*
	 * Set the callback function when the Position service disconnect from
	 * frontend
	 */
	//NLNET::CUnifiedNetwork::getInstance ()->setServiceDownCallback ("POS", onDisconnectPosition, 0);

	/*
	 * Initialize database.
	 */
	std::string dbName, dbUser, dbPass, dbHost;
	if(ConfigFile.exists("DatabaseName"))
		dbName = ConfigFile.getVar("DatabaseName").asString();
	else
		dbName = "wwshard";
	if(ConfigFile.exists("DatabaseUser"))
		dbUser = ConfigFile.getVar("DatabaseUser").asString();
	else
		dbUser = "root";
	if(ConfigFile.exists("DatabasePass"))
		dbPass = ConfigFile.getVar("DatabasePass").asString();
	else
		dbPass = "";
	if(ConfigFile.exists("DatabaseHost"))
		dbHost = ConfigFile.getVar("DatabaseHost").asString();
	else
		dbHost = "localhost";

	// set up DB connenctions
	MYSQL *db = mysql_init(NULL);
	if(db == NULL) {
		nlwarning ("mysql_init() failed");
		return;
	}

	my_bool opt = true;
	if (mysql_options (db, MYSQL_OPT_RECONNECT, &opt))
	{
		mysql_close(db);
		m_DatabaseConnection = 0;
		nlerror("mysql_options() failed for database connection to '%s'", dbHost.c_str());
		return;
	}

	m_DatabaseConnection = mysql_real_connect(db, dbHost.c_str(), dbUser.c_str(),
		dbPass.c_str(), dbName.c_str(),0,NULL,0);
	if(m_DatabaseConnection == NULL || m_DatabaseConnection != db) {
		nlerror ("mysql_real_connect() failed to '%s' with login '%s' and database name '%s'",
			dbHost.c_str(), dbUser.c_str(), dbName.c_str());
		return;
	}

#if MYSQL_VERSION_ID < 50019
	opt = true;
	if (mysql_options(m_DatabaseConnection, MYSQL_OPT_RECONNECT, &opt)) {
		mysql_close(db);
		m_DatabaseConnection = 0;
		nlerror("mysql_options() failed for database connection to '%s'", dbHost.c_str());
		return;
	}
#endif


	// Initialize Frontend Managers/Tasks
	CCharacterManager::instance().init();
	CGameEventManager::instance();

	nlinfo("Server Sim Init.");
	getServerSimulation()->init();

}

bool CFrontendService::update() {
	//CServerSimulation *serversim=(CServerSimulation *)getSimulation();

	// Manage messages from clients
	m_ClientServer->update();

	// update the time.
	getServerSimulation()->updateTime();

	// process the server simulation each tick.
	getServerSimulation()->update();

	// update the proximity system
	CProximityManager::instance().update();

	// we want to continue
	return true;
}

void CFrontendService::sendMessage(NLNET::CMessage msg, NLNET::TSockId dest) {
	m_ClientServer->send(msg,dest);
}

void CFrontendService::sendMessage(NLNET::CMessage msg) {
	m_ClientServer->send(msg, NLNET::InvalidSockId);
}

void CFrontendService::sendMessage(NLNET::CMessage msg, uint32 sobid) {
	PlayerMap::iterator itr=m_Players.begin();
	while(itr != m_Players.end()) {
		if( (itr->second).SobID == sobid ) {
//			nlinfo("Sending message to player: %d", itr->first);
			sendMessage(msg,(itr->second).Con);
		}
		itr++;
	}
}

void CFrontendService::removePlayer(uint32 plrid) {
	CPlayer *p = getPlayer(plrid);
	nldebug("Removing player %d which was using entity %d",plrid,p->SobID);
	m_Players.erase(plrid);
}

uint32 CFrontendService::addPlayer(uint32 uid, NLNET::TSockId con) {
	m_Players.insert(std::make_pair(uid, CPlayer(0, con)));
	return uid;
}

CPlayer *CFrontendService::getPlayer(uint32 id) {
	return &(m_Players.find(id)->second);
}

MYSQL *CFrontendService::getDatabase() {
	return m_DatabaseConnection;
}
