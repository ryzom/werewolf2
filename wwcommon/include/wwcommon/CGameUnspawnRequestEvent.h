/**
 * \file CGameUnspawnRequestEvent.h
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

#ifndef __CGAMEUNSPAWNREQUESTEVENT_H__
#define __CGAMEUNSPAWNREQUESTEVENT_H__

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
class CGameUnspawnRequestEvent : public IGameEvent {
public:
	static const uint16 CGameUnspawnRequestEventID;
	NLMISC_DECLARE_CLASS(CGameUnspawnRequestEvent);

	CGameUnspawnRequestEvent();
	virtual ~CGameUnspawnRequestEvent();

	virtual void serial(NLMISC::IStream &f);

	virtual uint32 getPlayerID() const;
	virtual void setPlayerID(uint32 id);

//~ Instance fields ****************************************
	uint32 SobID;

protected:
	uint32 m_PlayerID;

};

}; // END OF NAMESPACE WWCOMMON

#endif // __CGAMEUNSPAWNREQUESTEVENT_H__