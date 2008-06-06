/**
 * \file CGAMEEVENTMANAGER.H
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

#ifndef __CGAMEEVENTMANAGER_H__
#define __CGAMEEVENTMANAGER_H__

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
#include <wwcommon/CGameSpawnRequestEvent.h>
#include <wwcommon/CGameUnspawnRequestEvent.h>
#include <wwcommon/CSobUnspawnEvent.h>

//
// Namespaces
//

class CGameEventManager : public WWCOMMON::ISingleton<CGameEventManager>, WWCOMMON::IGameEventListener {
public:
	CGameEventManager();
	~CGameEventManager();

	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event);

	void spawn(WWCOMMON::CGameSpawnRequestEvent *event);
	void postUnspawn(WWCOMMON::CGameUnspawnRequestEvent *event);
	void unspawn(WWCOMMON::CSobUnspawnEvent *event);
};

#endif // __CGAMEEVENTMANAGER_H__
