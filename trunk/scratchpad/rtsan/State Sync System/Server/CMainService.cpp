/**
 * \file CMainService.cpp
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

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "CMainService.h"
#include "../Shared/CEntityMap.h"
#include "../Shared/CMessageTranslator.h"
#include "CEntityManager.h"
#include "CCharacterManager.h"
#include "callbacks.h"

//
// Namespaces
//

/**
 * The callback array for calls coming in from the client.
 */
NLNET::TCallbackItem ClientCallbackArray[] = {
	{ "PING", cbPing },
	{ "ST", cbStatus },
	{ "SPW", cbSpawn },
	{ "LGCL", cbLGCharList },
	{ "VLP", cbVLP }
};


NLNET_SERVICE_MAIN (CMainService, "MS", "server", 0, NLNET::EmptyCallbackArray, "", "")

void CMainService::init() {

	// Create the server where the client must connect into
	m_ClientServer = new NLNET::CCallbackServer();
	nlassert(m_ClientServer!=0);

	m_ClientServer->init(50005);

	m_ClientServer->addCallbackArray(ClientCallbackArray, sizeof(ClientCallbackArray)/sizeof(ClientCallbackArray[0]));
	m_ClientServer->setConnectionCallback(cbClientConnection, 0);
	m_ClientServer->setDisconnectionCallback(cbClientDisconnection, 0);

	// initialize the entity manager.
	CEntityManager::instance().init();
	CCharacterManager::instance().init();
	CMessageTranslator::instance().init();

	nlinfo("Client connections are ready.");
}

bool CMainService::update() {
	// Manage messages from clients
	m_ClientServer->update ();

	// tell the entity manager to process entity updates.
	CEntityManager::instance().update();

	// we want to continue
	return true;
}

void CMainService::release() {
	if(m_ClientServer) {
		delete m_ClientServer;
		m_ClientServer = 0;
	}
	m_Players.clear();
}

NLNET::CCallbackServer *CMainService::getClientServer() {
	return m_ClientServer;
}

void CMainService::sendMessage(NLNET::CMessage msg, NLNET::TSockId dest) {
	m_ClientServer->send(msg,dest);
}

CPlayer *CMainService::getPlayer(uint32 id) {
	return &(m_Players.find(id)->second);
}

uint32 CMainService::addPlayer(uint32 uid, NLNET::TSockId con) {
	m_Players.insert(std::make_pair(uid, CPlayer(0, con)));
	return uid;
}

void CMainService::removePlayer(uint32 id) {
	CPlayer *p = getPlayer(id);
	nldebug("Removing player %d which was using entity %d",id,p->entity);
	CEntityManager::instance().unSpawnEntity(p->entity);
	m_Players.erase(id);
}

void CMainService::spawnOnPlayer(uint32 id, uint32 entid) {
	CPlayer *plr=getPlayer(id);
	plr->entity=entid;
}

sint32 CMainService::authorizePlayer(std::string user, std::string pass) {
	if(user=="mattr" && pass=="banana") {
		return 0;
	} else if(user=="rtsan" && pass=="test") {
		return 1;
	}
	return -1;
}


/**
 * \brief Displays the number of front-ends and information about them.
 */
NLMISC_COMMAND(onlineUsers, "displays the list of all connected users", "") {
	CMainService *ms=(CMainService *)NLNET::IService::getInstance();

	if(args.size() != 0) return false;

	log.displayNL("There are %d connected users...", CEntityMap::instance().nbActiveEntity());
	//for (std::list<CFESData>::iterator it = FESList.begin(); it != FESList.end (); it++) {
	//	log.displayNL ("> FE %u: nb estimated users: %u nb users: %u", (*it).SId, (*it).NbEstimatedUser, (*it).NbUser );
	//}
	log.displayNL ("End of the list");

	return true;
}