/**
 * \file CPerformerDRController.h
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

#ifndef __CPerformerDRController_h__
#define __CPerformerDRController_h__

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
#include <wwcommon/CSobDREvent.h>
#include <wwcommon/ISimulationObj.h>

//
// Namespaces
//

class CPerformerDRController : public WWCOMMON::ISobController, WWCOMMON::IGameEventListener {
public:
	CPerformerDRController(WWCOMMON::CPerformer* subject, double timeout);
	~CPerformerDRController();

	void update();
	const char* getControllerName();

	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event);

private:
	void fillDREvent(WWCOMMON::CSobDREvent* event);
	bool checkIdle();
	void sendDREvent();

	WWCOMMON::CPerformer* m_Subject;
	static const char* m_Name;
	double m_Timeout;
	double m_NextUpdateTime;
	bool m_ForceUpdate;
};

#endif // __CPerformerDRController_h__