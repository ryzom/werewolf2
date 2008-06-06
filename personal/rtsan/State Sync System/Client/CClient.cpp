/**
 * \file CClient.cpp
 * \date February 2005
 * \author Henri Kuuste
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
// Standard Includes
//

//
// System Includes
//
#include <windows.h>
#undef min
#undef max
#include <string>
#include <cstdio>

//
// NeL Includes
//
#include <nel/net/login_cookie.h> 

//
// Werewolf Includes
//
#include "../Shared/CMsgEntity.h"
#include "../Shared/CMessageTranslator.h"
#include "CClient.h"
#include "callbacks.h"
#include "CEntityTask.h"

//
// Namespaces
//

//
// Global Variables
//
NLNET::TCallbackItem CallbackArray[] = {
	{ "PING", cbPing },
	{ "IDENT", cbIdentify },
	{ "LGCL", cbLGCharList },
	{ "ENT", cbEntity },
};

void CClient::init() {
	// create the client
	cbClient=new NLNET::CCallbackClient();
	cbClient->addCallbackArray(CallbackArray, sizeof(CallbackArray)/sizeof(CallbackArray[0]));
	cbClient->setDisconnectionCallback(cbDisconnected, NULL);

	m_state = new CPCState();
	m_CLState = CLSTATE_NONE;

	m_Server="localhost:50005";
	m_User="testuser";
	m_Pass="testpass";

	CMessageTranslator::instance().init();
	CEntityTask::instance().init();
}

void CClient::clear() {
	disconnect();
	if(cbClient) {
		delete cbClient;
		cbClient = NULL;
	}
	if(m_state) {
		delete m_state;
		m_state = NULL;
	}
}

bool CClient::isConnected() {
	if(!cbClient) {
		nlwarning("Not connected, client doesn't exist.");
		return false;
	}
	if(cbClient->connected()) {
		nlwarning("Not connected, client not connected.");
		return true;
	}
	return false;
}

void CClient::connectUsing(std::string srv, std::string usr, std::string pass) {
	if(!srv.empty())
		m_Server=srv;
	else
		m_Server="localhost:50005";

	// add default port if not set by the config file
	if(m_Server.find(":") == std::string::npos)
		m_Server += ":50005";

	m_User=usr;
	m_Pass=pass;
}

bool CClient::connect() {
	if(!cbClient)
		init();
	if(cbClient->connected())
		return true;
	try {
		//NLNET::CLoginCookie lc;
		//lc.set(1,1,1);

		// connect to the server.
		//std::string addr = "localhost:50005";
		nlinfo("Connecting to: %s", m_Server.c_str());
		cbClient->connect(NLNET::CInetAddress(m_Server));

		nlinfo("Sending VLP with: %s / %s", m_User.c_str(), m_Pass.c_str());
		// send the service validation message with our cookie.
		NLNET::CMessage msgout("VLP");
		msgout.serial(m_User);
		msgout.serial(m_Pass);
		cbClient->send(msgout);

	} catch(NLNET::ESocket &e) {
		nlwarning ("Connection refused to server: %s", e.what ());
		return false;
	}
	return true;
}

bool CClient::disconnect() {
	if(cbClient) {
		if(cbClient->connected())
			cbClient->disconnect();
		else
			nlwarning("Cannot disconnect if not already connected.");
	} else
		nlwarning("Cannot disconnect if client doesn't exist.");

	return true;
}

NLNET::CCallbackClient *CClient::getClient() {
	return cbClient;
}

void CClient::update() {
	if(cbClient && cbClient->connected()) {
		cbClient->update();
	}
}

int CClient::mainLoop() {
#ifdef NL_OS_WINDOWS
	HANDLE hIn;
	bool Continue = TRUE;
	INPUT_RECORD InRec;
	DWORD NumRead;

	hIn = GetStdHandle(STD_INPUT_HANDLE);

	while (Continue) {
		if(cbClient && cbClient->connected()) {
			cbClient->update();
		}
		ReadConsoleInput(hIn, &InRec, 1, &NumRead);
		switch (InRec.EventType) {
		case KEY_EVENT:
			// handle keys relevant only to spawning an entity.
			if(InRec.Event.KeyEvent.uChar.AsciiChar == 's' && InRec.Event.KeyEvent.bKeyDown) {
				m_CLState=CLSTATE_SPAWNING;
			}
			if(m_CLState == CLSTATE_SPAWNING) {
				if(InRec.Event.KeyEvent.uChar.AsciiChar == '0' && InRec.Event.KeyEvent.bKeyDown) {
					spawnEntity(0);
					m_CLState=CLSTATE_NONE;
				}
				break;
			}

			if(InRec.Event.KeyEvent.uChar.AsciiChar == 'i' && InRec.Event.KeyEvent.bKeyDown) {
				changeState((uint32)CPCState::Idle);
			}
			if(InRec.Event.KeyEvent.uChar.AsciiChar == 'w' && InRec.Event.KeyEvent.bKeyDown) {
				changeState((uint32)CPCState::Walk);
			}
			if(InRec.Event.KeyEvent.uChar.AsciiChar == 'r' && InRec.Event.KeyEvent.bKeyDown) {
				changeState((uint32)CPCState::Run);
			}
			break;
		}
	}
#endif
	return 1;
}

void CClient::spawnEntity(uint32 objectID) {
	if(cbClient && cbClient->connected()) {
		NLNET::CMessage msgout("SPW");
		
		/*
		 * Create the message we'll send. Remember, you can fill all of the fields in
		 * CMsgEntity up, but only the ones in the map are necessary. Also remember that
		 * the ones in the map are required - not filling in the minimums for a type 
		 * will cause problems.
		 * For ENT_MSG_SPAWN:
		 * source, target, objType, objectid
		 */
		CMsgEntity msgent;
		msgent.Type=CMsgEntity::ENT_MSG_SPAWN;
		msgent.OwnerID=CClient::instance().getUserID();
		msgent.TargetID=CEntityTask::EID_NONE; // not used for requests.
		msgent.ObjType="default";
		msgent.ObjectID=objectID;

		// dump it into the outgoing message
		msgout.serial(msgent);
		cbClient->send(msgout);
		nlinfo("Sent spawn request for Object ID: %d", objectID);
	}
}

void CClient::changeState(uint32 state) {
	m_state->setId(state);
	m_state->move();
	if(cbClient && cbClient->connected()) {
		NLNET::CMessage msgout("ST");
		msgout.serial(state);
		cbClient->send(msgout);
	}
}

uint32 CClient::getUserID() {
	return m_UserID;
}
void CClient::setUserID(uint32 uid) {
	m_UserID=uid;
}
