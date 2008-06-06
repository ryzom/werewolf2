/**
 * \file CServerSimulation.cpp
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

//
// Werewolf Includes
//
#include "wwcommon/ISimulationObj.h"
#include "wwcommon/CPerformer.h"
#include "CActorProxy.h"

#include <wwcommon/CSobManager.h>
#include <wwcommon/CSobFactory.h>
#include <wwcommon/IBaseSimulation.h>
#include <wwcommon/CGameEventServer.h>
#include "CClientSimulation.h"
#include "controllers/CInteractiveSobController.h"

//#include "CCharacterManager.h"
#include "tasks/CNetworkTask.h"
#include "tasks/CConfigTask.h"
#include "tasks/CResourceTask.h"
#include "views/CPerformer3DView.h"

#include <wwcommon/CSobHandlerFactory.h>
#include <wwcommon/CMotionController.h>
#include "tasks/CPacsTask.h"

//
// Namespaces
//

namespace WWCLIENT {

WWCOMMON::IBaseSimulation *Simulation;
WWCOMMON::IBaseSimulation *getSimulation() {
	if(Simulation==NULL) {
		Simulation=static_cast<WWCOMMON::IBaseSimulation *>(new CClientSimulation());
	}
	return static_cast<CClientSimulation *>(Simulation);
}
CClientSimulation *getClientSimulation() {
	if(Simulation==NULL) {
		Simulation=static_cast<WWCOMMON::IBaseSimulation *>(new CClientSimulation());
	}
	return static_cast<CClientSimulation *>(Simulation);
}

bool CClientSimulation::attachUser(uint32 uid, uint32 sobid) {
	// There is no logic in this method on the client.
	return true;
}

void CClientSimulation::detachUser(uint32 uid, uint32 sobid) {
	// There is no logic in this method on the client.
}

	
void CClientSimulation::init() {
	// get the paths to the retrievers.
	std::string retBankName, globRetBank;
	retBankName=CConfigTask::instance().configFile().getVar("RetrieverBankName").asString();
	globRetBank=CConfigTask::instance().configFile().getVar("GlobalRetrieverName").asString();

	nlinfo("Create Retriever Bank: %s", NLMISC::CPath::lookup(retBankName).c_str());
	nlinfo("Create Global Retriever: %s", NLMISC::CPath::lookup(globRetBank).c_str());

	// pacs stuff.
	m_RetrieverBank = NLPACS::URetrieverBank::createRetrieverBank(retBankName.c_str());
	if(m_RetrieverBank == NULL)
		nlinfo("what the fuck bitch.");
	m_GlobalRetriever = NLPACS::UGlobalRetriever::createGlobalRetriever(globRetBank.c_str(), m_RetrieverBank);
	
	nlinfo("Create Move Container.");
	m_MoveContainer = NLPACS::UMoveContainer::createMoveContainer(m_GlobalRetriever, 100, 100, 6.0);

	// null out the self-sob to mark a clear simulation
	m_SelfSob=NULL;

	WWCOMMON::IBaseSimulation::init();

	// Client lags behind by 100ms
	m_SimulationDelay=0.1;
	m_PingLatency=0;
	m_PingCount=0;
	m_PingLocalTimeStamp=0;
	// nanosecond accuracy :)
	m_Epsilon=0.000001;
	m_NoPing=false;
	m_NextPingTime=0;

	WWCOMMON::CGameEventServer::instance().setDeltaMultiplier(0.0f);
}

void CClientSimulation::update() {
	// Check if we need to ping something.
	updatePing();

	// Process the basic functions.
	WWCOMMON::IBaseSimulation::update();
}

void CClientSimulation::spawnSelf(WWCOMMON::CSobSpawnEvent *event) {
	// Create a new SOB and set its unique ID to that from the server.
	CActorProxy *sob=dynamic_cast<CActorProxy *>(getNewSob("sobActor", event->SourceID));

	// And add it to the manager
	m_ActorManager->addSob(sob);
	// Record it as the local sob.
	m_SelfSob=sob;

	// Make sure out starting position is valid.
	if(event->Position == NLMISC::CVector::Null) {
		nlwarning("Entity spawned at an invalid position: %d", sob->getSobId());
		return;
	}

	sob->setPosition(event->Position);
	sob->setGlobalPosition(event->Position);
	WWCOMMON::IView* view = new CPerformer3DView(sob, CResourceTask::instance().getEMD(event->EMD));
	view->show();
	sob->addView(view);

	sob->addController(new CInteractiveSobController(sob));
	// handlers are global and should come from the factory
	sob->addHandler(WWCOMMON::CSobHandlerFactory::instance().getHandler(WWCOMMON::CSobHandlerFactory::H_CLIENT_MOTION));
//	sob->addHandler(WWCOMMON::CSobHandlerFactory::instance().getHandler(WWCOMMON::CSobHandlerFactory::H_MOTION));
//	sob->addHandler(WWCOMMON::CSobHandlerFactory::instance().getHandler(WWCOMMON::CSobHandlerFactory::H_MOTION_REQUEST));
}

void CClientSimulation::addSob(WWCOMMON::CSobAddEvent *event) {
	// create a new SOB and set its unique ID to that from the server.
	CActorProxy *sob=dynamic_cast<CActorProxy *>(getNewSob("sobActor", event->SourceID));

	// and add it to the manager
	m_ActorManager->addSob(sob);

	// make sure out starting position is valid.
	if(event->Position == NLMISC::CVector::Null) {
		nlwarning("Entity spawned at an invalid position: %d", sob->getSobId());
		return;
	}

	sob->setPosition(event->Position);
	sob->getCurrentKey()->setSpeed(event->Speed);
	sob->getCurrentKey()->setYaw(event->Yaw);
	sob->getCurrentKey()->setYawSpeed(event->YawVelocity);
	sob->setGlobalPosition(event->Position);
	sob->setActiveStateList(&event->ActiveStateList);

	WWCOMMON::IView* view = new CPerformer3DView(sob, CResourceTask::instance().getEMD(event->EMD));
	view->show();
	sob->addView(view);

	sob->addHandler(WWCOMMON::CSobHandlerFactory::instance().getHandler(WWCOMMON::CSobHandlerFactory::H_CLIENT_MOTION));

	WWCOMMON::CMotionController* controller = new WWCOMMON::CMotionController(sob);
	controller->setVisualCollisionEntity(&(CPacsTask::instance().getVisualCollisionManager()));
	sob->addController(controller);
}

void CClientSimulation::removeSob(uint32 id) {
	WWCOMMON::ISimulationObj *sob=m_ActorManager->find(id);
	if(sob==NULL) {
		nlwarning("Failed to locate sob %d to unspawn.", id);
		return;
	}

	nlinfo("Removing sob %d from simulation.", id);
	m_ActorManager->removeSob(sob);

	delete sob;
}

void CClientSimulation::recvPing(double serverTimeStamp, double localTimeStamp) {
	// make sure there's a ping in progress.
	if(m_PingLocalTimeStamp==0) {
		nlwarning("Received a ping response with no ping pending!");
		return;
	}

	// and make sure it's our ping.
	if(m_PingLocalTimeStamp != localTimeStamp) {
		nlwarning("Received a ping response for a ping that's not current.");
		return;
	}

	// Try something a little..simpler..and without STL (faster)
	double localRecvTimeStamp = time();
	double currentLatency = localRecvTimeStamp - localTimeStamp;

	// Let's do some weighted averages

   	++m_PingCount;
	// Since the ping changes all the time let the last sample have the most weight (similar to taking just 5 samples)
	unsigned long pingWeight = std::min<unsigned long>(5, m_PingCount-1);
	m_PingLatency = (pingWeight*m_PingLatency + currentLatency)/(pingWeight+1);

	// The offset should not change - hence the samples have less and less effect
	// until epsilon is reached and further calculations become pointless
	double currentOffset = serverTimeStamp - localRecvTimeStamp + m_PingLatency;
	currentOffset /= m_PingCount;
	if(currentOffset > m_Epsilon) {
		unsigned long offsetWeight = m_PingCount-1;
		// Improve our accuracy by taking the last 5 samples only during the first 50 pings 
		if(m_PingCount < 50) {
			offsetWeight = std::min<unsigned long>(5, offsetWeight);
			currentOffset = serverTimeStamp - localRecvTimeStamp + m_PingLatency;
			currentOffset /= (offsetWeight+1);
		}
		m_OffsetTime = (m_OffsetTime*offsetWeight)/(offsetWeight+1) + currentOffset;
	} else {
		// no need for pings anymore since we already have what we believe to be the correct offset
		// and nothing will change that.
		// The difference in server and client time should always stay the same based upon the assumption
		// that the clocks of the respective machines run at the same speed.
		m_NoPing = true;
		nlinfo("Stopped pinging.");
	}

	nlinfo("I think the server time is: %1f", time() + m_OffsetTime);
}

void CClientSimulation::sendPing() {
	// we can't ping if we're not online.
	if(!CNetworkTask::instance().connected())
		return;

	m_PingLocalTimeStamp = time();
	// schedule the next ping
	// if we are just starting out do a few pings real fast
	// to get it in the ball park...takes about 10 seconds
	if(m_PingCount < 25) {
		m_NextPingTime = time() + 0.4;
	} else {
		m_NextPingTime=time()+ 5.00;
	}

	// send the message to the server.
	NLNET::CMessage msgout("PINGREQ");
	msgout.serial(m_PingLocalTimeStamp);
	CNetworkTask::instance().send(msgout);
}

void CClientSimulation::updatePing() {
	// ping not wanted.
	if(m_NoPing)
		return;

	// we can't process out of sequence events until the simulation has been started.
	if(m_SelfSob == NULL)
		return;

	// check for next available time slot.
	if(m_NextPingTime <= time())
		sendPing();
}

}; // END NAMESPACE WWCLIENT
