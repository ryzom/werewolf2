/**
 * \file messenger.cpp
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

//
// NeL Includes
//
#include "nel/misc/debug.h"

#include "i_message.h"
#include "i_messagehandler.h"
#include "messenger.h"

/// To add new listeners.
void CMessenger::registerHandler(IMessageHandler *handler) {
	m_Handlers.push_back(handler);
}

/// And to remove listeners.
void CMessenger::removeHandler(IMessageHandler *handler) {
	for(HandlerList::iterator itr=m_Handlers.begin() ; itr != m_Handlers.end() ; itr++) {
		if( (*itr) == handler ) {
			m_Handlers.erase(itr);
			return;
		}
	}
}

/// To send messages.
void CMessenger::sendMessage(IMessage *event) {
	for(HandlerList::iterator itr=m_Handlers.begin() ; itr!=m_Handlers.end() ; itr++) {
		(*itr)->HandleMessage(*event);
	}
}

//std::list<IMessageHandler *> m_Handlers;
