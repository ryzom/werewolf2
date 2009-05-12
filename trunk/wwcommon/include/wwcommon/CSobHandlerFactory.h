/**
 * \file CSobHandlerFactory.h
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

#ifndef __CSOBHANDLERFACTORY_H__
#define __CSOBHANDLERFACTORY_H__

//
// Standard Includes
//

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
#include "ISingleton.h"

//
// Namespaces
//

namespace WWCOMMON {

// forward declaration to avoid looped includes.
class ISobHandler;

class CSobHandlerFactory : public ISingleton<CSobHandlerFactory> {
public:
	enum EHandler {
		H_MOTION,
		H_CLIENT_MOTION,
		H_MOTION_REQUEST
	};

	typedef ISobHandler* (*CreateHandler)();
	bool Register(uint32 type, CreateHandler create);
	ISobHandler *getHandler(uint32 handlerId);

protected:
	typedef CHashMap<uint32, ISobHandler *> THandlerMap;
	THandlerMap m_HandlerMap;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CSOBHANDLERFACTORY_H__
