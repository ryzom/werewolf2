/**
 * \file ISobEvent.h
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

#ifndef __ISOBEVENT_H__
#define __ISOBEVENT_H__

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

//
// Werewolf Includes
//
#include "IGameEvent.h"

//
// Namespaces
//

namespace WWCOMMON {

/**
 An event sent to a specific SOB.
 */
class ISobEvent : public IGameEvent {
public:
	static const uint16 ISobEventID = IGameEventID + 10000;
	NLMISC_DECLARE_CLASS(ISobEvent);

	virtual ~ISobEvent();

	virtual void serial(NLMISC::IStream &f);

	/// Return the ID relative to the ISobEventID
	static uint16 getSobRelativeEventID(uint16 eventID);

//~ Instance fields ****************************************
	uint32					SourceID;
	uint32					TargetID;

protected:
	ISobEvent();

};

}; // END OF NAMESPACE WWCOMMON

#endif // __ISOBEVENT_H__