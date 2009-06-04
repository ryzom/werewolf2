/**
 * \file CSobUnspawnEvent.h
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

#ifndef __CSOBUNSPAWNEVENT_H__
#define __CSOBUNSPAWNEVENT_H__

//
// Standard Includes
//

//
// System Includes
//

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
class CSobUnspawnEvent : public ISobEvent {
public:
	static const uint16 CSobUnspawnEventID;
	NLMISC_DECLARE_CLASS(CSobUnspawnEvent);

	CSobUnspawnEvent();
	virtual ~CSobUnspawnEvent();

	virtual void serial(NLMISC::IStream &f);

//~ Instance fields ****************************************
	// TODO henri:everyone figure out what fields to put here

protected:
	/// Register this object with the class registry.
	WWCOMMON_SETUP_EVENT(CSobUnspawnEvent);
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CSOBUNSPAWNEVENT_H__
