/**
 * \file IBaseSimulation.h
 * \date April 2005
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

#ifndef __IBASESIMULATION_H__
#define __IBASESIMULATION_H__

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
#include <nel/pacs/u_retriever_bank.h>
#include <nel/pacs/u_global_retriever.h>
#include <nel/pacs/u_move_container.h>
#include <nel/misc/value_smoother.h>

//
// Werewolf Includes
//
#include "CSobManager.h"
#include "CSobFactory.h"
#include "ISingleton.h"
#include "CGameEventServer.h"

//
// Namespaces
//

namespace WWCOMMON {

class IBaseSimulation {
public:
	/// Default virtual dtor.
	virtual ~IBaseSimulation() {}

	/// Updates the internal time system.
	virtual void updateTime() {
		// get the new time.
		double newTime = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());
		
		// save the old time.
		m_OldTime = m_Time;
		
		// get the new adjusted time.
		m_Time = newTime - m_FirstTime;

		// get the time between frames.
		m_DeltaTime = m_Time - m_OldTime;

		m_DeltaSmoother.addValue(m_DeltaTime);
	}

	/**
	 * \brief The current time, in seconds.
	 *
	 * \return double containing the current tome in seconds.
	 */
	double time() const { return m_Time; }

	/**
	 * \brief The current adjusted time, in seconds.
	 *
	 * \return double containing the current time adjusted for network latency in seconds.
	 */
	double adjustedTime() const { return m_Time + m_OffsetTime - m_SimulationDelay; }

	/**
	 * \brief The time between this frame and the last frame.
	 *
	 * \return double containing the time difference in seconds.
	 */
	double deltaTime () const { return m_DeltaTime; }

	double smoothDeltaTime() const { return m_DeltaSmoother.getSmoothValue(); }

	/**
	 * \brief The amount of time to backplay the simulation.
	 *
	 * \return double containing the amount of time to run the simulation in the past.
	 */
	double adjustSimulationTime() const { return m_Time - m_NetworkTime; }

	const CSobManager *getActorManager() {
		return m_ActorManager;
	}

	const CSobManager *getAreaManager() {
		return m_AreaManager;
	}

	virtual bool attachUser(uint32 uid, uint32 sobid)=0;
	virtual void detachUser(uint32 uid, uint32 sobid)=0;

	void init() {
		nlinfo("IBase Init.");
		// init time system.
		m_NetworkTime = m_FirstTime = m_OldTime = m_Time = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());
		m_OffsetTime = 0;
		m_DeltaSmoother.init(50);

		// init basic sob managers.
		m_ActorManager=new CSobManager();
		m_AreaManager=new CSobManager();
	}

	void release() {
		// sob managers
		delete m_ActorManager;
		delete m_AreaManager;

		nlinfo("Free Collision Data.");

		// free pacs stuff
		NLPACS::UGlobalRetriever::deleteGlobalRetriever(m_GlobalRetriever);
		NLPACS::URetrieverBank::deleteRetrieverBank(m_RetrieverBank);
		NLPACS::UMoveContainer::deleteMoveContainer(m_MoveContainer);
	}

	virtual void update() {
		CGameEventServer::instance().processEventQueue();
		// eval pacs stuff.
		CSobManager::TSobMap::iterator itr=m_ActorManager->m_SobMap.begin();
		while(itr!=m_ActorManager->m_SobMap.end()) {
			(*itr).second->update();
			itr++;
		}
		m_MoveContainer->evalCollision(deltaTime(),0);
	}

	// pacs stuff

	NLPACS::UMoveContainer			&getMoveContainer() { return *m_MoveContainer; }
	//NLPACS::UVisualCollisionManager	&getVisualCollisionManager()
	NLPACS::UGlobalRetriever		&getGlobalRetriever()  { return *m_GlobalRetriever; }


protected:
	/// Creates a new sob, since IBaseSimulation is the only class allowed to do this.
	ISimulationObj *getNewSob(std::string name) {
		return CSobFactory::instance().createSob(name);
	}

	ISimulationObj* getNewSob(std::string name, uint32 id) {
		return CSobFactory::instance().createSob(name, id);
	}

	// pacs related stuff.
	NLPACS::URetrieverBank				*m_RetrieverBank;
	NLPACS::UGlobalRetriever			*m_GlobalRetriever;
	NLPACS::UMoveContainer				*m_MoveContainer;

	// time related stuff.
	double m_Time, m_DeltaTime, m_OldTime;
	double m_FirstTime;
	double m_NetworkTime;
	double m_SimulationDelay;
	double m_OffsetTime;
	NLMISC::CValueSmootherTemplate<double> m_DeltaSmoother;

	// sob managers
	CSobManager *m_ActorManager;
	CSobManager *m_AreaManager;

	// this guy owns everything.
	ISimulationObj *m_RootSob;
};

}; // END OF NAMESPACE WWCOMMON

WWCOMMON::IBaseSimulation *getSimulation();

#endif // __IBASESIMULATION_H__
