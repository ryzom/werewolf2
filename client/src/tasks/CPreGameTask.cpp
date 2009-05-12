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
#include <wwcommon/CTaskManager.h>
#include <wwcommon/CGameSpawnRequestEvent.h>
#include <wwcommon/IGameEvent.h>
#include <wwcommon/CGameEventServer.h>

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
	struct TCharacterData character = (*(struct TCharacterData *)(lbox->getFirstSelectedItem()->getUserData()));
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

//
// Class
//

void CPreGameTask::init() {
	// show the character selector to the world.
	CEGUI::WindowManager::getSingleton().getWindow("PreGameTask/SelectChar")->show();
	CEGUI::WindowManager::getSingleton().getWindow("PreGameTask/SelectChar/ConnectBTN")->subscribeEvent(CEGUI::PushButton::EventClicked, handleCharSelectBtn);
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
	WWCOMMON::CTaskManager::instance().remove(CPreGameTask::instance());

	// and start the game.
	WWCOMMON::CTaskManager::instance().add(CGameTask::instance(), 60);
}

void CPreGameTask::stop() {
	CBackgroundTask::instance().stop();
}

void CPreGameTask::insertCharacter(struct TCharacterData character) {
	CEGUI::Listbox *lbox=(CEGUI::Listbox *)CEGUI::WindowManager::getSingleton().getWindow("PreGameTask/SelectChar/CharBox");
	lbox->addItem(new CharItem(character.Name, new struct TCharacterData(character)) );
}

}; // END NAMESPACE WWCLIENT
