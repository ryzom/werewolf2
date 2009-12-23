/**
 * \file CFrontendService.h
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

#ifndef __CFRONTENDSERVICE_H__
#define __CFRONTENDSERVICE_H__

//
// System Includes
//
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/service.h>
#include <nel/misc/displayer.h>
#include <nel/misc/vector.h>

//
// Werewolf Includes
//
#include <mysql.h>

#include "CUserManager.h"

//
// Callbacks
//

//
// Variables
//

//
// Class
//

std::string sqlQuery(const std::string &query, sint32 &nbRow, MYSQL_ROW &firstRow, MYSQL_RES *&result);

/**
 * \brief Contains network-level player information.
 */
struct CPlayer {
	/// Data-filling constructor.
	CPlayer(uint32 sobid, NLNET::TSockId con) : Con(con) { };
	/// Default destructor.
	~CPlayer() { };

	/// The Simulation Object ID that this player is controlling.
	//uint32 SobID;

	/// Whether this player is connected to the game.
	//bool Connected;

	/// Holds the connection for this client.
	NLNET::TSockId  Con;

	/// This is the character that this player logged in as.
	//uint32 CharacterID;

	/// The user object associated with this connection.
	//CUser *User;
};

/**
 * \brief The service at the core of it all.
 */
class CFrontendService : public NLNET::IService {
public:
	/**
	 * \brief Initializes the service. Sets up the login server.
	 */
	void init();
	bool update();

	/**
	 * \brief Send a message.
	 */
	void sendMessage(NLNET::CMessage msg, NLNET::TSockId dest);

	/**
	 * \brief Send a message to all connected clients.
	 */
	void sendMessage(NLNET::CMessage msg);

	/**
	 * \brief Send a message to a client whom owns the sob id.
	 */
	void sendMessage(NLNET::CMessage msg, uint32 sobid);

	void removePlayer(uint32 plrid);

	/**
	 * \brief Register a player to an ID as active in the map.
	 *
	 * \param plrid The userid of a player.
	 * \param plr The player object.
	 * \return The userid registered.
	 */
	uint32	addPlayer(uint32 uid, NLNET::TSockId con);

	/**
	 * \brief Get a player.
	 *
	 * \param id The UserID to find.
	 * \return The CPlayer the id represents.
	 */
	CPlayer *getPlayer(uint32 id);

	/**
	 * \brief Save a player to the users table.
	 *
	 * \param plr The player to be saved/updated.
	 * \return Whether the player was saved or not.
	 */
//	bool savePlayer(CPlayer *plr);

	/**
	 * \brief Returns a reference to the database connection.
	 */
	MYSQL *getDatabase();
private:
	NLNET::CCallbackServer	*m_ClientServer;

	typedef std::map<uint32, CPlayer *> PlayerMap;
	PlayerMap				m_Players;

	MYSQL					*m_DatabaseConnection;
};

#endif // __CFRONTENDSERVICE_H__
