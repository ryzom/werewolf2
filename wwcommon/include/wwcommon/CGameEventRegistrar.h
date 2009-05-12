/**
 * \file CGameEventRegistrar.h
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

#ifndef __CGAMEEVENTREGISTRAR_H__
#define __CGAMEEVENTREGISTRAR_H__

//
// Standard Includes
//
#include <string>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/stream.h>

//
// Werewolf Includes
//	
#include "CGameEventServer.h"

//
// Namespaces
//

namespace WWCOMMON {

class CGameEventServer;

/// This macro is used in the header to setup the registrar object.
#define WWCOMMON_SETUP_EVENT(EVENT) \
	const static CGameEventRegistrar<EVENT> registrar;

/// This macro is used in the source to initialize the registrar object, registering the DO object.
#define WWCOMMON_REGISTER_EVENT(EVENT) \
	const static CGameEventRegistrar<EVENT> registrar(#EVENT);

template<class EventClass>
class CGameEventRegistrar {
public:

	CGameEventRegistrar(const std::string& EventClassName) {
		CGameEventServer::instance().Register(RegisterEventType, EventClassName);
	}

	static bool RegisterEventType(const std::string& EventClassName) {
		NLMISC::CClassRegistry::registerClass(EventClassName, EventClass::creator, typeid(EventClass).name());
		return true;
	}
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CGAMEEVENTREGISTRAR_H__