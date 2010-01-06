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

bool handlePlayOnlineBtn(const CEGUI::EventArgs& e) {
	CIntroTask::instance().handlePlayOnline();
	return true;
}

bool handleConnectBtn(const CEGUI::EventArgs& e) {
	CIntroTask::instance().handleConnect();
	return true;
}

bool handleUserAcceptBtn(const CEGUI::EventArgs& e) {
	CIntroTask::instance().handleUserAccept();
	return true;
}

bool handleConnectShardBtn(const CEGUI::EventArgs& e) {
	CIntroTask::instance().handleConnectShard();
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

	// Set up windows.
	CEGUI::Editbox *edb=(CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/PassDL");
	edb->setTextMasked(true);
	edb->setMaskCodePoint( (CEGUI::utf32)'*' );

	// set up the columns on the shard list.
	CEGUI::MultiColumnList *mcl=(CEGUI::MultiColumnList *)CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList/ShardMCL");
	mcl->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
	mcl->addColumn("Shard ID",0,cegui_reldim(0.33f));
	mcl->addColumn("Shard Name",1,cegui_reldim(0.5f));
	mcl->addColumn("Players",2,cegui_reldim(0.2f));


	// Set up Main Menu event subscriptions.
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu/Quit")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handleQuitBtn);
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu/PlayOn")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handlePlayOnlineBtn);
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu/PlayOff")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handlePlayOfflineBtn);

	// Set up LS Connection event subscriptions
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/ConnectBTN")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handleConnectBtn);
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/UserDL")->
		subscribeEvent(CEGUI::Editbox::EventTextAccepted, handleUserAcceptBtn);
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList/ConnectBTN")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handleConnectShardBtn);
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

void CIntroTask::handlePlayOnline() {
	// Hide the main menu and show the login menu.
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->hide();
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS")->show();
	WWCOMMON::CTaskManager::instance().add(CNetworkTask::instance(), 40);
}

void CIntroTask::handleConnect() {
	std::string user, pass, reason;
	bool status=false;
	CEGUI::Editbox *edb;

	// Retrieve the username that was entered.
	edb=dynamic_cast<CEGUI::Editbox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/UserDL"));
	user = edb->getText().c_str();

	// Retrieve the password that was entered.
	edb = dynamic_cast<CEGUI::Editbox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/PassDL"));
	pass = edb->getText().c_str();

	// Validate the user/pass, simple.
	if(user.empty() || pass.empty()) {
		nlwarning("User entered invalid (blank) username or password.");
		return;
	}

	// Try to connect.
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/StatusTXT")->setText("Connecting...");
	reason = CNetworkTask::instance().connectToLoginServer(user,pass);

	// Check the results, if there was a problem update the box and return.
	if(!reason.empty()) {
		nlwarning("Failed to connect to LS with user/pass: %s", reason.c_str());
		CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/StatusTXT")->setText(reason.c_str());
		return;
	}

	nlinfo("assume we're here because we conected fine and have a shard list.");
	// If there was no problem we'll populate the shard list and let the user choose a shard.
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS")->hide();

	nlinfo("get the MCL.");
	CEGUI::MultiColumnList *mcl=dynamic_cast<CEGUI::MultiColumnList *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList/ShardMCL"));
	nlinfo("get the shard iterator.");
	CNetworkTask::TShardList shards = CNetworkTask::instance().getShardList();
	CNetworkTask::TShardList::iterator shardItr = shards.begin();
	uint32 i=0;
	nlinfo("start iterating shards.");
	while(shardItr != shards.end()) {
		nlinfo("iterating shard: %s", (*shardItr).ShardName.c_str());
		mcl->addRow();
		nlinfo("setting items: id.");
		mcl->setItem(new IntroListItem(NLMISC::toString((*shardItr).ShardId)), 0, i);
		nlinfo("setting items: name.");
		mcl->setItem(new IntroListItem(NLMISC::toString((*shardItr).ShardName)), 1, i);
		nlinfo("setting items: nb plrs.");
		mcl->setItem(new IntroListItem(NLMISC::toString((*shardItr).ShardNbPlayers)), 2, i);
		nlinfo("increase iterations.");
		i++;
		shardItr++;
	}
	nlinfo("set mcl selection modes.");
	mcl->setSelectionMode(CEGUI::MultiColumnList::RowMultiple);
	mcl->setSelectionMode(CEGUI::MultiColumnList::RowSingle);

	nlinfo("show shard list window.");
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList")->show();
}

void CIntroTask::handleConnectShard() {
	uint32 shardid;
	std::string reason;

	CEGUI::MultiColumnList *mcl=(CEGUI::MultiColumnList *)CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList/ShardMCL");
	CEGUI::ListboxItem *selected=mcl->getFirstSelectedItem();

	// Make sure we have a valid entry selected.
	if(selected==NULL) return;
	shardid=atoi(selected->getText().c_str());

	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList/StatusTXT")->setText("Connecting to the shard...");

	reason=CNetworkTask::instance().connectToShard(shardid);
	if(!reason.empty()) {
		nlwarning("Failed to connect to Shard: %s", reason.c_str());
		CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList/StatusTXT")->setText(reason.c_str());
		return;
	}

	reason=CNetworkTask::instance().connect();
	if(!reason.empty()) {
		nlwarning("Failed to connect to Frontend: %s", reason.c_str());
		CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList/StatusTXT")->setText(reason.c_str());
		return;
	}

	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList")->hide();
	WWCOMMON::CTaskManager::instance().add(CPreGameTask::instance(), 60);
}

void CIntroTask::handleUserAccept() {
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/PassDL")->activate();
}


}; // END NAMESPACE WWCLIENT
