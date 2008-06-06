/**
 * \file CLoginService.cpp
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
#include "wwcommon/IBaseSimulation.h"
#include "CLoginService.h"
#include "callbacks.h"

#include <mysql.h>

//
// Class
//

// Required by wwcommon
WWCOMMON::IBaseSimulation *getSimulation() {
	return NULL;
}

NLNET::IService * gServiceInstance=NULL;
NLMISC::CLog *gOutput=NULL;

// handy sql function
std::string sqlQuery(const std::string &query, sint32 &nbRow, MYSQL_ROW &firstRow, MYSQL_RES *&result) {
	CLoginService *loginService=(CLoginService *)gServiceInstance;
	MYSQL *db=loginService->getDatabase();
	
	nlassert(db);
	nlinfo("sqlQuery: '%s'", query.c_str());
	sint ret = mysql_query(db, query.c_str());
	if(ret != 0) {
		nlwarning("mysql_query('%s') failed: '%s'", query.c_str(),  mysql_error(db));
		return NLMISC::toString("mysql_query('%s') failed: '%s'", query.c_str(),  mysql_error(db));
	}

	if(query.find("select") == 0) {
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
	}
	return "";
}

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

NLNET_SERVICE_MAIN(CLoginService, "LS", "login_service", 49999, NLNET::EmptyCallbackArray, WW_CONFIG, WW_LOGS);

/*****
 * Callback Arrays
 *****/

/**
 * The callback array for calls coming in from the client.
 */
NLNET::TCallbackItem ClientCallbackArray[] = {
	{ "VLP", cbClientVerifyLoginPassword },
	{ "CS", cbClientChooseShard },
};

/****
 * Welcome Service callbacks.
 ****/
static const NLNET::TUnifiedCallbackItem WSCallbackArray[] =
{
	{ "CC", cbWSClientConnected },
	{ "WS_IDENT", cbWSIdentification },
	{ "SCS", cbWSShardChooseShard },
	//{ "SU", cbScoreUpdate },
};


/*****
 * CLoginService methods.
 *****/

void CLoginService::init() {
	gServiceInstance=this;

	/**
	 * Create a log for output.
	 */
	gOutput=new NLMISC::CLog;

	/**
	 * Set up the displayers.
	 */
	std::string fn=NLNET::IService::getInstance()->SaveFilesDirectory;
	fn += "login_service.stat";
	nlinfo("Login stat in directory: '%s'", fn.c_str());
	m_FileDisplay=new NLMISC::CFileDisplayer(fn);
	gOutput->addDisplayer(m_FileDisplay);
	if(WindowDisplayer != NULL)
		gOutput->addDisplayer(WindowDisplayer);

	/**
	 * Init the network related pieces, talking to shards and clients
	 */

	// so we can talk to shards
	connectionWSInit();

	// so we can talk to clients
	connectionCLInit();

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

	// we're done, flag us as ready to accept connections
	m_Init=true;
	gOutput->displayNL("Login Service Initialized.");
}

bool CLoginService::update() {
	connectionWSUpdate();
	connectionCLUpdate();

	return true;
}

void CLoginService::release() {
	if(m_Init) {
		//writePlayerDatabase ();
	}

	connectionWSRelease();
	connectionCLRelease();
		
	gOutput->displayNL ("Login Service released");
}

NLNET::CCallbackServer *CLoginService::getClientServer() {
	return m_CallbackServer;
}

std::vector<CShard> CLoginService::getShardList() {
	return m_Shards;
}

MYSQL *CLoginService::getDatabase() {
	return m_DatabaseConnection;
}

void CLoginService::beep(uint freq, uint nb, uint beepDuration, uint pauseDuration) {
#ifdef NL_OS_WINDOWS
	try {
		if(ConfigFile.getVar("Beep").asInt() == 1) {
			for(uint i = 0; i < nb; i++) {
				Beep(freq, beepDuration);
				NLMISC::nlSleep(pauseDuration);
			}
		}
	} catch(NLMISC::Exception &) {
		;
	}
#endif // NL_OS_WINDOWS
}

/****
 * Welcome Service related methods.
 ****/

void CLoginService::connectionWSInit() {
	// set up the call backs for connections to/from the welcome service.
	NLNET::CUnifiedNetwork::getInstance()->addCallbackArray (WSCallbackArray, sizeof(WSCallbackArray)/sizeof(WSCallbackArray[0]));
	NLNET::CUnifiedNetwork::getInstance()->setServiceUpCallback("WS", cbWSConnection);
	NLNET::CUnifiedNetwork::getInstance()->setServiceDownCallback ("WS", cbWSDisconnection);
}

void CLoginService::connectionWSUpdate() {
	;
}

void CLoginService::connectionWSRelease() {
	nlinfo ("I'm going down, clean the database");

	while(!m_Shards.empty())	{
		cbWSDisconnection("", m_Shards[0].SId, NULL);
	}
}

/****
 * Client connection related methods.
 ****/

void CLoginService::connectionCLInit() {
	//m_ClientServer = new NLNET::CBufServer();
	m_CallbackServer= new NLNET::CCallbackServer();

	//nlassert(m_ClientServer != NULL);
	nlassert(m_CallbackServer != NULL);

	uint16 port = (uint16)ConfigFile.getVar("ClientPort").asInt();
	//m_ClientServer->init(port);
	m_CallbackServer->init(port);

	// the servers aren't set up yet. this will be merged into the rest of the WS callbacks.
	// catch the messages from Welcome Service to know if the user can connect or not
	//CUnifiedNetwork::getInstance()->addCallbackArray (WSCallbackArray, sizeof(WSCallbackArray)/sizeof(WSCallbackArray[0]));
	// that wsa for cbufserver

	// this is for callback server
	m_CallbackServer->addCallbackArray(ClientCallbackArray, sizeof(ClientCallbackArray)/sizeof(ClientCallbackArray[0]));
	m_CallbackServer->setConnectionCallback(cbClientConnection, 0);
	m_CallbackServer->setDisconnectionCallback(cbClientDisconnection, 0);
	nlinfo("Client connections are ready on port: %hu", port);
}

void CLoginService::connectionCLUpdate() {
	// we can't really work without a socket.
	nlassert(m_CallbackServer != NULL);

	try {
		m_CallbackServer->update();
	} catch(NLMISC::Exception &e) {
		nlwarning("Error during update: '%s'", e.what ());
	}
}

void CLoginService::connectionCLRelease() {
	// init should have been called long ago.
	nlassert(m_CallbackServer != NULL);

	delete m_CallbackServer;
	m_CallbackServer = 0;
}

//
// Commands
//
void displayShards() {
	NLMISC::ICommand::execute("shards", *NLMISC::InfoLog);
}
NLMISC_COMMAND(shards, "displays the list of all registered shards", "") {
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	if(args.size() != 0) return false;

	reason = sqlQuery("select * from shards where online=1", nbrow, row, result);
	if(!reason.empty()) return false;

	if(nbrow == 0) {
		log.displayNL("No online shards were detected.");
		return true;
	}

	uint i=0;
	log.displayNL("Display the %d registered shards :", nbrow);
	while(row != 0)	{
		std::string nbplr, shardid, internalid, shardname;
		shardname=row[3];
		shardid=row[0];
		internalid=row[7];
		nbplr=row[2];
		log.displayNL("* Shard Name: %s Shard ID: %s Internal ID: %s Player Count: %s", shardname.c_str(), shardid.c_str(), internalid.c_str(), nbplr.c_str());
		NLNET::CUnifiedNetwork::getInstance()->displayUnifiedConnection(atoi(internalid.c_str()), &log);
		row = mysql_fetch_row(result);
	}
	log.displayNL ("End of the list");

	return true;
}


