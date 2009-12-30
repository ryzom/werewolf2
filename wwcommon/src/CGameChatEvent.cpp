/**
 * \file CGameChatEvent.cpp
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
#include "wwcommon/CGameChatEvent.h"

//
// Namespaces
//

namespace WWCOMMON {

const uint16 CGameChatEvent::CGameChatEventID = IGameEventID + 3;

CGameChatEvent::CGameChatEvent() : IGameEvent(), m_PlayerID(0), SourceSobID(0), TargetSobID(0), ChatChannel(0), Processed(false) {
	m_ID = CGameChatEvent::CGameChatEventID;
}

CGameChatEvent::~CGameChatEvent() {
	;
}

void CGameChatEvent::serial(NLMISC::IStream &f) {
	IGameEvent::serial(f);

	f.serial(SourceSobID);
	f.serial(TargetSobID);
	f.serial(ChatChannel);
	f.serial(ChatMessage);
	f.serial(Processed);
}

uint32 CGameChatEvent::getPlayerID() const {
	return m_PlayerID;
}

void CGameChatEvent::setPlayerID(uint32 id) {
	m_PlayerID = id;
}

}; // END OF NAMESPACE WWCOMMON
