/**
 * \file CPerformerChatView.cpp
 * \date December 2009
 * \author Matt Raykowski
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
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include "views/CPerformerChatView.h"
#include <wwcommon/CMovementState.h>
#include <wwcommon/CStateManager.h>
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CSobChatEvent.h>
#include "CSimulationImpl.h"
#include "entities/CAnimationController.h"
#include "tasks/CGameTask.h"

//
// Namespaces
//

namespace WWCLIENT {

const char* CPerformerChatView::m_Name = "CHAT";

CPerformerChatView::CPerformerChatView(WWCOMMON::CPerformer* performer) : m_performer(performer) {
	// Add the sob chat event in, we need all of them.
	std::vector<uint16> events;
	events.push_back(WWCOMMON::CSobChatEvent::CSobChatEventID);

	WWCOMMON::CGameEventServer::instance().addListener(this, events, WWCOMMON::CGameEventServer::POST_EVENT);
}

CPerformerChatView::~CPerformerChatView() {
}

void CPerformerChatView::render() {
	// at some point we'll probably use this for chat bubbles...
}

void CPerformerChatView::update() {
	// at some point we'll probably use this for chat bubbles...
}

const char* CPerformerChatView::getViewName() {
	return m_Name;
}

bool CPerformerChatView::observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CPerformerChatView::observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CPerformerChatView::observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	if(event->getId() == EVENT_ID(CSobChatEvent)) {
		nlinfo("received sob chat event!!!");
		WWCOMMON::CSobChatEvent *chatEvent = dynamic_cast<WWCOMMON::CSobChatEvent *>(event.getPtr());
		nlinfo("chatting: %d to %d : %s", chatEvent->SourceID, chatEvent->TargetID, chatEvent->ChatMessage.c_str());

		// Only process events originated from me.
		if(chatEvent->SourceID == m_performer->getSobId()) {
			// Wow. I didn't communicate any character specific information to the client?
			// TODO fix this, add property information to SOBs
			std::stringstream strstr;
			strstr << "You said, \"" << chatEvent->ChatMessage << "\"";
			CGameTask::instance().recvChat(strstr.str());
		} else {
			std::stringstream strstr;
			strstr << "They (" << chatEvent->SourceID << ") said, \"" << chatEvent->ChatMessage << "\"";
			CGameTask::instance().recvChat(strstr.str());
		}

		// or a specific person
	}
	return true;
}

}; // END NAMESPACE WWCLIENT
