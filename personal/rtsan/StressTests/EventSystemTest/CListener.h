/**
 * \file CListener.h
 * \date November 2006
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

#ifndef __CLISTENER_H__
#define __CLISTENER_H__

#include <wwcommon/IGameEventListener.h>
#include <wwcommon/CGameEventServer.h>
#include <nel/misc/time_nl.h>
#include "CTestEvent.h"

class CListener : public WWCOMMON::IGameEventListener {
public:
	CListener(WWCOMMON::CGameEventServer::EListenerType type);
	~CListener();

	bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event);
	bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event);

	double getObservedDelay() { return m_averageObservedDelay; }

private:
	void observeEvent(CTestEvent* evt);
	uint m_numObservedEvents;
	double m_averageObservedDelay;
};

#endif // __CLISTENER_H__
