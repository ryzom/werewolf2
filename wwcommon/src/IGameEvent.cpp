/**
 * \file IGameEvent.cpp
 * \date February 2006
 * \author Henri Kuuste
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
#include "wwcommon/IGameEvent.h"

//
// Namespaces
//

namespace WWCOMMON {

IGameEvent::IGameEvent() : m_ID(IGameEvent::IGameEventID), Timestamp(0) {
	;
}

IGameEvent::~IGameEvent() {
	;
}

void IGameEvent::serial(NLMISC::IStream &f) {
	f.serial(m_ID);

	if(hasTimestamp()) {
		f.serial(Timestamp);
	}
}

uint16 IGameEvent::getId() const {
	return m_ID;
}

bool IGameEvent::hasTimestamp() const {
	return false;
}

uint32 IGameEvent::getPlayerID() const {
	return 0; // returns an invalid id by default.
}

void IGameEvent::setPlayerID(uint32 id) {
	// does nothing by default.
}

}; // END OF NAMESPACE WWCOMMON