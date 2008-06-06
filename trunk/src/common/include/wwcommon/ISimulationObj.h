/**
 * \file ISimulationObj.h
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

#ifndef __ISIMULATIONOBJ_H__
#define __ISIMULATIONOBJ_H__

//
// Standard Includes
//
#include <vector>
#include <list>

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
#include "general.h"
#include "CSobIdManager.h"
#include "IGameEventListener.h"
#include "CBaseProperty.h"

//
// Namespaces
//

namespace WWCOMMON {

class ISobEvent;
class ISobController;
class ISimulationState;
class IView;
class ISobHandler;
class CGameEventServer;

class ISimulationObj : IGameEventListener {
public:
	// TODO henri:everyone possible point of optimization. Could use vector instead of map.
	typedef std::hash_map<const char*, ISobController*, std::hash<const char*>, streqpred> controllerMap;
	typedef std::hash_map<const char*, IView*, std::hash<const char*>, streqpred> viewMap;
	typedef std::set<ISobHandler*> handlerList;

	/// Standard ctor. Acquires a unique ID.
	ISimulationObj();
	/// Standard dtor. Releases it's unique ID.
	virtual ~ISimulationObj();

	/// Initialize the sob.
	virtual void init();

	/// Get the current unique sob ID.
	uint32 getSobId();
	/// Set a new ID and release the old one for reuse. Note, that makes this a dangerous method to call.
	void setSobId(uint32 sobId);
	/// Get the ID of the sob that owns this.
	uint32 getOwnerSobId();
	/// Set the owner of this sob.
	void setOwnerSobId(uint32 sobid);

	/// Render the views.
	virtual void render();
	virtual void update();
	void updateViews();
	void updateControllers();
	void updateStates();

	typedef struct stateLayerStruct {
		CBaseProperty<ISimulationState*> ActiveState;
		double Duration;
		double CorrectionDt;

		stateLayerStruct() : ActiveState("activeState"), Duration(0.0f), CorrectionDt(0.0f) {
			ActiveState.setValue(NULL);
		}

		bool isActive() {
			return ActiveState.getValue() != NULL;
		}
	} stateLayer;

	void addStateLayer(uint8 layerID);
	void setState(ISimulationState* state);
	ISimulationState* getState(uint8 layer);
	stateLayer* getStateLayer(uint8 layer);
	bool changeState(ISimulationState* targetState, double duration, double eventDt);
	std::vector<uint32> getActiveStateList();
	void setActiveStateList(std::vector<uint32> *list);

	bool addView(IView* view);
	bool removeView(const char* name);
	IView* getView(const char* name);

	bool addController(ISobController* controller);
	bool removeController(const char* name);
	ISobController* getController(const char* name);

	// event listener methods
	bool observePreGameEvent(CGameEventServer::EventPtr event);
	bool observeGameEvent(CGameEventServer::EventPtr event);
	bool observePostGameEvent(CGameEventServer::EventPtr event);

	/// add a new event handler
	void addHandler(ISobHandler* handler);
	ISobHandler* getHandler(uint16 eventID);

protected:
	uint32 m_OwnerSobId;
	uint32 m_SobId;

	viewMap m_Views;
	controllerMap m_Controllers;

	handlerList m_Handlers;
	ISobHandler** m_HandlerMap;
	uint16 m_HandlerMapSize;

	stateLayer* m_StateLayers;
	uint8 m_StateLayerSize;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __ISIMULATIONOBJ_H__
