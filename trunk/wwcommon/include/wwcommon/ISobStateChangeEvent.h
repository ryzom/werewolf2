/**
 * \file ISobStateChangeEvent.h
 * \date March 2006
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

#ifndef __ISOBSTATECHANGEEVENT_H__
#define __ISOBSTATECHANGEEVENT_H__

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
#include "ISobEvent.h" 

//
// Namespaces
//

namespace WWCOMMON {

/**
 The base class for state change events.
 */
class ISobStateChangeEvent : public ISobEvent {
public:
	static const uint16 ISobStateChangeEventID = ISobEventID + 6;
	NLMISC_DECLARE_CLASS(ISobStateChangeEvent);

	ISobStateChangeEvent();
	virtual ~ISobStateChangeEvent();

	virtual void serial(NLMISC::IStream &f);

//~ Instance fields ****************************************
	uint32					TargetState;
	double					StateDuration;
protected:
	bool hasTimestamp() const;

};

}; // END OF NAMESPACE WWCOMMON

#endif // __ISOBSTATECHANGEEVENT_H__