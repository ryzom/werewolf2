/**
 * \file messenger.h
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

#ifndef WW_I_MESSENGER_H
#define WW_I_MESSENGER_H

//
// System Includes
//

//
// NeL Includes
//
#include "nel/misc/debug.h"

#include "i_message.h"
#include "i_messagehandler.h"

class CMessenger {
public:
	/// To add new listeners.
	void registerHandler(IMessageHandler *handler);
	/// And to remove listeners.
	void removeHandler(IMessageHandler *handler);

	/// To send messages.
	void sendMessage(IMessage *event);
private:
	typedef std::list<IMessageHandler *> HandlerList;

	/// Keep a list of handlers.
	std::list<IMessageHandler *> m_Handlers;
};

#endif // WW_I_MESSENGER_H