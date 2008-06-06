/**
 * \file callbacks.h
 * \date December 2004
 * \author Matt Raykowski
 *
 * This file contains exports for callback functions and the callback arrays 
 * for the login service.
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

extern NLNET::IService * gServiceInstance;

/**
 * Welcome Service callbacks
 */
void cbWSConnection(const std::string &serviceName, uint16 sid, void *arg);
void cbWSDisconnection (const std::string &serviceName, uint16 sid, void *arg);
void cbWSIdentification(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid);
void cbWSShardChooseShard(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid);
void cbWSClientConnected(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid);

typedef void (*ClientCallback)(NLMISC::CMemStream &msgin, NLNET::TSockId host);

/**
 * Client Connection callbacks.
 */
void cbDisconnectClient (NLMISC::CMemStream &msgin, NLNET::TSockId host);
void cbAskClientConnection (NLMISC::CMemStream &msgin, NLNET::TSockId host);
void cbClientConnection(NLNET::TSockId from, void *arg);
void cbClientDisconnection(NLNET::TSockId from, void *arg);
void cbClientVerifyLoginPassword(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase);
void cbClientChooseShard(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase);

#endif
