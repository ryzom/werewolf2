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

bool handleMainMenuOptionsBtn(const CEGUI::EventArgs &e) {
	CIntroTask::instance().handleMainMenuOptions();
	return true;
}

bool handleOptionsCancelBtn(const CEGUI::EventArgs &e) {
	CIntroTask::instance().handleOptionsCancel();
	return true;
}

bool handleOptionsSaveBtn(const CEGUI::EventArgs &e) {
	CIntroTask::instance().handleOptionsSave();
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
	m_InitializedOptionsDlg = false;

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
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu/Options")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handleMainMenuOptionsBtn);

	

	// Set up LS Connection event subscriptions
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/ConnectBTN")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handleConnectBtn);
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS/UserDL")->
		subscribeEvent(CEGUI::Editbox::EventTextAccepted, handleUserAcceptBtn);
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList/ConnectBTN")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handleConnectShardBtn);

	CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/CancelBtn")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handleOptionsCancelBtn);
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/OptionsSaveBtn")->
		subscribeEvent(CEGUI::PushButton::EventClicked, handleOptionsSaveBtn);

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

	// If there was no problem we'll populate the shard list and let the user choose a shard.
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS")->hide();

	CEGUI::MultiColumnList *mcl=dynamic_cast<CEGUI::MultiColumnList *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList/ShardMCL"));
	CNetworkTask::TShardList shards = CNetworkTask::instance().getShardList();
	CNetworkTask::TShardList::iterator shardItr = shards.begin();
	uint32 i=0;
	while(shardItr != shards.end()) {
		mcl->addRow();
		mcl->setItem(new IntroListItem(NLMISC::toString((*shardItr).ShardId)), 0, i);
		mcl->setItem(new IntroListItem(NLMISC::toString((*shardItr).ShardName)), 1, i);
		mcl->setItem(new IntroListItem(NLMISC::toString((*shardItr).ShardNbPlayers)), 2, i);
		i++;
		shardItr++;
	}

	mcl->setSelectionMode(CEGUI::MultiColumnList::RowMultiple);
	mcl->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
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


void CIntroTask::handleMainMenuOptions() {
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->hide();
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg")->show();

	if(!m_InitializedOptionsDlg)
		initializeOptionsDlg();

	// Set the configured resolution as the default item
	uint32 currentWidth = CConfigTask::instance().configFile().getVar("ScreenWidth").asInt();
	uint32 currentHeight = CConfigTask::instance().configFile().getVar("ScreenHeight").asInt();
	uint32 currentDepth = CConfigTask::instance().configFile().getVar("ScreenDepth").asInt();

	std::string currentModeText = NLMISC::toString(currentWidth) + "x" + NLMISC::toString(currentHeight) + "x" + NLMISC::toString(currentDepth);
	CEGUI::Combobox *resCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/ResCombo"));
	resCombo->setText(currentModeText.c_str());

	// Set the AA default value.
	sint8 curAA = CConfigTask::instance().configFile().getVar("AntiAlias").asInt();
	CEGUI::Combobox *aaCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/AACombo"));
	if(curAA == -1) aaCombo->setText((CEGUI::utf8*)"None");
	if(curAA ==  0) aaCombo->setText((CEGUI::utf8*)"Max");
	if(curAA ==  2) aaCombo->setText((CEGUI::utf8*)"2x Sample");
	if(curAA ==  4) aaCombo->setText((CEGUI::utf8*)"4x Sample");

	// Set the fullscreen checkbox.
	CEGUI::Checkbox *fsCheck = dynamic_cast<CEGUI::Checkbox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/FullscreenChk"));
	bool fullscreen = CConfigTask::instance().configFile().getVar("Fullscreen").asBool();
	fsCheck->setSelected(fullscreen);

	// Set the default driver selection.
	CEGUI::Combobox *drvCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/DriverCombo"));
	if(CConfigTask::instance().configFile().getVar("OpenGL").asBool())
		drvCombo->setText("OpenGL");
	else
		drvCombo->setText("Direct3D");

	// Set the fullscreen checkbox.
	CEGUI::Checkbox *soundCheck = dynamic_cast<CEGUI::Checkbox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/SoundTab/EnableSoundBox"));
	bool soundOn = CConfigTask::instance().configFile().getVar("EnableSound").asBool();
	soundCheck->setSelected(soundOn);

	CEGUI::Combobox *sndDrvCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/SoundTab/SoundDriverCombo"));
	std::string cfgSoundDriver = CConfigTask::instance().configFile().getVar("SoundDriver").asString();
	if(	cfgSoundDriver == "OpenAL" ||
		cfgSoundDriver == "DirectSound" ||
		cfgSoundDriver == "XAudio2" ||
		cfgSoundDriver == "FMOD") {
		sndDrvCombo->setText(cfgSoundDriver.c_str());
	} else {
		nlwarning("Invalid sound driver configuration name in configuration file! '%s'", cfgSoundDriver.c_str());
	}
}

void CIntroTask::initializeOptionsDlg() {
	// Get the combo box with the video modes.
	CEGUI::Combobox *resCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/ResCombo"));

	// Populate available graphics modes.
	std::vector<NL3D::UDriver::CMode> modesAvailable;
	NL3D::UDriver::CMode currentMode;
	
	C3DTask::instance().driver().getCurrentScreenMode(currentMode);
	C3DTask::instance().driver().getModes(modesAvailable);

	// First inject all available modes.
	std::vector<NL3D::UDriver::CMode>::iterator modeIterator = modesAvailable.begin();
	while(modeIterator != modesAvailable.end()) {
		NL3D::UDriver::CMode mode = (*modeIterator);
		std::string modeText = NLMISC::toString(mode.Width) + "x" + NLMISC::toString(mode.Height) + "x" + NLMISC::toString(mode.Depth);
		CEGUI::ListboxTextItem *item = new CEGUI::ListboxTextItem(modeText.c_str(), 0, &mode);
		resCombo->addItem(item);
		modeIterator++;
	}
	resCombo->setReadOnly(true);
	

	// Insert values into the AACombo box.
	CEGUI::Combobox *aaCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/AACombo"));
	aaCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"None"));
	aaCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"2x Sample"));
	aaCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"4x Sample"));
	aaCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"Max"));
	aaCombo->setReadOnly(true);

	// Populate the driver dropdown.
	CEGUI::Combobox *drvCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/DriverCombo"));
	drvCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"OpenGL"));
	drvCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"Direct3D"));
	drvCombo->setReadOnly(true);

	// Set up the sound drivers.
	CEGUI::Combobox *sndDrvCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/SoundTab/SoundDriverCombo"));
	sndDrvCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"OpenAL"));
#ifdef NL_OS_WINDOWS
	sndDrvCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"DirectSound"));
	sndDrvCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"XAudio2"));
#endif // NL_OS_WINDOWS
	sndDrvCombo->addItem(new CEGUI::ListboxTextItem((CEGUI::utf8*)"FMOD"));
	sndDrvCombo->setReadOnly(true);

	m_InitializedOptionsDlg=true;
}

void CIntroTask::handleOptionsSave() {
	CEGUI::Combobox *drvCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/DriverCombo"));
	CEGUI::ListboxItem *item = drvCombo->getSelectedItem();
	if(item) { // Only process this if we made a selection.
		std::string driverName = drvCombo->getSelectedItem()->getText().c_str();
		uint8 drv = 0;
		if(drvCombo->getSelectedItem()->getText() == "OpenGL") {
			drv = 1;
		} else if(drvCombo->getSelectedItem()->getText() == "Direct3D") {
			drv = 0;
		} else {
			nlwarning("Invalid driver selection made in options dialog: %s", drvCombo->getSelectedItem()->getText().c_str());
		}
		CConfigTask::instance().configFile().getVar("OpenGL").setAsInt(drv);
	}

	CEGUI::Combobox *aaCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/AACombo"));
	sint8 curAA = -1;
	if(aaCombo->getText()=="None") curAA = -1;
	if(aaCombo->getText()=="Max") curAA =  0;
	if(aaCombo->getText()=="2x Sample") curAA =  2;
	if(aaCombo->getText()=="4x Sample") curAA =  4;
	CConfigTask::instance().configFile().getVar("AntiAlias").setAsInt(curAA);

	CEGUI::Checkbox *fsCheck = dynamic_cast<CEGUI::Checkbox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/VidTab/FullscreenChk"));
	bool fullscreen = fsCheck->isSelected();
	CConfigTask::instance().configFile().getVar("Fullscreen").setAsInt((int)fullscreen);

	CEGUI::Checkbox *soundCheck = dynamic_cast<CEGUI::Checkbox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/SoundTab/EnableSoundBox"));
	bool soundOn = soundCheck->isSelected();
	CConfigTask::instance().configFile().getVar("EnableSound").setAsInt((int)soundOn);	

	CEGUI::Combobox *sndDrvCombo = dynamic_cast<CEGUI::Combobox *>(CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg/SoundTab/SoundDriverCombo"));
	item = sndDrvCombo->getSelectedItem();
	if(item) {
		std::string soundDriverName = sndDrvCombo->getSelectedItem()->getText().c_str();
		if(	soundDriverName == "OpenAL" ||
			soundDriverName == "DirectSound" ||
			soundDriverName == "XAudio2" ||
			soundDriverName == "FMOD") {
				CConfigTask::instance().configFile().getVar("SoundDriver").setAsString(soundDriverName);
		} else {
			nlwarning("Invalid sound driver type '%s' chosen!", soundDriverName.c_str());
		}
	}

	nlinfo("Saving configuration file changes!");
	CConfigTask::instance().configFile().save();
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->show();
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg")->hide();
}

void CIntroTask::handleOptionsCancel() {
	CEGUI::WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->show();
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/OptionsDlg")->hide();
}

}; // END NAMESPACE WWCLIENT
