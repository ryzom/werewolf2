/**
 * \file CLocalObjectView.h
 * \date February 2006
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

#ifndef __CLOCALOBJECTVIEW_H__
#define __CLOCALOBJECTVIEW_H__

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
#include "wwcommon/ISimulationObj.h"
#include "wwcommon/IGameEventListener.h"

//
// Callbacks
//

//
// Variables
//

//
// Class
//

namespace WWCLIENT {

class CLocalObjectView : public WWCOMMON::IGameEventListener {
public:
	CLocalObjectView(WWCOMMON::ISimulationObj *owner);
	~CLocalObjectView();

	/// Process list of sobs and generate unspawn messages for sobs that have left the view.
	void update();

	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
protected:
	/// A pointer to the sob whose server view this manages.
	WWCOMMON::ISimulationObj *m_OwnerSob;

	typedef std::list<uint32> TSobViewList;
	/// A map that keeps track of the sobs the owner can see.
	TSobViewList m_SobViewList;

};

}; // END NAMESPACE WWCLIENT

#endif // __CLOCALOBJECTVIEW_H__
