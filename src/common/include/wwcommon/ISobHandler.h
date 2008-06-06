/**
 * \file ISobHandler.h
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

#ifndef __ISOBHANDLER_H__
#define __ISOBHANDLER_H__

//
// Standard Includes
//
#include <vector>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/smart_ptr.h>

//
// Werewolf Includes
//	

//
// Namespaces
//

namespace WWCOMMON {

class ISobEvent;
class ISimulationObj;

class ISobHandler {
public:
	virtual ~ISobHandler() {;};
	typedef std::vector<uint16> eventList;

	/// Returns the events this handler handles.
	virtual eventList* getEventList() = 0;

	/// Handle the event for the given sob.
	virtual bool handleSobEvent(NLMISC::CSmartPtr<ISobEvent> event, ISimulationObj* subject) = 0;

	void setPriority(unsigned int priority) { m_Priority = priority; }
	unsigned int getPriority() { return m_Priority; }

private:
	unsigned int m_Priority;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __ISOBHANDLER_H__