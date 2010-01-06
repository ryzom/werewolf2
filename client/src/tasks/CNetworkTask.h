/**
 * \file CNetworkTask.h
 * \date January 2005
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

#ifndef __CNETWORKTASK_H__
#define __CNETWORKTASK_H__

//
// System Includes
//
#include "CEGUI.h"

//
// NeL Includes
//
#include <nel/net/login_cookie.h>
#include <nel/net/callback_client.h>

//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Namespaces
//

namespace WWCLIENT {

//
// Class
//


/**
 * \class CNetworkTask CNetworkTask.h "tasks\CNetworkTask.h"
 * \brief The core network logic task.
 */
class CNetworkTask : public WWCOMMON::ISingleton<CNetworkTask>, public WWCOMMON::ITask {
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();
	virtual void stop();
	virtual std::string name() { return "CNetworkTask"; }
	std::string connectToLoginServer(std::string user, std::string pass);
	std::string connectToShard(uint shardid);
	std::string connect();
	void setSelfId(uint32 id);

	bool connected() { if(m_NetworkState!=eOnline) return false; return m_Client->connected(); }

	// state of the network.
	enum TState { eOffline, eOnline };
	TState getState() { return m_NetworkState; };

	/// Sending comm messages.
	void sendChat(std::string chatLine);

	void send(NLNET::CMessage msgout);

	/// Allow outsiders to get the client to send custom messages.
	NLNET::CCallbackClient *getClient();

	/**
	 * \brief Shard information.
	 */
	struct CShard {
		CShard() { ShardNbPlayers = 0; }
		CShard(const std::string &name, uint8 nbp, uint32 sid) {
			ShardName=name;
			ShardNbPlayers=nbp;
			ShardId=sid;
		}
		std::string ShardName;
		uint8		ShardNbPlayers;
		uint32		ShardId;
	};
	typedef std::vector<CShard> TShardList;
	TShardList getShardList() { return m_ShardList; }

//	void cbVLP(NLNET::CMessage msgin);
//	void cbSCS(NLNET::CMessage msgin);
    NLNET::CInetAddress getShardIp() { return m_ShardIP; }
    void setShardIp(NLNET::CInetAddress addr) { m_ShardIP = addr; }
    void setShardIp(std::string addr) { m_ShardIP.setNameAndPort(addr); }

    NLNET::CLoginCookie getLoginCookie() { return m_LoginCookie; }
    void setLoginCookie(NLNET::CLoginCookie cookie) { m_LoginCookie = cookie; }
    void setLoginCookie(std::string cookie) { m_LoginCookie.setFromString(cookie); }
private:
	// verifying the login vars.
//	bool m_VLP;
//	std::string m_VLPReason;

	// verifying shard connection vars.
	bool m_ConnectShard;
	std::string m_ShardConnectReason;
	NLNET::CInetAddress m_ShardIP;

	// network connectivity stuff.
	NLNET::CLoginCookie		m_LoginCookie;
	NLNET::CCallbackClient	*m_Client;

	TState m_NetworkState;

	uint32 m_SelfNetworkId;

	/**
	 * \brief Contains a list of all shards available.
	 */
	TShardList		m_ShardList;
};

}; // END NAMESPACE WWCLIENT

#endif // __CNETWORKTASK_H__
