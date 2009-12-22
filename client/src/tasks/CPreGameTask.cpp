/**
 * \file CPreGameTask.h
 * \date January 2005
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
#include <nel/net/message.h>

//
// Werewolf Includes
//
#include "tasks/CPreGameTask.h"
#include "tasks/CBackgroundTask.h"
#include "tasks/CNetworkTask.h"
#include "tasks/CGameTask.h"
#include "tasks/CResourceTask.h"
#include <wwcommon/CTaskManager.h>
#include <wwcommon/CGameSpawnRequestEvent.h>
#include <wwcommon/IGameEvent.h>
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CCharacterData.h>

//
// Namespaces
//

namespace WWCLIENT {

//
// CEGUI Event Callbacks
//

/// Handles when the connect button is pressed.
bool handleCharSelectBtn(const CEGUI::EventArgs& e) {
	CEGUI::Listbox *lbox=(CEGUI::Listbox *)CEGUI::WindowManager::getSingleton().getWindow("PreGameTask/SelectChar/CharBox");

	// if there aren't any items selected, just purge the event and continue on.
	if(lbox->getSelectedCount() <= 0)
		return true;

	// get the selected item.
	WWCOMMON::CCharacterData character = (*(WWCOMMON::CCharacterData *)(lbox->getFirstSelectedItem()->getUserData()));
	uint32 objId=character.CharacterID;

	nlinfo("Connecting to character ID: %d", objId);

	// inform the server we've found our character.
	WWCOMMON::CGameSpawnRequestEvent *event = new WWCOMMON::CGameSpawnRequestEvent();
	event->CharacterID = objId;
	WWCOMMON::CGameEventServer::instance().postEvent(event);

	CEGUI::WindowManager::getSingleton().getWindow("PreGameTask/SelectChar")->hide();

	for(size_t i = 0; i < lbox->getItemCount(); ++i) {
		delete lbox->getListboxItemFromIndex(i)->getUserData();
	}
	for(size_t i = 0; i < lbox->getItemCount(); ++i) {
		lbox->removeItem(lbox->getListboxItemFromIndex(i));
	}

	return true;
}

/// Handles when the connect button is pressed.
bool handleCharSelectCreateBtn(const CEGUI::EventArgs& e) {
	CEGUI::WindowManager::getSingleton().getWindow("PreGameTask/SelectChar")->hide();
	CEGUI::WindowManager::getSingleton().getWindow("werewolf/PreGameTask/CreateChar")->show();
	return true;
}

/// Handles when the connect button is pressed.
bool handleCharCreateCreateBtn(const CEGUI::EventArgs& e) {
	// TODO craft the message to the server to create the character.
	//werewolf/PreGameTask/CreateChar/CharNameBox
	//werewolf/PreGameTask/CreateChar/CharTypesList

	// Get the listbox containing the EMD type.
	CEGUI::Listbox *lbox=(CEGUI::Listbox *)CEGUI::WindowManager::getSingleton().getWindow("werewolf/PreGameTask/CreateChar/CharTypesList");

	// if there aren't any character types selected, just purge the event and continue on.
	if(lbox->getSelectedCount() <= 0)
		return true;

	CEGUI::ListboxTextItem *lboxItem = static_cast<CEGUI::ListboxTextItem *>(lbox->getFirstSelectedItem());

	nlinfo("user selected EMD: %s", lboxItem->getText().c_str());

	CEGUI::Editbox *editbox = (CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().getWindow("werewolf/PreGameTask/CreateChar/CharNameBox");
	std::string charName = editbox->getText().c_str();
	std::string charType = lboxItem->getText().c_str();

	NLNET::CMessage msgout("CH_CR"); // Character Create
	msgout.serial(charType);
	msgout.serial(charName);

	nlinfo("sending %s / %s to server.", charType.c_str(), charName.c_str());

	CNetworkTask::instance().send(msgout);

	// We won't hide this and show the character selection until we receive the message back from the server.
	// So look for that in the CH_CR_ACK message.

	return true;
}


//
// Class
//

void CPreGameTask::init() {
	try {
		using namespace CEGUI;
		nlinfo("Register Pre-Game Task Window Events.");
		CEGUI::WindowManager &wndMgr = CEGUI::WindowManager::getSingleton();
		
		// Reveal the character creation screen, hide the selection screen and subscribe events.
		wndMgr.getWindow("PreGameTask/SelectChar")->show();
		wndMgr.getWindow("PreGameTask/SelectChar/ConnectBTN")->subscribeEvent(CEGUI::PushButton::EventClicked, handleCharSelectBtn);
		wndMgr.getWindow("PreGameTask/SelectChar/CreateChar")->subscribeEvent(CEGUI::PushButton::EventClicked, handleCharSelectCreateBtn);
		wndMgr.getWindow("werewolf/PreGameTask/CreateChar/CharCreateBtn")->subscribeEvent(CEGUI::PushButton::EventClicked, handleCharCreateCreateBtn);

		// Retrieve the Listbox object.
		Listbox *lbox=static_cast<Listbox *>(wndMgr.getWindow("werewolf/PreGameTask/CreateChar/CharTypesList"));

		// Iterate through the entity media map and add the available character types to the listbox.
		entityMediaMap emdMap = CResourceTask::instance().getEntityMediaMap();
		entityMediaMap::iterator itr = emdMap.begin();
		while(itr != emdMap.end()) {
			CEntityMedia media = itr->second;
			nlinfo("Need to add '%s' to the character type box.", media.Name.c_str());
			lbox->addItem(new CharTypeItem(media.Name.c_str(), &media) );
			itr++;
		}

		nlinfo("Number of items in listbox: %d", lbox->getItemCount());

	} catch(CEGUI::UnknownObjectException &e) {
		nlinfo("Received object exception: %s", e.getMessage().c_str());
	}
}

void CPreGameTask::update() {
	;
}

void CPreGameTask::render() {
	;
}

void CPreGameTask::release() {
	;
}

void CPreGameTask::startGame() {
	// stop and remove the background.
	CBackgroundTask::instance().stop();
	WWCOMMON::CTaskManager::instance().remove(CBackgroundTask::instance());

	// remove this from the task list
	nlinfo("Removing pre-game task now.");
	WWCOMMON::CTaskManager::instance().remove(CPreGameTask::instance());

	// and start the game.
	nlinfo("starting game - initializing game task!!");
	WWCOMMON::CTaskManager::instance().add(CGameTask::instance(), 60);
	nlinfo("game task initialized! lets rock!");
}

void CPreGameTask::stop() {
	CBackgroundTask::instance().stop();
}

void CPreGameTask::insertCharacter(WWCOMMON::CCharacterData character) {
	CEGUI::Listbox *lbox=(CEGUI::Listbox *)CEGUI::WindowManager::getSingleton().getWindow("PreGameTask/SelectChar/CharBox");
	lbox->addItem(new CharItem(character.Name, new WWCOMMON::CCharacterData(character)) );
}

}; // END NAMESPACE WWCLIENT
