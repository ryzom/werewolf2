/**
 * \file CRequestHandler.h
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

#ifndef __CREQUESTHANDLER_H__
#define __CRequestHANDLER_H__

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
#include "ISobHandler.h"
#include "CSobStateRequestEvent.h"
#include "CSobDREvent.h"
#include "CSobHandlerFactory.h"
#include "CFactoryRegistrar.h"

//
// Namespaces
//
namespace WWCOMMON {

class CRequestHandler : public ISobHandler {
	OF_SETUP_REGISTRAR(CSobHandlerFactory, ISobHandler, CRequestHandler, uint32);
public:
	CRequestHandler();
	virtual ~CRequestHandler();

	eventList* getEventList();
	bool handleSobEvent(NLMISC::CSmartPtr<ISobEvent> event, ISimulationObj* subject);
private:
	bool handleStateRequestEvent(CSobStateRequestEvent& event, ISimulationObj* subject);
protected:
	eventList m_HandledEvents;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CREQUESTHANDLER_H__