/**
 * \file CServerSimulation.h
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

#ifndef __CCLIENTSIMULATION_H__
#define __CCLIENTSIMULATION_H__

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

//
// Werewolf Includes
//
#include "wwcommon/ISimulationObj.h"
#include "wwcommon/CPerformer.h"
#include "wwcommon/CSobManager.h"
#include "wwcommon/IBaseSimulation.h"
#include <wwcommon/CSobSpawnEvent.h>
#include <wwcommon/CSobAddEvent.h>
#include "CActorProxy.h"

//
// Namespaces
//

namespace WWCLIENT {

class CSimulationImpl : public WWCOMMON::IBaseSimulation {
public:
	NLMISC_DECLARE_CLASS(CSimulationImpl);

	/// Attaches a user (uid) to a sob.
	virtual bool attachUser(uint32 uid, uint32 sobid);

	/// Detaches a user (uid) from a sob.
	virtual void detachUser(uint32 uid, uint32 sobid);

	/// Initializes the simulation.
	void init();

	/// Process tasks and updates.
	void update();

	/// Spawn the owner on this simulation.
	void spawnSelf(WWCOMMON::CSobSpawnEvent *event);
	void addSob(WWCOMMON::CSobAddEvent *event);

	void removeSob(uint32 id);

	CActorProxy *getSelfSob() { return m_SelfSob; }

	/*** Time Sync Methods. ***/

	/// Receive a ping response from the server.
	void recvPing(double serverTimeStamp, double localTimeStamp);
	/// Send a ping request to the server.
	void sendPing();
	/// Checks to see if we need to update the ping stats.
	void updatePing();
	 
protected:
	/// The sob this client has primary control over.
	CActorProxy *m_SelfSob;

	double m_NextPingTime;
	/// the time it takes for a packet to travel between client and server
	double m_PingLatency;
	/// the local timestamp we are expecting a pong with
	double m_PingLocalTimeStamp;
	/// the number of pings received
	unsigned long m_PingCount;
	/// a very small float (used to stop calculating offset)
	double m_Epsilon;
	/// tells the client if it should stop pinging
	bool m_NoPing;
};

CSimulationImpl *getClientSimulation();

}; // END NAMESPACE WWCLIENT

#endif // __CCLIENTSIMULATION_H__
