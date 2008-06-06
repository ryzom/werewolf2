/**
 * \file CGameEventServer.cpp
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

//
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/stream.h>

//
// Werewolf Includes
//
#include "CGameEventServer.h"

#include "CSobDREvent.h"
#include "CSobMoveEvent.h"
#include "CSobOrientEvent.h"
#include "CSobStateRequestEvent.h"
#include "CSobStrafeEvent.h"
#include "IGameEvent.h"
#include "ISobEvent.h"
#include "IGameEventListener.h"
#include "IBaseSimulation.h"

//
// Namespaces
//

namespace WWCOMMON {

CGameEventServer::CGameEventServer() : m_Processing(false), m_Unused(false), m_DeltaMultiplier(0) {
}

CGameEventServer::~CGameEventServer() {
	CGameEventServer::sobEventListenerMap::iterator mapIter = m_SobEventListeners.begin();
	for(;mapIter != m_SobEventListeners.end(); ++mapIter) {
		for(int i = 0; i < mapIter->second.size; ++i) {
			delete[] mapIter->second.listeners[i];
		}
		delete[] mapIter->second.listeners;
	}

	CGameEventServer::sobListenerMap::iterator sobMapIter = m_SobListeners.begin();
	for(;sobMapIter != m_SobListeners.end(); ++sobMapIter) {
		delete[] sobMapIter->second;
	}

	CGameEventServer::eventListenerMap::iterator eventMapIter = m_EventListeners.begin();
	for(;eventMapIter != m_EventListeners.end(); ++eventMapIter) {
		delete[] eventMapIter->second;
	}
}

void CGameEventServer::Register(RegisterEventFn func, const std::string& EventClassName) {
	m_Unused=func(EventClassName);
}

void CGameEventServer::addListener(IGameEventListener* listener, EListenerType type) {
	addListener(listener, m_GlobalListeners, type);
}

void CGameEventServer::addListener(IGameEventListener* listener, std::vector<uint16> eventIDs, EListenerType type) {
	std::vector<uint16>::iterator eventIter = eventIDs.begin();
	for(;eventIter != eventIDs.end(); ++eventIter) {
		CGameEventServer::eventListenerMap::iterator mapIter = m_EventListeners.find(*eventIter);
		if(mapIter == m_EventListeners.end()) {
			CGameEventServer::listenerVector* listeners = new CGameEventServer::listenerVector[CGameEventServer::ALL_TYPES];
			addListener(listener, *listeners, type);
			m_EventListeners[*eventIter] = listeners;
		} else {
			addListener(listener, *(mapIter->second), type);
		}
	}
}

void CGameEventServer::addListener(IGameEventListener* listener, std::vector<uint32> sobIDs, EListenerType type) {
	std::vector<uint32>::iterator sobIter = sobIDs.begin();
	for(;sobIter != sobIDs.end(); ++sobIter) {
		CGameEventServer::sobListenerMap::iterator mapIter = m_SobListeners.find(*sobIter);
		if(mapIter == m_SobListeners.end()) {
			CGameEventServer::listenerVector* listeners = new CGameEventServer::listenerVector[CGameEventServer::ALL_TYPES];
			addListener(listener, *listeners, type);
			m_SobListeners[*sobIter] = listeners;
		} else {
			addListener(listener, *(mapIter->second), type);
		}
	}
}

void CGameEventServer::addListener(IGameEventListener* listener, std::vector<uint16> eventIDs, std::vector<uint32> sobIDs, EListenerType type) {
	std::vector<uint32>::iterator sobIter = sobIDs.begin();
	for(;sobIter != sobIDs.end(); ++sobIter) {
		std::vector<uint16>::iterator eventIter = eventIDs.begin();
		for(;eventIter != eventIDs.end(); ++eventIter) {
			addListener(listener, *eventIter, *sobIter, type);
		}
	}
}

void CGameEventServer::addListener(IGameEventListener* listener, std::vector<uint16> eventIDs, uint32 sobID, EListenerType type) {
	std::vector<uint16>::iterator eventIter = eventIDs.begin();
	for(;eventIter != eventIDs.end(); ++eventIter) {
		addListener(listener, *eventIter, sobID, type);
	}
}

void CGameEventServer::addListener(IGameEventListener* listener, uint16 eventID, uint32 sobID, EListenerType type) {
	if(eventID < ISobEvent::ISobEventID)
		return; // not a valid SOB event.

	uint16 sobEventID = ISobEvent::getSobRelativeEventID(eventID);

	CGameEventServer::sobEventListenerMap::iterator mapIter = m_SobEventListeners.find(sobID);
	CGameEventServer::eventListenerArray* listenerArray;
	if(mapIter == m_SobEventListeners.end()) {
		CGameEventServer::eventListenerArray newEventListenerArray = {NULL, 0};
		listenerArray = &(m_SobEventListeners[sobID] = newEventListenerArray);
	} else {
		listenerArray = &(mapIter->second);
	}

	if(listenerArray->size <= sobEventID) {
		// Make the array bigger
		uint16 newSize = sobEventID + 1;
		CGameEventServer::listenerVector** eventListeners = new CGameEventServer::listenerVector*[newSize];
		// fill the new entries
		for(int i = listenerArray->size; i < newSize; ++i) {
			eventListeners[i] = new CGameEventServer::listenerVector[CGameEventServer::ALL_TYPES];
		}
		// copy the old entries
		if(listenerArray->size > 0) {
			memcpy(eventListeners, listenerArray->listeners, listenerArray->size*sizeof(CGameEventServer::listenerVector*));
			delete[] listenerArray->listeners;
		}
		listenerArray->listeners = eventListeners;
		listenerArray->size = newSize;
	}

	addListener(listener, *(listenerArray->listeners[sobEventID]), type);


}

void CGameEventServer::addListener(IGameEventListener* listener, listenerVector& vector, EListenerType type) {
	if(type == CGameEventServer::ALL_TYPES) {
		for(int i = 0; i < CGameEventServer::ALL_TYPES; ++i) {
			vector[i].insert(listener);
		}
	} else {
		vector[type].insert(listener);
	}
}


void CGameEventServer::removeListener(IGameEventListener* listener, EListenerType type) {
	// remove from global listeners
	removeListener(listener, m_GlobalListeners, type);

	// remove from sob listeners
	CGameEventServer::sobListenerMap::iterator sobMapIter = m_SobListeners.begin();
	for(;sobMapIter != m_SobListeners.end(); ++sobMapIter) {
		removeListener(listener, *(sobMapIter->second), type);
	}

	// remove from event listeners
	CGameEventServer::eventListenerMap::iterator eventMapIter = m_EventListeners.begin();
	for(;eventMapIter != m_EventListeners.end(); ++eventMapIter) {
		removeListener(listener, *(eventMapIter->second), type);
	}

	// remove from sob event listeners
	CGameEventServer::sobEventListenerMap::iterator sobEventMapIter = m_SobEventListeners.begin();
	for(;sobEventMapIter != m_SobEventListeners.end(); ++sobEventMapIter) {
		for(int i = 0; i < sobEventMapIter->second.size; ++i) {
			removeListener(listener, *(sobEventMapIter->second.listeners[i]), type);
		}
	}
}

void CGameEventServer::removeListener(IGameEventListener* listener, std::vector<uint16> eventIDs, EListenerType type) {
	std::vector<uint16>::iterator eventIter = eventIDs.begin();
	for(;eventIter != eventIDs.end(); ++eventIter) {
		// remove from event listeners
		CGameEventServer::eventListenerMap::iterator mapIter = m_EventListeners.find(*eventIter);
		if(mapIter != m_EventListeners.end()) {
			removeListener(listener, *(mapIter->second), type);
		}

		// remove from sob event listeners
		uint16 sobEventID = ISobEvent::getSobRelativeEventID(*eventIter);
		CGameEventServer::sobEventListenerMap::iterator sobEventMapIter = m_SobEventListeners.begin();
		for(;sobEventMapIter != m_SobEventListeners.end(); ++sobEventMapIter) {
			if(sobEventMapIter->second.size <= sobEventID)
				continue;
			removeListener(listener, *(sobEventMapIter->second.listeners[sobEventID]), type);
		}
	}
}

void CGameEventServer::removeListener(IGameEventListener* listener, std::vector<uint32> sobIDs, EListenerType type) {
	std::vector<uint32>::iterator sobIter = sobIDs.begin();
	for(; sobIter != sobIDs.end(); ++sobIter) {
		// remove from sob listeners
		CGameEventServer::sobListenerMap::iterator mapIter = m_SobListeners.find(*sobIter);
		if(mapIter != m_SobListeners.end()) {
			removeListener(listener, *(mapIter->second), type);
		}

		// remove from sob event listeners
		CGameEventServer::sobEventListenerMap::iterator sobEventMapIter = m_SobEventListeners.find(*sobIter);
		if(sobEventMapIter != m_SobEventListeners.end()) {
			for(int i = 0; i < sobEventMapIter->second.size; ++i) {
				removeListener(listener, *(sobEventMapIter->second.listeners[i]), type);
			}
		}
	}
}

void CGameEventServer::removeListener(IGameEventListener* listener, std::vector<uint16> eventIDs, std::vector<uint32> sobIDs, EListenerType type) {
	std::vector<uint32>::iterator sobIter = sobIDs.begin();
	for(; sobIter != sobIDs.end(); ++sobIter) {
		std::vector<uint16>::iterator eventIter = eventIDs.begin();
		for(; eventIter != eventIDs.end(); ++eventIter) {
			removeListener(listener, *eventIter, *sobIter, type);
		}
	}
}

void CGameEventServer::removeListener(IGameEventListener* listener, std::vector<uint16> eventIDs, uint32 sobID, EListenerType type) {
	std::vector<uint16>::iterator eventIter = eventIDs.begin();
	for(; eventIter != eventIDs.end(); ++eventIter) {
		removeListener(listener, *eventIter, sobID, type);
	}
}

void CGameEventServer::removeListener(IGameEventListener* listener, uint16 eventID, uint32 sobID, EListenerType type) {
	CGameEventServer::sobEventListenerMap::iterator mapIter = m_SobEventListeners.find(sobID);
	if(mapIter == m_SobEventListeners.end())
		return;
	uint16 sobEventID = ISobEvent::getSobRelativeEventID(eventID);
	if(mapIter->second.size <= sobEventID)
		return;
	removeListener(listener, *(mapIter->second.listeners[sobEventID]), type);
}

void CGameEventServer::removeListeners(uint32 sobID) {
	// remove from sob listeners
	CGameEventServer::sobListenerMap::iterator mapIter = m_SobListeners.find(sobID);
	if(mapIter != m_SobListeners.end()) {
		delete[] mapIter->second;
		m_SobListeners.erase(mapIter);
	}

	// remove from sob event listeners
	CGameEventServer::sobEventListenerMap::iterator sobEventMapIter = m_SobEventListeners.find(sobID);
	if(sobEventMapIter != m_SobEventListeners.end()) {
		for(int i = 0; i < sobEventMapIter->second.size; ++i) {
			delete[] sobEventMapIter->second.listeners[i];
		}
		delete[] sobEventMapIter->second.listeners;
		m_SobEventListeners.erase(sobEventMapIter);
	}
}

void CGameEventServer::removeListeners(uint16 eventID, EListenerType type) {
	// remove from event listeners
	CGameEventServer::eventListenerMap::iterator mapIter = m_EventListeners.find(eventID);
	if(mapIter != m_EventListeners.end()) {
		delete[] mapIter->second;
		m_EventListeners.erase(mapIter);
	}

	// remove from sob event listeners
	uint16 sobEventID = ISobEvent::getSobRelativeEventID(eventID);
	CGameEventServer::sobEventListenerMap::iterator sobEventMapIter = m_SobEventListeners.begin();
	for(; sobEventMapIter != m_SobEventListeners.end(); ++sobEventMapIter) {
		if(sobEventMapIter->second.size <= sobEventID)
			continue;
		
		if(type == CGameEventServer::ALL_TYPES) {
			for(int i = 0; i < CGameEventServer::ALL_TYPES; ++i) {
				(*(sobEventMapIter->second.listeners[sobEventID]))[i].clear();
			}
		} else {
			(*(sobEventMapIter->second.listeners[sobEventID]))[type].clear();
		}
	}
}

void CGameEventServer::removeListener(IGameEventListener* listener, listenerVector& vector, EListenerType type) {
	if(type == CGameEventServer::ALL_TYPES) {
		for(int i = 0; i < CGameEventServer::ALL_TYPES; ++i) {
			vector[i].erase(listener);
		}
	} else {
		vector[type].erase(listener);
	}
}

void CGameEventServer::postEvent(IGameEvent* event) {
	CGameEventServer::EventPtr pevent = event;
	postEvent(pevent);
}

void CGameEventServer::postEvent(CGameEventServer::EventPtr& event) {
	if(m_Processing && (event->Timestamp <= getTestTime())) {
		if(!processEvent(event, CGameEventServer::PRE_EVENT)) {
			nldebug("Event %s pre-filtered.", event->getClassName().c_str());
			return;
		}
		if(!processEvent(event, CGameEventServer::EVENT)) {
			nldebug("Event %s filtered.", event->getClassName().c_str());
			return;
		}
		processEvent(event, CGameEventServer::POST_EVENT);
		return;
	}


	bool addedEvent=false;
	CGameEventServer::eventQueue::iterator iter = m_EventQueue.begin();
	while(iter != m_EventQueue.end()) {
		if((*iter)->Timestamp < event->Timestamp) {
			m_EventQueue.insert(iter, event);
			addedEvent=true;
		}
		iter++;
	}
	if(addedEvent==false) {
		m_EventQueue.push_back(event);
	}

}

void CGameEventServer::processEventQueue() {
	m_Processing = true;
	double time = getTestTime();
	while( 	!m_EventQueue.empty() && 
			m_EventQueue.back()->Timestamp <= time ) { 
		if(!processEvent(m_EventQueue.back(), CGameEventServer::PRE_EVENT)) {
			nldebug("Event %s pre-filtered.", m_EventQueue.back()->getClassName().c_str());
			m_EventQueue.pop_back();
			continue;
		}
		if(!processEvent(m_EventQueue.back(), CGameEventServer::EVENT)) {
			nldebug("Event %s filtered.", m_EventQueue.back()->getClassName().c_str());
			m_EventQueue.pop_back();
			continue;
		}
		processEvent(m_EventQueue.back(), CGameEventServer::POST_EVENT); 
		m_EventQueue.pop_back();
	}
	m_Processing = false;
}

bool CGameEventServer::processEvent(CGameEventServer::EventPtr& event, EListenerType type) {
	bool ret = true;
	ret = ret && notifyGlobalListeners(event, type);
	ret = ret && notifyEventListeners(event, type);
	if(event->getId() > ISobEvent::ISobEventID) {
		NLMISC::CSmartPtr<ISobEvent> sobEvent = dynamic_cast<ISobEvent*>(event.getPtr());
		ret = ret && notifySobListeners(sobEvent, event, type);
		ret = ret && notifySobEventListeners(sobEvent, event, type);
	}
	return ret;
}

bool CGameEventServer::notifyGlobalListeners(CGameEventServer::EventPtr& event, EListenerType type) {
	bool ret = true;
	CGameEventServer::listenerSet::iterator iter = m_GlobalListeners[type].begin();
	for(;iter != m_GlobalListeners[type].end(); ++iter) {
		ret = ret && notifyListener(event, (*iter), type);
	}
	return ret;
}

bool CGameEventServer::notifyEventListeners(CGameEventServer::EventPtr& event, EListenerType type) {
	bool ret = true;
	CGameEventServer::eventListenerMap::iterator mapIter = m_EventListeners.find(event->getId());
	if(mapIter == m_EventListeners.end())
		return ret;
	CGameEventServer::listenerSet::iterator iter = (*(mapIter->second))[type].begin();
	for(;iter != (*(mapIter->second))[type].end(); ++iter) {
		ret = ret && notifyListener(event, (*iter), type);
	}
	return ret;
}

bool CGameEventServer::notifySobListeners(NLMISC::CSmartPtr<ISobEvent>& event, CGameEventServer::EventPtr& gameEvent, EListenerType type) {
	bool ret = true;
	if(event->getId() < ISobEvent::ISobEventID)
		return ret; // not a valid SOB event.

	CGameEventServer::sobListenerMap::iterator mapIter = m_SobListeners.find(event->TargetID);
	if(mapIter == m_SobListeners.end())
		return ret;
	CGameEventServer::listenerSet::iterator iter = (*(mapIter->second))[type].begin();
	for(;iter != (*(mapIter->second))[type].end(); ++iter) {
		ret = ret && notifyListener(gameEvent, (*iter), type);
	}
	return ret;
}

bool CGameEventServer::notifySobEventListeners(NLMISC::CSmartPtr<ISobEvent>& event, CGameEventServer::EventPtr& gameEvent, EListenerType type) {
	bool ret = true;
	if(event->getId() < ISobEvent::ISobEventID)
		return ret; // not a valid SOB event.

	uint16 sobEventID = ISobEvent::getSobRelativeEventID(event->getId());
	CGameEventServer::sobEventListenerMap::iterator mapIter = m_SobEventListeners.find(event->TargetID);
	if(mapIter == m_SobEventListeners.end())
		return ret; // SOB not found
	if(mapIter->second.size <= sobEventID)
		return ret; // event not found
	CGameEventServer::listenerSet::iterator iter = (*(mapIter->second.listeners[sobEventID]))[type].begin();
	for(;iter != (*(mapIter->second.listeners[sobEventID]))[type].end(); ++iter) {
		ret = ret && notifyListener(gameEvent, (*iter), type);
	}
	return ret;
}

bool CGameEventServer::notifyListener(CGameEventServer::EventPtr& event, IGameEventListener* listener, EListenerType type) {
	if(listener == NULL)
		return true;
	switch(type) {
		case PRE_EVENT:
			return listener->observePreGameEvent(event);
			break;
		case EVENT:
			return listener->observeGameEvent(event);
			break;
		case POST_EVENT:
			return listener->observePostGameEvent(event);
			break;
		default:
			break;
	}
	return true;
}

double CGameEventServer::getTestTime() {
	if(m_DeltaMultiplier == 0)
		return getSimulation()->adjustedTime();
	return getSimulation()->adjustedTime() + (getSimulation()->smoothDeltaTime()*m_DeltaMultiplier);
}

}; // END OF NAMESPACE WWCOMMON