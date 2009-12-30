/**
 * \file CGameChatEvent.h
 * \date June 2006
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

#ifndef __CGAMECHATEVENT_H__
#define __CGAMECHATEVENT_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "IGameEvent.h" 

//
// Namespaces
//

namespace WWCOMMON {

/**
 An event used to request a spawn.
 */
class CGameChatEvent : public IGameEvent {
public:
	static const uint16 CGameChatEventID;
	NLMISC_DECLARE_CLASS(CGameChatEvent);

	CGameChatEvent();
	virtual ~CGameChatEvent();

	virtual void serial(NLMISC::IStream &f);

	virtual uint32 getPlayerID() const;
	virtual void setPlayerID(uint32 id);

//~ Instance fields ****************************************

	/// Sob ID sending the chat message. 0 for system-wide.
	uint32 SourceSobID;

	/// Sob ID this is intended for, can be directed or private based on ChatChannel.
	uint32 TargetSobID;

	/// Chat channel. For now this is 0: global, 1: private.
	uint32 ChatChannel;

	/// Contains the chat message string.
	std::string ChatMessage;

	/// Has this event been processed by the server.
	bool Processed;

protected:
	uint32 m_PlayerID;

};

}; // END OF NAMESPACE WWCOMMON

#endif // __CGAMECHATEVENT_H__