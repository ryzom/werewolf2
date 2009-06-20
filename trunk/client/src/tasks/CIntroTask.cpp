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
#include "globals.h"
#include "CEGUI.h"

//
// NeL Includes
//
#include <nel/net/callback_client.h>
#include <nel/misc/smart_ptr.h>

//
// Werewolf Includes
//
#include "CIntroTask.h"
#include "tasks/CBackgroundTask.h"
#include "tasks/CConfigTask.h"
#include "tasks/C3DTask.h"
#include "tasks/CGameTask.h"
#include "tasks/CPreGameTask.h"
#include "tasks/CInputTask.h"
#include "tasks/CNetworkTask.h"
#include "wwcommon/CTaskManager.h"

//
// Namespaces
//
using namespace std;
using namespace NL3D;
using namespace NLMISC;
using namespace NLNET;

namespace WWCLIENT {

/*****
 * Callbacks and Subscribers for GUI events.
 *****/

/**
 * \brief Handles clicks on the 'quit' button.
 */
bool handleQuitBtn(const CEGUI::EventArgs& e) {
	WWCOMMON::CTaskManager::instance().exit();
	return true;
}

bool handlePlayOfflineBtn(const CEGUI::EventArgs& e) {
	CIntroTask::instance().handlePlayOffline();
	return true;
}

// intro task
void CIntroTask::reset() {
	;
}

void CIntroTask::init() {
	WWCOMMON::CTaskManager::instance().add(CBackgroundTask::instance(), 59);
	// not using autologin, at least not right now
	//m_AutoLogin = CConfigTask::instance().configFile().getVar("AutoLogin").asInt();

	initWindows();
	reset();
}

void CIntroTask::initWindows() {
	// show the menu we're interested in.
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->show();


	// set up event subscriptions
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu/Quit")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handleQuitBtn);
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu/PlayOff")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handlePlayOfflineBtn);
}




void CIntroTask::error(string &reason) {
	;
}

void CIntroTask::update() {
	;
}

void CIntroTask::render() {
	;
}

void CIntroTask::release() {
	;
}

/*****
 * Methods used for GUI events.
 *****/
void CIntroTask::handlePlayOffline() {
	// stop the intro and background tasks.
	CIntroTask::instance().stop();
	CBackgroundTask::instance().stop();

	// hide the main menu.
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::FrameWindow *wnd=(CEGUI::FrameWindow *)winMgr.getWindow("IntroTask/MainMenu");
	wnd->hide();

	// add the game task. let the games begin!
	WWCOMMON::CTaskManager::instance().add(CGameTask::instance(), 60);
}

}; // END NAMESPACE WWCLIENT
