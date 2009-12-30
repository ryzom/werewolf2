/**
 * \file CPerformerChatView.h
 * \date December 2009
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

#ifndef __CPERFORMERCHATVIEW_H__
#define __CPERFORMERCHATVIEW_H__

//
// Standard Includes
//

//
// System Includes
//
#include <string>

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include <wwcommon/IView.h>
#include <wwcommon/CPerformer.h>
#include "entities/CEntityMedia.h"
#include <wwcommon/IGameEventListener.h>

//
// Namespaces
//

namespace WWCLIENT {

class CPerformerChatView : public WWCOMMON::IView, public WWCOMMON::IGameEventListener {
private:
	static const char* m_Name;
	WWCOMMON::CPerformer* m_performer;

public:
	CPerformerChatView(WWCOMMON::CPerformer* performer);
	virtual ~CPerformerChatView();

	void update();
	void render();

	const char* getViewName();

	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
};

}; // END NAMESPACE WWCLIENT

#endif // __CPERFORMERCHATVIEW_H__