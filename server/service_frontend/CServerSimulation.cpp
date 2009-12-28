/**
 * \file CSimulationImpl.cpp
 * \date May 2005
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
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/message.h>
#include <nel/misc/path.h>

//
// Werewolf Includes
//
#include "CFrontendService.h"

#include "wwcommon/ISimulationObj.h"
#include "wwcommon/CPerformer.h"
#include "CActor.h"

#include "wwcommon/CSobManager.h"
#include "wwcommon/CSobFactory.h"

#include "wwcommon/IBaseSimulation.h"
#include "CServerSimulation.h"

#include "CCharacterManager.h"

#include "wwcommon/CControllerFactory.h"
#include "wwcommon/IController.h"
#include "CPerformerDRController.h"
#include <wwcommon/CSobHandlerFactory.h>
#include <wwcommon/CMotionController.h>
#include "CServerObjectView.h"
#include <wwcommon/CSobSpawnEvent.h>
#include "CProximityManager.h"
#include "CProximityController.h"

//#include "CSimEventGenerator.h"

//
// Namespaces
//

//WWCOMMON::IBaseSimulation *Simulation;

//WWCOMMON::IBaseSimulation *getSimulation() {
//	if(Simulation==NULL) {
//		Simulation=static_cast<WWCOMMON::IBaseSimulation *>(new CSimulationImpl());
//	}
//	return static_cast<CSimulationImpl *>(Simulation);
//}

CSimulationImpl *getServerSimulation() {
        return dynamic_cast<CSimulationImpl *>(getSimulation());
}


WWCOMMON::ISimulationObj *CSimulationImpl::initSob(std::string name) {
	CActor *sob=dynamic_cast<CActor *>(getNewSob("sobActor"));
	sob->setOwnerSobId(m_RootSob->getSobId());

	// put it in our manager
	m_ActorManager->addSob(sob);

	return sob;
}

bool CSimulationImpl::userLogout(uint32 sobid) {
	CActor *sob=dynamic_cast<CActor *>(m_ActorManager->find(sobid));
	if(sob==NULL) {
		nlwarning("Failed to locate sob to unspawn.");
		return false;
	}

	// TODO henri:everyone Very bad spot for this! Should be in SOV or proximity stuff. Register CProximityView as event listener?
	CProximityManager::instance().removeSob((WWCOMMON::CPerformer*)sob);

	nlinfo("Removing sob %d from simulation.", sobid);
	m_ActorManager->removeSob(sob);

	// Get the user and set the Sob ID to zero (0).
	CUser *user = CUserManager::instance().getUserById(sob->getPlayerId());
	user->SobID=0;
	CUserManager::instance().saveUser(user);

	delete sob;

	return true;
}

bool CSimulationImpl::userLogin(uint32 uid, uint32 objid) {
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();

	// check if the requested ID belongs to the player.
	if(!CCharacterManager::instance().isPlayers(objid,uid)) {
		nlwarning("A user (%d) just tried to use another user's character (%d)!", uid, objid);
		return false;
	}

	WWCOMMON::CCharacterData *charData = CCharacterManager::instance().getCharacter(objid);

	// create the sob and add it to the manager.
	CActor *sob=dynamic_cast<CActor *>(initSob("sobActor"));

	// now attach the sob.
	if(!attachUser(uid, sob->getSobId())) {
		nlwarning("Attaching user (%d) to sob (%d) failed!", uid, sob->getSobId());
		return false;
	}

	// get the user and save the sobid.
	CUser *user = CUserManager::instance().getUserById(uid);
	user->SobID=sob->getSobId();
	user->CharacterID = charData->CharacterID;
	CUserManager::instance().saveUser(user);

	// set it's properties
	//sob->setProp("foo", "bar");

	// save the sob's position.
	sob->setPosition(NLMISC::CVector(1840.0, -970.0, 0.0));
	sob->setGlobalPosition(NLMISC::CVector(1840.0, -970.0, 0.0));

	// TODO henri:everyone make this data based.
	sob->addHandler(WWCOMMON::CSobHandlerFactory::instance().getHandler(WWCOMMON::CSobHandlerFactory::H_MOTION));
	sob->addHandler(WWCOMMON::CSobHandlerFactory::instance().getHandler(WWCOMMON::CSobHandlerFactory::H_MOTION_REQUEST));

	sob->addController(new CPerformerDRController(sob, 10.0));
	sob->addController(new WWCOMMON::CMotionController(sob));
	sob->addController(new CProximityController(sob, 60.0));

	sob->addView(new CServerObjectView(sob));

	WWCOMMON::CSobSpawnEvent* event = new WWCOMMON::CSobSpawnEvent();
	event->SourceID = sob->getSobId();
	event->TargetID = sob->getSobId();
	event->Position = sob->getPosition();
	event->EMD = charData->EmdType;

	WWCOMMON::CGameEventServer::instance().postEvent(event);

	if(charData)
		delete charData;

	return true;
}

bool CSimulationImpl::attachUser(uint32 uid, uint32 sobid) {
	nlinfo("Server simulation is attaching uid %d to sobid %d", uid, sobid);
	CActor *sob=dynamic_cast<CActor *>(m_ActorManager->find(sobid));
	if(sob==NULL) {
		// the sobid wasn't in our database.
		nlwarning("User tried to attach to a sob that doesn't exist or is not an actor: %d", sobid);
		return false;
	} else {
		// we will set up a control state here later. but for now just associate the uid.
		sob->m_PlrId=uid;
		CUser *user = CUserManager::instance().getUserById(uid);
		user->SobID=sobid;
		CUserManager::instance().saveUser(user);
		return true;
	}
}

void CSimulationImpl::detachUser(uint32 uid, uint32 sobid) {
	CActor *sob=dynamic_cast<CActor *>(m_ActorManager->find(sobid));
	if(sob==NULL) {
		// the sobid wasn't in our database.
		nlwarning("User tried to attach to a sob that doesn't exist: %d", sobid);
		return;
	} else {
		nlinfo("Detaching user from simulation of sob: %d", sob->getSobId());

		// reset the player id.
		CUser *user = CUserManager::instance().getUserById(uid);
		user->SobID=0;
		CUserManager::instance().saveUser(user);
		sob->m_PlrId=0;
		
	}
}

void CSimulationImpl::init() {

	std::string retBankName, globRetBank;
	NLMISC::CConfigFile ConfigFile=NLNET::IService::getInstance()->ConfigFile;

	retBankName=ConfigFile.getVar("RetrieverBankName").asString();
	globRetBank=ConfigFile.getVar("GlobalRetrieverName").asString();

	nlinfo("Create Retriever Bank: %s", NLMISC::CPath::lookup(retBankName).c_str());
	nlinfo("Create Global Retriever: %s", NLMISC::CPath::lookup(globRetBank).c_str());

	// pacs stuff.
	m_RetrieverBank = NLPACS::URetrieverBank::createRetrieverBank(retBankName.c_str());
	if(m_RetrieverBank == NULL)
		nlinfo("Retriever bank creation failed!");
	m_GlobalRetriever = NLPACS::UGlobalRetriever::createGlobalRetriever(globRetBank.c_str(), m_RetrieverBank);

	nlinfo("Create Move Container.");
	m_MoveContainer = NLPACS::UMoveContainer::createMoveContainer(m_GlobalRetriever, 100, 100, 6.0);

	WWCOMMON::IBaseSimulation::init();

	m_RootSob=getNewSob("sobActor", 0);

	// the server does not lag behind.
	m_SimulationDelay=0;
}

void CSimulationImpl::update() {
	// process the basic functions.
	WWCOMMON::IBaseSimulation::update();
}
