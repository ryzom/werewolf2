/**
 * \file CStateManager.cpp
 * \date May 2005
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

//
// Standard Includes
//
#include "stdpch.h"
#include <fstream>
#include <iostream>

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
#include "wwcommon/CStateManager.h"

//
// Namespaces
//

namespace WWCOMMON {

CStateFactory::CStateFactory() {
	// TODO henri:everyone find a better way to initialize the defaults map.
	m_DefaultMap = new uint32[ST_LAYER_END];
	m_DefaultMap[ST_MOVEMENT]	= ST_MV_IDLE;
	m_DefaultMap[ST_POSTURE]	= ST_PS_STANDING;
	m_DefaultMap[ST_ACTION]		= ST_AC_IDLE;
	m_DefaultMap[ST_STRAFE]		= ST_ST_IDLE;
	m_DefaultMap[ST_ORIENTATION]= ST_OR_IDLE;
}

CStateFactory::~CStateFactory() {
	delete[] m_DefaultMap;
}

/// Register a state type
bool CStateFactory::Register(uint32 type, CreateState create) {

	return m_StateMap.insert( TStateMap::value_type(type, create())).second;
}

ISimulationState *CStateFactory::getState(uint32 stateId) {
	// Find the controller by id, make sure we can find it.
	TStateMap::const_iterator i = m_StateMap.find(stateId);
	if( i == m_StateMap.end()) {
		nlwarning("Simulation State object creation failed: %d not found!", stateId);
		return NULL;
	};

	return (i->second);
}

ISimulationState *CStateFactory::getDefault(uint8 layerId) {
	if(layerId >= ST_LAYER_END)
		return NULL;
	return getState(m_DefaultMap[layerId]);
}

}; // END OF NAMESPACE WWCOMMON
