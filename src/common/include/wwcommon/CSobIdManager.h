/**
 * \file CSobIdManager.h
 * \date April 2005
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

#ifndef __CSOBIDMANAGER_H__
#define __CSOBIDMANAGER_H__

//
// Standard Includes
//

//
// System Includes
//
#include <deque>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>

//
// Werewolf Includes
//	
#include "ISingleton.h"

//
// Namespaces
//

namespace WWCOMMON {

class CSobIdManager : public ISingleton<CSobIdManager> {
public:
	/// Ctor. Set current sob ID to 1. ID 0 is for the root.
	CSobIdManager() { m_CurrSobId=1; }

	/// Get the next available sob ID.
	uint32 getSobId() {
		uint32 sobid=0;

		if(m_AvailSobId.size() == 0)
			sobid=m_CurrSobId++;
		else {
			sobid=m_AvailSobId.front();
			m_AvailSobId.pop_front();
		}
		return sobid;
	}

	/// Release a sob ID for re-use.
	void releaseSobId(uint32 sobid) {
		m_AvailSobId.push_back(sobid);
	}

private:
	uint32 m_CurrSobId;
	std::deque<uint32> m_AvailSobId;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CSOBIDMANAGER_H__
