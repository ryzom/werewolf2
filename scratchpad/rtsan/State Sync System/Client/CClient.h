/**
 * \file CClient.h
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

#ifndef __SSS_CCLIENT_H__
#define __SSS_CCLIENT_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#ifdef NL_OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif
#include <nel/net/callback_client.h>
//#include <nel/net/udp_sock.h>
//#include <nel/net/login_cookie.h>
//#include <nel/misc/system_info.h> 
//#include <nel/net/login_client.h>


//
// Werewolf Includes
//	
#include "../Shared/CState.h"
#include "../Shared/ISingleton.h"



//
// Namespaces
//

class CClient : public ISingleton<CClient> {
public:
	void init();
	void update();
	void clear();
	void connectUsing(std::string srv, std::string usr, std::string pass);
	bool connect();
	bool isConnected();
	bool disconnect();
	int mainLoop();
	void changeState(uint32 state);
	NLNET::CCallbackClient *getClient();
	void spawnEntity(uint32 objectID);

	void addCharacter(std::string name, uint32 entid);
	uint32 getCharID(std::string name);

	uint32 getUserID();
	void setUserID(uint32 uid);
private:
	enum eCLState {
		CLSTATE_NONE = 0,
		CLSTATE_SPAWNING = 1,
	};

	eCLState				m_CLState;
	NLNET::CCallbackClient	*cbClient;
	CState					*m_state;
	std::string				m_Server, m_User, m_Pass;

	uint32					m_UserID;
	
};

#endif // __SSS_CCLIENT_H__