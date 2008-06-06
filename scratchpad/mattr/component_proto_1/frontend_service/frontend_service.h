/*
 * This file contain the Component Prototype Frontend Service.
 */

/*
 * Copyright, 2007 Matt Raykowski
 *
 * This file is part of NEVRAX SNOWBALLS.
 * NEVRAX SNOWBALLS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * NEVRAX SNOWBALLS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NEVRAX SNOWBALLS; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef __FRONTEND_SERVICE_H__
#define __FRONTEND_SERVICE_H__

#include <map>

#include <nel/net/service.h>

// Callbacks
void onDisconnectClient(NLNET::TSockId from, void *arg );
void onConnectionClient(NLNET::TSockId from, void *arg);

/**
 * @brief Keep a list of players connected.
 *
 * Keep a list of the players connected to that Frontend. Only map Id
 * to a Connection
 */
struct CPlayer {
	CPlayer(uint32 Id, NLNET::TSockId Con) : id(Id), con(Con) { }
	uint32   id;
	NLNET::TSockId  con;
};

/// Map a player to an ID.
typedef std::map<uint32, CPlayer> TPlayerMap;

/**
 * @brief The Component Prototype Frontend Service
 */
class CFrontendService : public NLNET::IService {
public:
	void init();
	bool update();
protected:
	TPlayerMap m_LocalPlayers;
	NLNET::CCallbackServer *m_Clients;
};

#ifndef FS_CONFIG
#define FS_CONFIG ""
#endif // FS_CONFIG

#ifndef FS_LOGS
#define FS_LOGS ""
#endif // FS_LOGS


#endif // __FRONTEND_SERVICE_H__

