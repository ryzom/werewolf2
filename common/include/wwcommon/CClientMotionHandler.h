/**
 * \file CClientMotionHandler.h
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

#ifndef __CCLIENTMOTIONHANDLER_H__
#define __CCLIENTMOTIONHANDLER_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "CMotionHandler.h"
#include "CClientMotionHandler.h"
#include "CFactoryRegistrar.h"
#include "CSobDREvent.h"

//
// Namespaces
//
namespace WWCOMMON {

class CClientMotionHandler : public CMotionHandler {
	OF_SETUP_REGISTRAR(CSobHandlerFactory, ISobHandler, CClientMotionHandler, uint32);
public:
	CClientMotionHandler();

	bool handleSobEvent(NLMISC::CSmartPtr<ISobEvent> event, ISimulationObj* subject);

protected:
	void handleDREvent(CSobDREvent* event, CPerformer* subject);
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CCLIENTMOTIONHANDLER_H__