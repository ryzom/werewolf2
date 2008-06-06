/**
 * \file CSobMoveEvent.h
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

#ifndef __CSOBMOVEEVENT_H__
#define __CSOBMOVEEVENT_H__

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
#include "ISobStateChangeEvent.h" 

//
// Namespaces
//

namespace WWCOMMON {

/**
 An event used to move the given entity forward/backward.
 */
class CSobMoveEvent : public ISobStateChangeEvent {
public:
	static const uint16 CSobMoveEventID = ISobEventID + 3;
	NLMISC_DECLARE_CLASS(CSobMoveEvent);

	CSobMoveEvent();
	virtual ~CSobMoveEvent();

	virtual void serial(NLMISC::IStream &f);

protected:
	/// Register this object with the class registry.
	WWCOMMON_SETUP_EVENT(CSobMoveEvent);
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CSOBMOVEEVENT_H__