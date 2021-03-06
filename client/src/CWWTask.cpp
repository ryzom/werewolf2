/**
 * \file CWWTask.cpp
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

//
// NeL Includes
//
#include <nel/misc/path.h>
#include <nel/misc/debug.h>
#include <nel/misc/report.h>
#include <nel/misc/config_file.h>

#include <nel/3d/u_scene.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_texture.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_text_context.h>

#include <nel/net/email.h>

//
// Werewolf Includes
//
#include "tasks/C3DTask.h"
#include "tasks/C3DSwapTask.h"
#include "tasks/CConfigTask.h"
#include "tasks/CTimeTask.h"
#include "tasks/CResourceTask.h"
#include "tasks/CBackgroundTask.h"
#include "tasks/CIntroTask.h"
#include "tasks/CGuiTask.h"
#include "tasks/CAnimateTask.h"
//#include "tasks/CEntityTask.h"
#include "tasks/CInputTask.h"
#include "tasks/CNetworkTask.h"
#include "tasks/CPreGameTask.h"

#include "wwcommon/CTaskManager.h"
//#include "CCamera.h"
#include "controllers/CInputController.h"
#include "CWWTask.h"

#include "CSimulationImpl.h"
#include "CClientEventManager.h"
#include "CUserPaths.h"

//
// Namespaces
//

using namespace NL3D;
using namespace NLNET;
using namespace NLMISC;

namespace WWCLIENT {

void CWWTask::init() {
	m_DoError=false;

	// First we want to register some essential paths:
	std::string dataDir = DATA_DIR "data";
	NLMISC::CPath::addSearchPath(dataDir,true,false,NULL);
	NLMISC::CPath::addSearchPath(CUserPaths::getWerewolfConfigDir(),true,true,NULL);


	// Register game and simulation object events.
	WWCOMMON::registerEvents();
	WWCOMMON::registerCommonFactoryObjects();
	OF_REGISTER(WWCOMMON::CSobFactory, CActorProxy, "sobActor");

	// add default tasks...
	WWCOMMON::CTaskManager::instance().add(CConfigTask::instance(), 10);
	WWCOMMON::CTaskManager::instance().add(CTimeTask::instance(), 20);
	WWCOMMON::CTaskManager::instance().add(CResourceTask::instance(), 30);
	try
	{
		NLMISC_REGISTER_CLASS(CSimulationImpl);
	}
	catch(ERegisteredClass &e)
	{
		nlinfo("CSimulationImpl is already registered.");
	}
	getClientSimulation()->init();

	WWCOMMON::CTaskManager::instance().add(C3DTask::instance(), 110);
	WWCOMMON::CTaskManager::instance().add(CInputTask::instance(), 115);
	WWCOMMON::CTaskManager::instance().add(CAnimateTask::instance(), 120);
	WWCOMMON::CTaskManager::instance().add(CGuiTask::instance(), 130);
	WWCOMMON::CTaskManager::instance().add(C3DSwapTask::instance(), 10000);


	// Create a new input controller.
	m_Controller = new CInputController;

	CClientEventManager::instance();

	if(CNetworkTask::instance().getShardIp().isValid() &&
		CNetworkTask::instance().getLoginCookie().isValid()) {
		// If the network task has a valid IP and cookie then we should just skip
		// to the in-game play.
		WWCOMMON::CTaskManager::instance().add(CNetworkTask::instance(), 40);

		std::string conResult = CNetworkTask::instance().connect();
		if(conResult.empty()) {
			// We successfully connected to the server.
			// We'll fire up the Pre Game Task which will let you choose
			// a character and so on...
			WWCOMMON::CTaskManager::instance().add(CPreGameTask::instance(), 60);
		} else {
			// We failed to connect. Either the FSAddr/Cookie was invalid or
			// the frontend service is down and the LS doesn't know about it
			// yet. So we'll just fire up the main menu and let you play offline.
			WWCOMMON::CTaskManager::instance().add(CIntroTask::instance(), 1000);
		}
	} else {
		// Start up the main menu for offline play.
		WWCOMMON::CTaskManager::instance().add(CIntroTask::instance(), 1000);
	}

	reset();
}

void CWWTask::reset() {
	// Reset the input controller.
	controller().reset();

	// I don't have a clear list of game states yet, so we'll stick with this.
	State = CWWTask::eBeforeFirstSession;
}

void CWWTask::update()
{
	if(m_DoError)
		m_error();

	// let the input controller update.
	controller().update();

	getClientSimulation()->updateTime();
	getClientSimulation()->update();
}

void CWWTask::error(std::string reason){
	m_DoError = true;
	m_ErrorReason = reason;
}

void CWWTask::m_error() {
	reset();
	m_DoError = false;
}

std::string CWWTask::name() {
	return "CWWTask";
}

CInputController &CWWTask::controller() const {
	nlassert(m_Controller);
	return *m_Controller;
}

}; // END NAMESPACE WWCLIENT
