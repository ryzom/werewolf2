/**
 * \file CClientEventManager.h
 * \date June 2006
 * \author Henri Kuuste
 */

/* Copyright, 2006 Werewolf
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

#ifndef __CCLIENTEVENTMANAGER_H__
#define __CCLIENTEVENTMANAGER_H__

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
#include <wwcommon/ISingleton.h>
#include <wwcommon/IGameEventListener.h>
#include <wwcommon/IGameEvent.h>
#include <wwcommon/CSobSpawnEvent.h>
#include <wwcommon/CSobAddEvent.h>
#include <wwcommon/CSobRemoveEvent.h>

//
// Namespaces
//

namespace WWCLIENT {

class CClientEventManager : public WWCOMMON::ISingleton<CClientEventManager>, WWCOMMON::IGameEventListener {
public:
	CClientEventManager();
	virtual ~CClientEventManager();

	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event);

	void logOut();
	void sendToServer(WWCOMMON::CGameEventServer::EventPtr &event);

private:

	void spawn(WWCOMMON::CSobSpawnEvent* event);
	void add(WWCOMMON::CSobAddEvent* event);
	void remove(WWCOMMON::CSobRemoveEvent* event);
};

}; // END NAMESPACE WWCLIENT

#endif // __CCLIENTEVENTMANAGER_H__
