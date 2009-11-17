/**
 * \file CSimulationImpl.h 
 * \date November 2009
 * \author Matt Raykowski
 */

/* Copyright, 2009 Werewolf
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

#ifndef __CSIMULATIONIMPL_H__
#define __CSIMULATIONIMPL_H__

//
// Standard Includes
//
#include <string>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>

//
// Werewolf Includes
//	
#include "wwcommon/CPerformer.h"
#include "wwcommon/ISimulationObj.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/CGameEventServer.h"
#include "wwcommon/CSobSpawnEvent.h"

//
// Namespaces
//

class CSimulationImpl;

class CActor : public WWCOMMON::CPerformer {
	OF_SETUP_REGISTRAR(WWCOMMON::CSobFactory, WWCOMMON::ISimulationObj, CActor, std::string);
public:
	static WWCOMMON::ISimulationObj *Create() { return new CActor(); }

	uint32 getPlayerId() { return m_PlrId; }

	void update() { WWCOMMON::CPerformer::update(); }

protected:
	friend class CSimulationImpl;

	uint32 m_PlrId;
	CActor() : WWCOMMON::CPerformer(),m_PlrId(0) { }
};

// Make a fake simulation implementation.
class CSimulationImpl : public WWCOMMON::IBaseSimulation {
public:
	NLMISC_DECLARE_CLASS(CSimulationImpl);

	static void BindScriptSimulationImpl() { }

	bool attachUser(uint32 uid, uint32 sobid) { 
		CActor *sob=dynamic_cast<CActor *>(m_ActorManager->find(sobid));
		if(sob==NULL) {
			// the sobid wasn't in our database.
			nlwarning("User tried to attach to a sob that doesn't exist or is not an actor: %d", sobid);
			return false;
		} else {
			// we will set up a control state here later. but for now just associate the uid.
			sob->m_PlrId=uid;
			return true;
		}
	};

	void detachUser(uint32 uid, uint32 sobid) { };
	void init() {
		std::string retBankName, globRetBank;
		retBankName="snowballs.rbank";
		globRetBank="snowballs.gr";
		// pacs stuff.
		m_RetrieverBank = NLPACS::URetrieverBank::createRetrieverBank(retBankName.c_str());
		m_GlobalRetriever = NLPACS::UGlobalRetriever::createGlobalRetriever(globRetBank.c_str(), m_RetrieverBank);

		nlinfo("Create Move Container.");
		m_MoveContainer = NLPACS::UMoveContainer::createMoveContainer(m_GlobalRetriever, 100, 100, 6.0);
		WWCOMMON::IBaseSimulation::init();
		WWCOMMON::CGameEventServer::instance().setDeltaMultiplier(0.0f);

		// This should be something else probably. TODO
		m_RootSob=getNewSob("sobActor", 0);
	}

	void update() {
		// Process the basic functions.
		WWCOMMON::IBaseSimulation::update();
	}

	WWCOMMON::ISimulationObj *initSob(std::string name) {
		CActor *sob=dynamic_cast<CActor *>(getNewSob("sobActor"));
		sob->setOwnerSobId(m_RootSob->getSobId());

		// put it in our manager
		m_ActorManager->addSob(sob);

		return sob;
	}

	bool userLogin(uint32 uid, uint32 objid) {
		// create the sob and add it to the manager.
		CActor *sob=dynamic_cast<CActor *>(initSob("sobActor"));

		// now attach the sob.
		if(!attachUser(uid, sob->getSobId())) {
			nlwarning("Attaching user (%d) to sob (%d) failed!", uid, sob->getSobId());
			return false;
		}

		// save the sob's position.
		//sob->setPosition(NLMISC::CVector(1840.0, -970.0, 0.0));
		//sob->setGlobalPosition(NLMISC::CVector(1840.0, -970.0, 0.0));

		// TODO henri:everyone make this data based.
		//sob->addHandler(WWCOMMON::CSobHandlerFactory::instance().getHandler(WWCOMMON::CSobHandlerFactory::H_MOTION));
		//sob->addHandler(WWCOMMON::CSobHandlerFactory::instance().getHandler(WWCOMMON::CSobHandlerFactory::H_MOTION_REQUEST));
		
		// Create and set up sob event handler, we'll listen for spawn events.
		const WWSCRIPT::Script *exampleScr = WWSCRIPT::ScriptManager::getInstance().getScript("ExampleScript");
		if(!exampleScr) {
			nlerror("Failed to retrieve: ExampleScript");
			return false;
		}
		const WWSCRIPT::ScriptFunction *sobFunc = exampleScr->getFunction("handleSobEvent");
		CScriptedSobEventHandler *handler = new CScriptedSobEventHandler(sobFunc);
		handler->addHandledEvent(EVENT_ID(CSobSpawnEvent));
		sob->addHandler(handler);

		//sob->addController(new CPerformerDRController(sob, 10.0));
		//sob->addController(new WWCOMMON::CMotionController(sob));
		//sob->addController(new CProximityController(sob, 60.0));

		//sob->addView(new CServerObjectView(sob));

		WWCOMMON::CSobSpawnEvent* event = new WWCOMMON::CSobSpawnEvent();
		event->SourceID = sob->getSobId();
		event->TargetID = sob->getSobId();
		event->Position = sob->getPosition();
		event->EMD = "sphere guy";

		WWCOMMON::CGameEventServer::instance().postEvent(event);

		return true;
	}

};

#endif __CSIMULATIONIMPL_H__