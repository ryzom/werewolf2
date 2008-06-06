/**
 * \file callbacks.cpp
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

//
// System Includes
//
#include <string>
#include <vector>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "../Shared/CMsgEntity.h"
#include "CEntityTask.h"
#include "CClient.h"
#include "callbacks.h"
// wx
#include "dlgCharList.h"

//
// Namespaces
//

dlgCharList *CharListDlg;

void cbPing(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	nlinfo("PONG");
}

void cbDisconnected (NLNET::TSockId from, void *arg) {
	nlwarning("You lost the connection to the server");
}

void cbIdentify(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	uint32 id;
	msgin.serial(id);
	nlinfo("Got id %d", id);

	// save the userid, we'll need it later.
	CClient::instance().setUserID(id);

	NLNET::CMessage msgout("LGCL");
	CClient::instance().getClient()->send(msgout);
}

void cbLGCharList(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	uint8 nbChars;
	std::vector<std::string> charList;
	CharListDlg = new dlgCharList(NULL, ID_DLG_CHAR, _("Choose Character"));
    
	// get the number of characters sent.
	msgin.serial(nbChars);
	nlinfo("Received my character list of %d chars.", nbChars);
	for(uint i=0;i<nbChars;i++) {
		// variables send per character.
		std::string charName;
		uint32 objID;

		// serialize the character
		msgin.serial(charName);
		msgin.serial(objID);

		// create the item for the list dialog
		wxListItem item;
		item.SetId(i);
		item.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_TEXT);
		item.SetColumn(0);
		item.SetText(wxString::Format(_T("%s"), charName.c_str()));
		item.SetData(objID);
		
		nlinfo("Receiving character: %s %d", charName.c_str(), objID);
		CharListDlg->charList->InsertItem(item);
	}
	
	CharListDlg->Show(true);
}

// all entity messages have:
// uint32 type - type of message
// uint32 source - the source uid/owner
// uint32 target - the entity id of the target
// see entity task for type descriptions.
void cbEntity(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	nlinfo("Received an entity message. This should get forwarded to the entity manager in the client.");
	CMsgEntity msgent;
	msgin.serial(msgent);
	CEntityTask::instance().recvMessage(msgent);
}
