/**
 * \file CListener.cpp
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

#include "stdafx.h"
#include "CListener.h"

CListener::CListener(WWCOMMON::CGameEventServer::EListenerType type) {
	WWCOMMON::CGameEventServer::instance().addListener(this, type);
}

CListener::~CListener() {
	WWCOMMON::CGameEventServer::instance().removeListener(this, WWCOMMON::CGameEventServer::ALL_TYPES);
}

bool CListener::observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	if(event->getId() == CTestEvent::CTestEventID) {
		observeEvent(dynamic_cast<CTestEvent*>(event.getPtr()));
	}
	return true;
}

bool CListener::observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	if(event->getId() == CTestEvent::CTestEventID) {
		observeEvent(dynamic_cast<CTestEvent*>(event.getPtr()));
	}
	return true;
}

bool CListener::observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	if(event->getId() == CTestEvent::CTestEventID) {
		observeEvent(dynamic_cast<CTestEvent*>(event.getPtr()));
	}
	return true;
}

void CListener::observeEvent(CTestEvent* evt) {
	double observedTime = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());
	m_averageObservedDelay = (double)((m_numObservedEvents * m_averageObservedDelay) + (observedTime - evt->sendTime))/(double)(m_numObservedEvents + 1);
	++m_numObservedEvents;
}
