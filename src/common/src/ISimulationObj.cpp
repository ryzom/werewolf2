/**
 * \file ISimulationObject.cpp
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

//
// Werewolf Includes
//
#include "ISimulationObj.h"
#include "CGameEventServer.h"
#include "IView.h"
#include "ISobHandler.h"
#include "ISimulationState.h"
#include "ISobEvent.h"
#include "ISobController.h"
#include "IBaseSimulation.h"
#include "CStateManager.h"

//
// Namespaces
//

namespace WWCOMMON {

ISimulationObj::ISimulationObj() : m_HandlerMapSize(0), m_StateLayerSize(0) {
	m_SobId=CSobIdManager::instance().getSobId();
	nlinfo("Created with sob id: %d", m_SobId);
}

ISimulationObj::~ISimulationObj() {
	WWCOMMON::CSobIdManager::instance().releaseSobId(m_SobId);

	// delete views
	ISimulationObj::viewMap::iterator viewIter = m_Views.begin();
	for(;viewIter != m_Views.end();++viewIter) {
		delete viewIter->second;
	}
	m_Views.clear();

	// delete controllers
	ISimulationObj::controllerMap::iterator controllerIter = m_Controllers.begin();
	for(;controllerIter != m_Controllers.end(); ++controllerIter) {
		delete controllerIter->second;
	}
	m_Controllers.clear();

	// delete handler map
	if(m_HandlerMapSize > 0) {
		delete[] m_HandlerMap;
	}

	// delete state layers
	if(m_StateLayerSize > 0) {
		delete[] m_StateLayers;
	}

	// remove listeners
	WWCOMMON::CGameEventServer::instance().removeListeners(getSobId());
}

void ISimulationObj::init() {
	nlinfo("Initialized SOB with ID %d", m_SobId);

	std::vector<uint32> sobIDs;
	sobIDs.push_back(m_SobId);
	CGameEventServer::instance().addListener(this, sobIDs, CGameEventServer::EVENT);
}

uint32 ISimulationObj::getSobId() { return m_SobId; }

void ISimulationObj::setSobId(uint32 sobId) { 
	CSobIdManager::instance().releaseSobId(m_SobId);
	m_SobId=sobId;
}

uint32 ISimulationObj::getOwnerSobId() { return m_OwnerSobId; }

void ISimulationObj::setOwnerSobId(uint32 sobid) { m_OwnerSobId=sobid; }

void ISimulationObj::render() {
	ISimulationObj::viewMap::iterator viewIter = m_Views.begin();
	for(;viewIter != m_Views.end();++viewIter) {
		if(viewIter->second->isVisible()) {
			viewIter->second->render();
		}
	}
}

void ISimulationObj::update() {
	updateStates();
	updateControllers();
	updateViews();
}

void ISimulationObj::updateViews() {
	ISimulationObj::viewMap::iterator viewIter = m_Views.begin();
	for(;viewIter != m_Views.end();++viewIter) {
		if(viewIter->second->isVisible()) {
			viewIter->second->update();
		}
	}
}

void ISimulationObj::updateControllers() {
	ISimulationObj::controllerMap::iterator iter = m_Controllers.begin();
	for(;iter != m_Controllers.end(); ++iter) {
		iter->second->update();
	}
}

void ISimulationObj::updateStates() {
	for(uint8 i = 0; i < m_StateLayerSize; ++i) {
		if(m_StateLayers[i].ActiveState.getValue() != NULL)
			m_StateLayers[i].ActiveState.getValue()->handle(this);
	}
}

void ISimulationObj::addStateLayer(uint8 layerID) {
	if(layerID >= CStateFactory::ST_LAYER_END) {
		nlwarning("Tried to add a nonexistent layer %d to SOB %d", layerID, getSobId());
		return;
	}
	if(m_StateLayerSize <= layerID) {
		// make the array larger
		uint8 newSize = layerID + 1;
		ISimulationObj::stateLayer* newLayers = new ISimulationObj::stateLayer[newSize];

		// fill new entries with NULL
//		memset(&newLayers[m_StateLayerSize], 0, (newSize - m_StateLayerSize)*sizeof(ISimulationObj::stateLayer));

		// copy old entries
		if(m_StateLayerSize > 0) {
			memcpy(newLayers, m_StateLayers, m_StateLayerSize*sizeof(ISimulationObj::stateLayer));
			delete[] m_StateLayers;
		}

		m_StateLayers = newLayers;
		m_StateLayerSize = newSize;
	}

	m_StateLayers[layerID].ActiveState.setValue(CStateFactory::instance().getDefault(layerID));
}

void ISimulationObj::setState(ISimulationState* state) {
	if(state == NULL)
		return;
	if(m_StateLayerSize <= state->getLayerId())
		return; // state can not be handled by this sob
	if(m_StateLayers[state->getLayerId()].isActive())
		m_StateLayers[state->getLayerId()].ActiveState.setValue(state);
}

ISimulationState* ISimulationObj::getState(uint8 layer) {
	if(m_StateLayerSize <= layer)
		return NULL; // no such layer
	return m_StateLayers[layer].ActiveState.getValue();
}

ISimulationObj::stateLayer* ISimulationObj::getStateLayer(uint8 layer) {
	if(m_StateLayerSize <= layer)
		return NULL;
	return &m_StateLayers[layer];
}

bool ISimulationObj::changeState(ISimulationState* targetState, double duration, double eventDt) {
	if(targetState == NULL)
		return false;

	ISimulationObj::stateLayer* layer = getStateLayer(targetState->getLayerId());
	if(!layer || !layer->isActive()) {
		nlwarning("Tried to change the state on an inactive layer %d on the SOB %d", targetState->getLayerId(), getSobId());
		return false;
	}

	ISimulationState* currentState = getState(targetState->getLayerId());

	if(currentState != NULL) {
		// first check if we can transition from the current state to this state.
		if(currentState->canTransition(targetState->getStateId())) {
			// always exit before changing states.
			currentState->exit(this, eventDt);
			setState(targetState);
			targetState->enter(this);
			return true;
		} else {
			nlwarning("Attempted to transition from %d to a non-transitionable state %d.", currentState->getStateId(), targetState->getStateId());
			return false;
		}
	} else {
		layer->CorrectionDt = eventDt;
		setState(targetState);
		targetState->enter(this);
		return true;
	}
}

std::vector<uint32> ISimulationObj::getActiveStateList() {
	std::vector<uint32> list;
	for(uint i = 0; i < m_StateLayerSize; ++i) {
		ISimulationObj::stateLayer* layer = getStateLayer(i);
		if(layer && layer->isActive()) {
			list.push_back(layer->ActiveState.getValue()->getStateId());
		}
	}
	return list;
}

void ISimulationObj::setActiveStateList(std::vector<uint32> *list) {
	if(list->empty())
		return;

	std::vector<uint32>::iterator iter = list->begin();
	for( ; iter != list->end(); ++iter) {
		ISimulationState* state = CStateFactory::instance().getState(*iter);
		setState(state);
	}
}

bool ISimulationObj::addView(IView* view) {
	if(view && view->getViewName()) {
		if(m_Views.find(view->getViewName()) != m_Views.end()) {
			nlwarning("Tried to add an already existing view: %s", view->getViewName());
			return false;
		}
		m_Views[view->getViewName()] = view;
		return true;
	}
	return false;
}

bool ISimulationObj::removeView(const char* name) {
	ISimulationObj::viewMap::iterator iter = m_Views.find(name);
	if(iter != m_Views.end()) {
		delete iter->second;
		m_Views.erase(iter);
		return true;
	}
	nlwarning("Tried to remove a view that does not exist: %s", name);
	return false;
}

IView* ISimulationObj::getView(const char* name) {
	ISimulationObj::viewMap::iterator iter = m_Views.find(name);
	if(iter != m_Views.end()) {
		return iter->second;
	}
	return NULL;
}

bool ISimulationObj::addController(ISobController* controller) {
	if(controller && controller->getControllerName()) {
		if(m_Controllers.find(controller->getControllerName()) != m_Controllers.end()) {
			nlwarning("Tried to add an already existing controller: %s", controller->getControllerName());
			return false;
		}
		m_Controllers[controller->getControllerName()] = controller;
		return true;
	}
	return false;
}

bool ISimulationObj::removeController(const char* name) {
	ISimulationObj::controllerMap::iterator iter = m_Controllers.find(name);
	if(iter != m_Controllers.end()) {
		delete iter->second;
		m_Controllers.erase(iter);
		return true;
	}
	nlwarning("Tried to remove a controller that does not exist: %s", name);
	return false;
}

ISobController* ISimulationObj::getController(const char* name) {
	ISimulationObj::controllerMap::iterator iter = m_Controllers.find(name);
	if(iter != m_Controllers.end()) {
		return iter->second;
	}
	return NULL;
}

bool ISimulationObj::observePreGameEvent(CGameEventServer::EventPtr event) {
	return true;
}

bool ISimulationObj::observeGameEvent(CGameEventServer::EventPtr event) {
	if(event->getId() < ISobEvent::ISobEventID)
		return true; // not a sob event

	NLMISC::CSmartPtr<ISobEvent> sobEvent = dynamic_cast<ISobEvent*>(event.getPtr());
	uint16 sobEventID = ISobEvent::getSobRelativeEventID(sobEvent->getId());
	if(m_HandlerMapSize <= sobEventID)
		return true; // no handler exists
	if(m_HandlerMap[sobEventID] == NULL)
		return true; // no handler exists
	return m_HandlerMap[sobEventID]->handleSobEvent(sobEvent, this);
}

bool ISimulationObj::observePostGameEvent(CGameEventServer::EventPtr event) {
	return true;
}

void ISimulationObj::addHandler(ISobHandler* handler) {
	if(handler == NULL)
		return;

	// TODO henri:henri At the moment handlers are assumed to have static priority hence the map is not reconstructed
	// but new handlers are just added in.
	// this vector is here in case of dynamic priorities and also handler removal so we can rebuild the map.
	m_Handlers.insert(handler);

	ISobHandler::eventList* eventList = handler->getEventList();

	// find the maximum event ID needed for this handler
	uint16 maxEventID = 0;
	ISobHandler::eventList::iterator eventIter = eventList->begin();
	for(;eventIter != eventList->end(); ++eventIter) {
		if(ISobEvent::getSobRelativeEventID(*eventIter) > maxEventID)
			maxEventID = ISobEvent::getSobRelativeEventID(*eventIter);
	}

	// resize the handler map
	if(m_HandlerMapSize <= maxEventID) {
		uint16 newSize = maxEventID + 1;
		ISobHandler** newHandlerMap = new ISobHandler*[newSize];

		// fill the new elements with NULL
		memset(&newHandlerMap[m_HandlerMapSize], 0, (newSize - m_HandlerMapSize)*sizeof(ISobHandler*));
		
		// copy the old elements
		if(m_HandlerMapSize > 0) {
			memcpy(newHandlerMap, m_HandlerMap, m_HandlerMapSize*sizeof(ISobHandler*));
			delete[] m_HandlerMap;
		}

		m_HandlerMap = newHandlerMap;
		m_HandlerMapSize = newSize;
	}

	eventIter = eventList->begin();
	for(;eventIter != eventList->end(); ++eventIter) {
		uint16 sobEventID = ISobEvent::getSobRelativeEventID(*eventIter);
		if(m_HandlerMap[sobEventID] == NULL) {
			m_HandlerMap[sobEventID] = handler;
			continue;
		}
		if(m_HandlerMap[sobEventID]->getPriority() < handler->getPriority()) {
			m_HandlerMap[sobEventID] = handler;
			continue;
		}
	}
}

ISobHandler* ISimulationObj::getHandler(uint16 eventID) {
	if(m_HandlerMapSize <= ISobEvent::getSobRelativeEventID(eventID))
		return NULL;
	return m_HandlerMap[ISobEvent::getSobRelativeEventID(eventID)];
}

}; // END OF NAMESPACE WWCOMMON