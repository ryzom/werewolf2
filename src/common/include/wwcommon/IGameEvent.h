/**
 * \file IGameEvent.h
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

#ifndef __IGAMEEVENT_H__
#define __IGAMEEVENT_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/stream.h>
#include <nel/misc/smart_ptr.h>

//
// Werewolf Includes
//	
#include "CGameEventRegistrar.h"

//
// Namespaces
//

namespace WWCOMMON {

/**
 The basic game event.
 */
class IGameEvent : public NLMISC::IStreamable, public NLMISC::CRefCount {
public:
	static const uint16 IGameEventID = 0;
	NLMISC_DECLARE_CLASS(IGameEvent);

	virtual ~IGameEvent();
	uint16 getId() const;

	/** Implement these if the event needs to know the player ID on the server */
	virtual uint32 getPlayerID() const;
	virtual void setPlayerID(uint32);

	virtual void serial(NLMISC::IStream &f);

//~ Instance fields ****************************************
	double					Timestamp;

protected:
	IGameEvent();

	/// implement this to indicate if a timestamp would be needed
	virtual bool hasTimestamp() const;

//~ Instance fields ****************************************
	uint16					m_ID;

	/// Register this object with the class registry.
	WWCOMMON_SETUP_EVENT(IGameEvent);
};

}; // END OF NAMESPACE WWCOMMON

#endif // __IGAMEEVENT_H__