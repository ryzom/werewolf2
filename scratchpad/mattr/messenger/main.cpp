/**
 * \file main.cpp
 * \date October 2004
 * \author Matt Raykowski
 *
 * This sample tests out the event messaging system idea.
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

int main(void) {
	nlinfo("Creating Messenger...");
	CMessenger msgr;
	nlinfo("Creating a handler...");
	CSampleHandler *hnd=new CSampleHandler();

	nlinfo("Registering the handler...");
	msgr.registerHandler(hnd);

	CEventQuit msg;
	msg.setReason("I feel like it.");
	msgr.sendMessage(&msg);
}
