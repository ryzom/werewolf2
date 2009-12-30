/**
 * \file CSobChatEvent.cpp
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
#include "wwcommon/CSobChatEvent.h"

//
// Namespaces
//

namespace WWCOMMON {

const uint16 CSobChatEvent::CSobChatEventID = ISobEventID + 11;

CSobChatEvent::CSobChatEvent() : ISobEvent() {
	m_ID = CSobChatEvent::CSobChatEventID;
}

CSobChatEvent::~CSobChatEvent() {
	;
}

void CSobChatEvent::serial(NLMISC::IStream &f) {
	ISobEvent::serial(f);

	f.serial(TargetSobID);
	f.serial(ChatChannel);
	f.serial(ChatMessage);
}

}; // END OF NAMESPACE WWCOMMON
