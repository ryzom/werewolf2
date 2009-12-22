/**
 * \file callbacks.h
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

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/service.h>
#include <nel/misc/displayer.h>
#include <nel/net/login_server.h>
#include <nel/net/login_cookie.h>

//
// Werewolf Includes
//

//
// Callbacks
//

//
// Variables
//

//
// Class
//

//
// Callbacks
//
void onConnectionClient(NLNET::TSockId from, const NLNET::CLoginCookie &cookie);
void onDisconnectClient(NLNET::TSockId from, void *arg );

void cbChatClient(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb);
void cbLGCharList(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase);
void cbSimEventClient(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb);
void cbPingRequestClient(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb);
void cbCHCR(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &clientcb);

#endif // __CALLBACKS_H__

