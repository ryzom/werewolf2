/**
 * \file CServerObjectView.h
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

#ifndef __CSERVEROBJECTVIEW_H__
#define __CSERVEROBJECTVIEW_H__

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
#include <wwcommon/ISimulationObj.h>
#include <wwcommon/IGameEventListener.h>
#include <wwcommon/IView.h>
#include <wwcommon/CSobSpawnEvent.h>
#include <wwcommon/CSobUnspawnEvent.h>
#include <wwcommon/CSobAddEvent.h>
#include <wwcommon/CSobRemoveEvent.h>

//
// Callbacks
//

//
// Variables
//

//
// Class
//

class CServerObjectView : public WWCOMMON::IView, WWCOMMON::IGameEventListener {
public:
	CServerObjectView(WWCOMMON::ISimulationObj *owner);
	virtual ~CServerObjectView();

	/// Process list of sobs and generate unspawn messages for sobs that have left the view.
	void update();
	void render();

	const char* getViewName();

	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event);

	bool spawn(WWCOMMON::CSobSpawnEvent *event);
	bool add(WWCOMMON::CSobAddEvent *event);
	bool unspawn(WWCOMMON::CSobUnspawnEvent *event);
	bool remove(WWCOMMON::CSobRemoveEvent *event);

protected:
	/// A pointer to the sob whose server view this manages.
	WWCOMMON::ISimulationObj *m_OwnerSob;
	static const char* m_Name;
};

#endif // __CSERVEROBJECTVIEW_H__
