/**
 * \file CPlayerController.cpp
 * \date July 2004
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
// System Includes
//
#include "stdpch.h"
#include "CEGUI.h"

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "controllers/CPlayerController.h"
#include "tasks/CInputTask.h"
//#include "tasks/CEntityTask.h"
#include "tasks/C3DTask.h"
#include "tasks/CCameraTask.h"
#include "tasks/CConfigTask.h"

#include "interfaces/IBaseCamera.h"

#include "wwcommon/ISimulationObj.h"
#include "CClientSimulation.h"

//
// Class
//

//
// Namespaces
//

namespace WWCLIENT {

CPlayerController::CPlayerController() {
	C3DTask::instance().driver().EventServer.addListener(NLMISC::EventMouseMoveId, this);
	C3DTask::instance().driver().EventServer.addListener(NLMISC::EventMouseDownId, this);
	C3DTask::instance().driver().EventServer.addListener(NLMISC::EventMouseUpId, this);
	C3DTask::instance().driver().EventServer.addListener(NLMISC::EventMouseWheelId, this);
	C3DTask::instance().driver().AsyncListener.addToServer(C3DTask::instance().driver().EventServer);
	reset();

	//m_SelfProxy=NULL;
}

CPlayerController::~CPlayerController() {
	C3DTask::instance().driver().EventServer.removeListener(NLMISC::EventMouseMoveId, this);
	C3DTask::instance().driver().EventServer.removeListener(NLMISC::EventMouseDownId, this);
	C3DTask::instance().driver().EventServer.removeListener(NLMISC::EventMouseUpId, this);
	C3DTask::instance().driver().EventServer.removeListener(NLMISC::EventMouseWheelId, this);
	C3DTask::instance().driver().AsyncListener.removeFromServer(C3DTask::instance().driver().EventServer);
}

void CPlayerController::update() {

	if(CInputTask::instance().kbPressed(NLMISC::KeyF3)) {
		NL3D::UDriver::TPolygonMode p = C3DTask::instance().driver().getPolygonMode();
		p = NL3D::UDriver::TPolygonMode(((int)p+1)%3);
		C3DTask::instance().driver().setPolygonMode(p);
	}

	if(CInputTask::instance().kbPressed(NLMISC::KeyLBRACKET)) {
		float val=CEGUI::WindowManager::getSingleton().getWindow("werewolf")->getAlpha();
		val-=0.1f;
		if(val>=0.0f)
			CEGUI::WindowManager::getSingleton().getWindow("werewolf")->setAlpha(val);
	}
	if(CInputTask::instance().kbPressed(NLMISC::KeyRBRACKET)) {
		float val=CEGUI::WindowManager::getSingleton().getWindow("werewolf")->getAlpha();
		val+=0.1f;
		if(val<=1.0f)
			CEGUI::WindowManager::getSingleton().getWindow("werewolf")->setAlpha(val);
	}
}

void CPlayerController::reset() {
	m_FirstFrame = true;
	m_VirtualFrameTime = 0.0f;
	m_MissedTime = 0.0f;
}	

void CPlayerController::operator()(const NLMISC::CEvent& event) {
	if (event==NLMISC::EventMouseWheelId) {
		// don't know if this is the right place for it...probably not
		NLMISC::CEventMouseWheel* mouseEvent=(NLMISC::CEventMouseWheel*)&event;

		IBaseCamera *cam = &CCameraTask::instance().getActiveCamera();
		float change = mouseEvent->Direction? -0.5f : 0.5f;
		cam->setViewingDistance(cam->getViewingDistance() + change);
	}
}

const NLMISC::CMatrix &CPlayerController::getViewMatrix() {
	return m_Matrix;
}

}; // END NAMESPACE WWCLIENT
