/**
 * \file CMainService.h
 * \date February 2005
 * \author Henri Kuuste
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

#ifndef __SSS_CMAINSERVICE_H__
#define __SSS_CMAINSERVICE_H__

//
// Standard Includes
//

//
// System Includes
//
#include <map>
#include <vector>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/service.h>
#include <nel/misc/displayer.h>

//
// Werewolf Includes
//
#include "../Shared/CEntity.h"

//
// Namespaces
//

struct CPlayer {
	CPlayer(uint32 Entity, NLNET::TSockId Con) : entity(Entity), con(Con) { };
	~CPlayer() { };
	uint32 entity;
	NLNET::TSockId  con;
};

typedef std::map<uint32, CPlayer> PlayerMap;

/**
 * \brief The service that does all the work.
 */
class CMainService : public NLNET::IService {
public:
	/**
	 * \brief Initializes the service.
	 */
	void init();
	bool update();
	void release();

	CPlayer	*getPlayer(uint32 id);
	uint32	addPlayer(uint32 uid, NLNET::TSockId con);
	void	removePlayer(uint32 id);
	void	spawnOnPlayer(uint32 id, uint32 entid);

	/**
	 * \brief Authenticate a player.
	 *
	 * \param user Username to authenticate.
	 * \param pass Password for authenticating user.
	 *
	 * \return UserID if authenticated or -1 if failed.
	 */
	sint32 authorizePlayer(std::string user, std::string pass);

	NLNET::CCallbackServer *getClientServer();
	void sendMessage(NLNET::CMessage msg, NLNET::TSockId dest);

private:
	NLNET::CCallbackServer	*m_ClientServer;
	PlayerMap				m_Players;
	std::vector<CEntity>	m_Entities;
};

#endif // __SSS_CMAINSERVICE_H__