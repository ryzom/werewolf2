/**
 * \file CGameEventServer.h
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

#ifndef __CGAMEEVENTSERVER_H__
#define __CGAMEEVENTSERVER_H__

//
// Standard Includes
//
#include <set>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/smart_ptr.h>

//
// Werewolf Includes
//
#include "ISingleton.h"

//
// Namespaces
//

namespace WWCOMMON {

class IGameEvent;
class ISobEvent;
class IGameEventListener;

//#define EVENT_ID(_EVENT_) WWCOMMON:: ## _EVENT_ ## :: ## _EVENT_ ## ID
#define EVENT_ID(_EVENT_) WWCOMMON::_EVENT_::_EVENT_ ## ID

class CGameEventServer : public ISingleton<CGameEventServer> {
public:
	CGameEventServer();
	virtual ~CGameEventServer();

	enum EListenerType {
		PRE_EVENT,
		EVENT,
		POST_EVENT,
		ALL_TYPES
	};

	typedef NLMISC::CSmartPtr<IGameEvent> EventPtr;

	/// Add a listener for all events.
	void addListener(IGameEventListener* listener, EListenerType type);
	/// Add a listener for all events of specific kind.
	void addListener(IGameEventListener* listener, std::vector<uint16> eventIDs, EListenerType type);
	/// Add a listener for all events on specific SOBs.
	void addListener(IGameEventListener* listener, std::vector<uint32> sobIDs, EListenerType type);
	/// Add a listener for specific events on specific SOBs.
	void addListener(IGameEventListener* listener, std::vector<uint16> eventIDs, std::vector<uint32> sobIDs, EListenerType type);
	/// Add a listener for specific events on a single SOB.
	void addListener(IGameEventListener* listener, std::vector<uint16> eventIDs, uint32 sobID, EListenerType type);
	/// Add a listener for a single event on a single SOB.
	void addListener(IGameEventListener* listener, uint16 eventID, uint32 sobID, EListenerType type);

	/// Remove a listener from all events.
	void removeListener(IGameEventListener* listener, EListenerType type);
	/// Remove a listener from specific events.
	void removeListener(IGameEventListener* listener, std::vector<uint16> eventIDs, EListenerType type);
	/// Remove a listener from specific SOBs.
	void removeListener(IGameEventListener* listener, std::vector<uint32> sobIDs, EListenerType type);
	/// Remove a listener for specific events on specific SOBs.
	void removeListener(IGameEventListener* listener, std::vector<uint16> eventIDs, std::vector<uint32> sobIDs, EListenerType type);
	/// Remove a listener from specific events on a single SOB.
	void removeListener(IGameEventListener* listener, std::vector<uint16> eventIDs, uint32 sobID, EListenerType type);
	/// Remove a listener from a single event and SOB.
	void removeListener(IGameEventListener* listener, uint16 eventID, uint32 sobID, EListenerType type);
	/// Remove all SOB specific listeners for the given SOB.
	void removeListeners(uint32 sobID);
	/// Remove all event specific listeners for the given event.
	void removeListeners(uint16 eventID, EListenerType type);

	void postEvent(EventPtr& event);
	void postEvent(IGameEvent* event);
	void processEventQueue();

	void setDeltaMultiplier(double multiplier) {
		m_DeltaMultiplier = multiplier;
	}
	double getTestTime();

private:
	typedef std::list< EventPtr > eventQueue;
	eventQueue m_EventQueue;

	// using a set for unique listeners.
	typedef std::set<IGameEventListener*> listenerSet;
	typedef listenerSet listenerVector[ALL_TYPES];
	typedef CHashMap<uint16, listenerVector*> eventListenerMap;
	typedef CHashMap<uint32, listenerVector*> sobListenerMap;
	typedef struct {
		listenerVector** listeners;
		uint16 size;
	} eventListenerArray;
	typedef CHashMap<uint32, eventListenerArray> sobEventListenerMap;

	/// The listeners that are listening to everything.
	listenerVector m_GlobalListeners;
	/// The listeners that are listening to all events of a specific type.
	eventListenerMap m_EventListeners;
	/// The listeners that are listening to all events of a specific SOB.
	sobListenerMap m_SobListeners;	
	/// The listeners that are listening to specific events on specific SOBs.
	sobEventListenerMap m_SobEventListeners;

	bool processEvent(EventPtr& event, EListenerType type);
	bool notifyGlobalListeners(EventPtr& event, EListenerType type);
	bool notifyEventListeners(EventPtr& event, EListenerType type);
	bool notifySobListeners(NLMISC::CSmartPtr<ISobEvent>& event, EventPtr& gameEvent, EListenerType type);
	bool notifySobEventListeners(NLMISC::CSmartPtr<ISobEvent>& event, EventPtr& gameEvent, EListenerType type);
	bool notifyListener(EventPtr& event, IGameEventListener* listener, EListenerType type);

	void addListener(IGameEventListener* listener, listenerVector& vector, EListenerType type);
	void removeListener(IGameEventListener* listener, listenerVector& vector, EListenerType type);

	/// This is an unused member used to for symbol generation in MSVC. Fuck Microsoft.
	bool m_Unused;
	bool m_Processing;

	// this is used to multiply the average delta time added to the time check. 0 for the default time.
	double m_DeltaMultiplier;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CGAMEEVENTSERVER_H__
