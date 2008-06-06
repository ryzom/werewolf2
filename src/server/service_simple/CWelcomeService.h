/**
 * \file CWelcomeService.h
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

#ifndef __CWELCOMESERVICE_H__
#define __CWELCOMESERVICE_H__

//
// System Includes
//
#include <vector>
#include <list>
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/service.h>
#include <nel/misc/displayer.h>

//
// Werewolf Includes
//
#include "CFESData.h"

#include <mysql.h>

//
// Callbacks
//

// Global callbacks.
void cbServiceUp(const std::string &serviceName, uint16 sid, void *arg);
void cbServiceDown(const std::string &serviceName, uint16 sid, void *arg);

// Login Service callbacks
void cbFailed(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid);
void cbLSConnection(const std::string &serviceName, uint16 sid, void *arg);
void cbLSDisconnectClient(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid);
void cbLSChooseShard(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid);

// Frontend Service callbacks
void cbFESDisconnection (const std::string &serviceName, uint16 sid, void *arg);
void cbFESConnection (const std::string &serviceName, uint16 sid, void *arg);
void cbFESShardChooseShard(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid);
void cbFESClientConnected(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid);

//
// Variables
//
extern std::list<CFESData> FESList;
extern std::map<uint32, NLNET::TServiceId> UserIdSockAssociations;

//
// Class
//

class CWelcomeService : public NLNET::IService {
public:
	/// Init the service, load the universal time.
	void init();
	void reportOnlineStatus(bool status);

	// handy sql functions
	std::string sqlQuery(const std::string &query, sint32 &nbRow, MYSQL_ROW &firstRow, MYSQL_RES *&result);
	MYSQL *getDatabase();
	std::string getFrontendAddress() { return m_FrontEndAddress; }

private:
	std::string m_FrontEndAddress;
	bool	m_OnlineStatus;
	MYSQL	*m_DatabaseConnection;
};

#endif // __CWELCOMESERVICE_H__

