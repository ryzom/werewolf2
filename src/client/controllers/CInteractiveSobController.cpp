/**
 * \file CInteractiveSobController.cpp
 * \date February 2006
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

//
// System Includes
//
#include "stdpch.h"

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "controllers/CInteractiveSobController.h"
#include "tasks/CInputTask.h"
#include "tasks/C3DTask.h"
#include "tasks/CCameraTask.h"
#include "tasks/CConfigTask.h"

#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CSobStateRequestEvent.h>
#include <wwcommon/ISimulationObj.h>

//
// Class
//

//
// Namespaces
//

namespace WWCLIENT {

const char* CInteractiveSobController::m_Name = "interactive";

CInteractiveSobController::CInteractiveSobController(CActorProxy* sob) {
	m_Subject = sob;
	m_fwd_back = m_strafe = m_orient = 0;
}

CInteractiveSobController::~CInteractiveSobController() {
	;
}

void CInteractiveSobController::update() {
	if(m_Subject==NULL) {
		nlinfo("CInteractiveSobController::update called with a NULL entity.");
		return;
	}

	// movement
	int fwd_back = (int)CInputTask::instance().kbDown(NLMISC::KeyW)-(int)CInputTask::instance().kbDown(NLMISC::KeyS);;
	int orient = (int)CInputTask::instance().kbDown(NLMISC::KeyA)-(int)CInputTask::instance().kbDown(NLMISC::KeyD);
	int strafe = (int)CInputTask::instance().kbDown(NLMISC::KeyE)-(int)CInputTask::instance().kbDown(NLMISC::KeyQ);;

	doMove(fwd_back);
	doOrient(orient);
	doStrafe(strafe);
}

const char* CInteractiveSobController::getControllerName() {
	return m_Name;
}

void CInteractiveSobController::doMove(int fwd_back){
	if(m_fwd_back == fwd_back)
		return;
	WWCOMMON::CSobStateRequestEvent* event = new WWCOMMON::CSobStateRequestEvent();
	event->TargetID = m_Subject->getSobId();
	event->SourceID = m_Subject->getSobId();
	switch(fwd_back) {
		case -1:
			event->TargetState = WWCOMMON::CStateFactory::ST_MV_BACKWARD;
			break;
		case 0:
			event->TargetState = WWCOMMON::CStateFactory::ST_MV_IDLE;
			break;
		case 1:
			event->TargetState = WWCOMMON::CStateFactory::ST_MV_FORWARD;
			break;
		default:
			// something fishy
			nlinfo("Something fishy - forward/backward state value: %i", fwd_back);
			return;
	}
	m_fwd_back = fwd_back;
	WWCOMMON::CGameEventServer::instance().postEvent(event);
}

void CInteractiveSobController::doOrient(int orient){
	if(m_orient == orient)
		return;
	WWCOMMON::CSobStateRequestEvent* event = new WWCOMMON::CSobStateRequestEvent();
	event->TargetID = m_Subject->getSobId();
	event->SourceID = m_Subject->getSobId();
	switch(orient) {
		case -1:
			event->TargetState = WWCOMMON::CStateFactory::ST_OR_TURNRIGHT;
			break;
		case 0:
			event->TargetState = WWCOMMON::CStateFactory::ST_OR_IDLE;
			break;
		case 1:
			event->TargetState = WWCOMMON::CStateFactory::ST_OR_TURNLEFT;
			break;
		default:
			// something fishy
			nlinfo("Something fishy - orient state value: %i", orient);
			return;
	}
	m_orient = orient;
	WWCOMMON::CGameEventServer::instance().postEvent(event);
}

void CInteractiveSobController::doStrafe(int strafe) {
	if(m_strafe == strafe)
		return;
	WWCOMMON::CSobStateRequestEvent* event = new WWCOMMON::CSobStateRequestEvent();
	event->TargetID = m_Subject->getSobId();
	event->SourceID = m_Subject->getSobId();
	switch(strafe) {
		case -1:
			event->TargetState = WWCOMMON::CStateFactory::ST_ST_LEFT;
			break;
		case 0:
			event->TargetState = WWCOMMON::CStateFactory::ST_ST_IDLE;
			break;
		case 1:
			event->TargetState = WWCOMMON::CStateFactory::ST_ST_RIGHT;
			break;
		default:
			// something fishy
			nlinfo("Something fishy - strafe state value: %i", strafe);
			return;
	}
	m_strafe = strafe;
	WWCOMMON::CGameEventServer::instance().postEvent(event);
}

}; // END NAMESPACE WWCLIENT
