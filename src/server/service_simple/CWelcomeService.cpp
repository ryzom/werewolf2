/**
 * \file CWelcomeService.cpp
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


//
// Werewolf Includes
//
#include "CWelcomeService.h"
//#include "callbacks.h"
#include "wwcommon/IBaseSimulation.h"

#include <mysql.h>

//
// Class
//

// Required by wwcommon
WWCOMMON::IBaseSimulation *getSimulation() {
	return NULL;
}


//
// Callback Arrays
//

// Callback array for Login Service messages.
NLNET::TUnifiedCallbackItem LSCallbackArray[] = {
	{ "CS", cbLSChooseShard },
	{ "DC", cbLSDisconnectClient },
	{ "FAILED", cbFailed },
};

// Callback Array for message from FES
NLNET::TUnifiedCallbackItem FESCallbackArray[] = {
	{ "SCS",				cbFESShardChooseShard },
	{ "CC",					cbFESClientConnected },
//	{ "RPC",				cbFESRemovedPendingCookie },
//	{ "FEPA",				cbFESPatchAddress },
//	{ "NBPLAYERS",			cbFESNbPlayers },

//	{ "SET_SHARD_OPEN",		cbSetShardOpen },
//	{ "RESTORE_SHARD_OPEN",	cbRestoreShardOpen },
};

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

NLNET_SERVICE_MAIN (CWelcomeService, "WS", "welcome_service", 0, FESCallbackArray, WW_CONFIG, WW_LOGS);

void CWelcomeService::init() {
	std::string frontendServiceName = ConfigFile.getVar("FrontendServiceName").asString();

	//try { 
	//	m_FrontEndAddress = ConfigFile.getVar("FrontEndAddress").asString(); 
	//} catch(NLMISC::Exception &) {
	//	;
	//}

	nlinfo ("Waiting frontend services named '%s'", frontendServiceName.c_str());
	NLNET::CUnifiedNetwork::getInstance()->setServiceUpCallback(frontendServiceName, cbFESConnection, NULL);
	NLNET::CUnifiedNetwork::getInstance()->setServiceDownCallback(frontendServiceName, cbFESDisconnection, NULL);
	NLNET::CUnifiedNetwork::getInstance()->setServiceUpCallback("*", cbServiceUp, NULL);
	NLNET::CUnifiedNetwork::getInstance()->setServiceDownCallback("*", cbServiceDown, NULL);

	// add a connection to the LS
	std::string LSAddr;
	if(ConfigFile.exists("LSHost"))
		LSAddr = ConfigFile.getVar("LSHost").asString();

	// the config file must have a valid address where the login service is
	nlassert(!LSAddr.empty());

	// add default port if not set by the config file
	if(LSAddr.find(":") == std::string::npos)
		LSAddr += ":49999";

	NLNET::CUnifiedNetwork::getInstance()->addCallbackArray(LSCallbackArray, sizeof(LSCallbackArray)/sizeof(LSCallbackArray[0]));
	NLNET::CUnifiedNetwork::getInstance()->setServiceUpCallback("LS", cbLSConnection, NULL);
	NLNET::CUnifiedNetwork::getInstance()->addService("LS", LSAddr);

	// List of expected service instances
//	ConfigFile.setCallback( "ExpectedServices", cbUpdateExpectedServices );
//	cbUpdateExpectedServices( ConfigFile.getVar( "ExpectedServices" ) );

	/*
	 * read config variable ShardOpenStateFile to update
	 * 
	 */
	//cbShardOpenStateFile(ShardOpenStateFile);

	// set up DB connenctions
	MYSQL *db = mysql_init(NULL);
	if(db == NULL) {
		nlwarning ("mysql_init() failed");
		return;
	}

	std::string dbHost=ConfigFile.getVar("DatabaseHost").asString();
	std::string dbLogin=ConfigFile.getVar("DatabaseLogin").asString();
	std::string dbPass=ConfigFile.getVar("DatabasePassword").asString();
	std::string dbName=ConfigFile.getVar("DatabaseName").asString();
	m_DatabaseConnection = mysql_real_connect(db, dbHost.c_str(), dbLogin.c_str(), dbPass.c_str(), dbName.c_str(),0,NULL,0);
	if(m_DatabaseConnection == NULL || m_DatabaseConnection != db) {
		nlerror ("mysql_real_connect() failed to '%s' with login '%s' and database name '%s'", dbHost.c_str(), dbLogin.c_str(), dbName.c_str());
		return;
	}
}

void CWelcomeService::reportOnlineStatus(bool status ) {
	if(status != m_OnlineStatus ) {
		NLNET::CMessage msgout( "OL_ST" );
		msgout.serial(status);
		NLNET::CUnifiedNetwork::getInstance()->send("LS", msgout);
		m_OnlineStatus = status;
	}
}

std::string CWelcomeService::sqlQuery(const std::string &query, sint32 &nbRow, MYSQL_ROW &firstRow, MYSQL_RES *&result) {
	nlassert(m_DatabaseConnection);
	nlinfo("sqlQuery: '%s'", query.c_str());
	sint ret = mysql_query(m_DatabaseConnection, query.c_str());
	if(ret != 0) {
		nlwarning("mysql_query('%s') failed: '%s'", query.c_str(),  mysql_error(m_DatabaseConnection));
		return NLMISC::toString("mysql_query('%s') failed: '%s'", query.c_str(),  mysql_error(m_DatabaseConnection));
	}

	if(query.find("select") == 0) {
		// store result on select query
		result = mysql_store_result(m_DatabaseConnection);
		if(result == 0) {
			nlwarning("mysql_store_result() failed from query '%s': %s", query.c_str(),  mysql_error(m_DatabaseConnection));
			return NLMISC::toString("mysql_store_result() failed from query '%s': %s", query.c_str(),  mysql_error(m_DatabaseConnection));
		}

		nbRow = (sint32)mysql_num_rows(result);
		if(nbRow > 0) {
			firstRow = mysql_fetch_row(result);
			if(firstRow == 0) {
				nlwarning("mysql_fetch_row (%s) failed: %s", query.c_str (),  mysql_error(m_DatabaseConnection));
				return NLMISC::toString("mysql_fetch_row (%s) failed: %s", query.c_str (),  mysql_error(m_DatabaseConnection));
			}
		} else {
			firstRow = 0;
		}
	}
	return "";
}

MYSQL *CWelcomeService::getDatabase() {
	return m_DatabaseConnection;
}

//
// Commands.
//

/**
 * \brief Displays the number of front-ends and information about them.
 */
NLMISC_COMMAND(frontends, "displays the list of all registered front ends", "") {
	if(args.size() != 0) return false;

	log.displayNL("Display the %d registered front end :", FESList.size());
	for (std::list<CFESData>::iterator it = FESList.begin(); it != FESList.end (); it++) {
		log.displayNL ("> FE %u: nb estimated users: %u nb users: %u", (*it).SId, (*it).NbEstimatedUser, (*it).NbUser );
	}
	log.displayNL ("End ot the list");

	return true;
}
