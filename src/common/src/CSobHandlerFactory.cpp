/**
 * \file CSobHandlerFactory.cpp
 * \date February 2006
 * \author Matt Raykowski
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
#include "CSobHandlerFactory.h"

//
// Namespaces
//

namespace WWCOMMON {

/// Register a state type
bool CSobHandlerFactory::Register(uint32 type, CSobHandlerFactory::CreateHandler create) {
	return m_HandlerMap.insert( THandlerMap::value_type(type, create())).second;
}

ISobHandler *CSobHandlerFactory::getHandler(uint32 handlerId) {
	// Find the controller by id, make sure we can find it.
	THandlerMap::const_iterator i = m_HandlerMap.find(handlerId);
	if( i == m_HandlerMap.end()) {
		nlwarning("SOB handler object creation failed: %d not found!", handlerId);
		return NULL;
	};

	return (i->second);
}

}; // END OF NAMESPACE WWCOMMON
