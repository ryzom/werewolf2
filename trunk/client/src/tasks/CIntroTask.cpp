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

bool handlePlayOnlineBtn(const CEGUI::EventArgs &e) {
	// show the login menu.
	CIntroTask::instance().handlePlayOnBtn();
	return true;
}

bool handleConnectBtn(const CEGUI::EventArgs& e) {
	CIntroTask::instance().handleConnect();
	return true;
}

bool handleConnectShardBtn(const CEGUI::EventArgs& e) {
	CIntroTask::instance().handleConnectShard();
	return true;
}

bool handleUserAccept(const CEGUI::EventArgs &e) {
	CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS/PassDL")->activate();
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
	//CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->show();

	// fix some stuff that isn't done in the XML
	// set masked text for the password dialog to '*'
	//CEGUI::Editbox *edb=(CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS/PassDL");
	//edb->setTextMasked(true);
	//edb->setMaskCodePoint( (CEGUI::utf32)'*' );

	// set up the columns on the shard list.
	//CEGUI::MultiColumnList *mcl=(CEGUI::MultiColumnList *)CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ShardList/ShardMCL");
	//mcl->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
	//mcl->addColumn("Shard ID",0,CEGUI::UDim(0.33f,0));
	//mcl->addColumn("Shard Name",1,CEGUI::UDim(0.5f,0));
	//mcl->addColumn("Players",2,CEGUI::UDim(0.2f,0));

	// set up event subscriptions
	//CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu/Quit")->
//		subscribeEvent(CEGUI::PushButton::EventClicked, handleQuitBtn);
//	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu/PlayOff")->
//		subscribeEvent(CEGUI::PushButton::EventClicked, handlePlayOfflineBtn);
//	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu/PlayOn")->
//		subscribeEvent(CEGUI::PushButton::EventClicked, handlePlayOnlineBtn);
//	CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS/ConnectBTN")->
//		subscribeEvent(CEGUI::PushButton::EventClicked, handleConnectBtn);
//	CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS/UserDL")->
//		subscribeEvent(CEGUI::Editbox::EventTextAccepted, handleUserAccept);
//	CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ShardList/ConnectBTN")->
//		subscribeEvent(CEGUI::PushButton::EventClicked, handleConnectShardBtn);
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

void CIntroTask::handlePlayOnBtn() {
	// hide the main menu and show the logon menu.
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->hide();
	CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS")->show();
	WWCOMMON::CTaskManager::instance().add(CNetworkTask::instance(), 40);
}

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

void CIntroTask::handleConnect() {
	std::string user, pass, reason;
	bool status=false;
	CEGUI::PushButton *btn=(CEGUI::PushButton *)CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS/UserDL");
	user=btn->getText().c_str();
	btn=(CEGUI::PushButton *)CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS/PassDL");
	pass=btn->getText().c_str();

	// simple user/pass validation.
	if(user.empty()) {
		nlwarning("User tried to login with blank username.");
		return;
	} else if(pass.empty()) {
		nlwarning("User tried to login with blank password.");
		return;
	}

	// try to connect.
	CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS/StatusTXT")->setText("Connecting...");
	reason=CNetworkTask::instance().connectToLoginServer(user,pass);
	if(reason.empty()) {
		nlinfo("Successfully connected.");
	} else {
		// i dunno, quit? probably give an ok box and back to the main menu...
		WWCOMMON::CTaskManager::instance().exit();
	}
	CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS")->hide();
	
	CEGUI::MultiColumnList *mcl=(CEGUI::MultiColumnList *)CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ShardList/ShardMCL");
	std::vector<CNetworkTask::CShard> shardList=CNetworkTask::instance().getShardList();
	for(uint i=0;i<shardList.size();i++) {
		mcl->addRow();
		mcl->setItem(new IntroListItem(NLMISC::toString(shardList[i].ShardId)), 0, i);
		mcl->setItem(new IntroListItem(NLMISC::toString(shardList[i].ShardName)), 1, i);
		mcl->setItem(new IntroListItem(NLMISC::toString(shardList[i].ShardNbPlayers)), 2, i);
	}
	// work-around a small bug (oops!)
	mcl->setSelectionMode(CEGUI::MultiColumnList::RowMultiple);
	mcl->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
	CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ShardList")->show();
}

void CIntroTask::handleConnectShard() {
	uint shardid;
	std::string reason;

	CEGUI::MultiColumnList *mcl=(CEGUI::MultiColumnList *)CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ShardList/ShardMCL");
	CEGUI::ListboxItem *selected=mcl->getFirstSelectedItem();

	// if nothing is selected.
	if(selected==NULL) return;
	shardid=atoi(selected->getText().c_str());

	CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS/StatusTXT")->setText("Connecting to the shard...");
	reason=CNetworkTask::instance().connectToShard(shardid);
	if(reason.empty()) { // successful login to WS next try FES.
		std::string reason2="";
		// try to connect to the frontend.
		CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS/StatusTXT")->setText("Establishing your existance...");
		reason2=CNetworkTask::instance().connect();
		if(reason2.empty()) {
			// successfully connected
			CEGUI::WindowManager::getSingleton().getWindow("NetworkTask/ShardList")->hide();
			WWCOMMON::CTaskManager::instance().add(CPreGameTask::instance(), 60);
		} else {
			WWCOMMON::CTaskManager::instance().exit(); // failed, just bail out i guess.
		}
	} else { // login failed.
		WWCOMMON::CTaskManager::instance().exit(); // error dialog and go back to main screen...
	}
}

}; // END NAMESPACE WWCLIENT
