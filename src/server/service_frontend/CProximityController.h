/**
 * \file CProximityController.h
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

#ifndef __CPROXIMITYCONTROLLER_H__
#define __CPROXIMITYCONTROLLER_H__

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
#include <wwcommon/ISobController.h>
#include <wwcommon/IGameEventListener.h>
#include <wwcommon/CPerformer.h>
#include <wwcommon/IGameEvent.h>
#include "CProximityManager.h"

//
// Namespaces
//

class CProximityController : public WWCOMMON::ISobController, public WWCOMMON::IGameEventListener {
public:
	CProximityController(WWCOMMON::CPerformer* subject, double idlePeriod);
	virtual ~CProximityController();

	const char* getControllerName();
	void update();

	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event);

	CProximityManager::sobGrid::Position getPosition();
	void setPosition(CProximityManager::sobGrid::Position pos);

private:
	void registerForUpdate();
	void updateIdle();

	CProximityManager::sobGrid::Position m_Position;

	WWCOMMON::CPerformer* m_Subject;
	static const char* m_Name;
	double m_NextUpdateTime;
	bool m_Idle;
	double m_IdlePeriod;
};

#endif // __CPROXIMITYCONTROLLER_H__
