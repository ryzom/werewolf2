/**
 * \file i_message.h
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

#ifndef WW_I_MESSAGE_H
#define WW_I_MESSAGE_H

//
// System Includes
//

//
// NeL Includes
//
#include "nel/misc/debug.h"

/// The message interface.
class IMessage {
public:
	std::string EventType;
};

/// A sample message implementation.
class CEventQuit : public IMessage {
public:
	CEventQuit() { EventType="EventQuit"; }
	void setReason(std::string reason) { m_Reason=reason; }
	std::string getReason() { return m_Reason; }
protected:
	std::string m_Reason;
};
	
#endif // WW_I_MESSAGE_H