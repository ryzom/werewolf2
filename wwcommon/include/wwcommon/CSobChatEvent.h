/**
 * \file CSobChatEvent.h
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

#ifndef __CSOBCHATEVENT_H__
#define __CSOBCHATEVENT_H__

//
// Standard Includes
//

//
// System Includes
//
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/vector.h>

//
// Werewolf Includes
//
#include "ISobEvent.h" 

//
// Namespaces
//

namespace WWCOMMON {

/**
 An event used to send a DR vector to the given entity.
 At the moment not an event in and of itself. Sent with motion events.
 */
class CSobChatEvent : public ISobEvent {
public:
	static const uint16 CSobChatEventID;
	NLMISC_DECLARE_CLASS(CSobChatEvent);

	CSobChatEvent();
	virtual ~CSobChatEvent();

	virtual void serial(NLMISC::IStream &f);

//~ Instance fields ****************************************

	/// Sob ID this is intended for, can be directed or private based on ChatChannel.
	uint32 TargetSobID;

	/// Chat channel. For now this is 0: global, 1: private.
	uint32 ChatChannel;

	/// Contains the chat message string.
	std::string ChatMessage;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CSOBCHATEVENT_H__