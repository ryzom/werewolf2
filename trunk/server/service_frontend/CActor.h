/**
 * \file CActor.h
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

#ifndef __CACTOR_H__
#define __CACTOR_H__

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
#include "wwcommon/CPerformer.h"
#include "wwcommon/ISimulationObj.h"
#include "wwcommon/IController.h"
#include "wwcommon/IObserver.h"
#include "wwcommon/CSobFactory.h"
#include "wwcommon/CFactoryRegistrar.h"

//
// Namespaces
//

class CActor : public WWCOMMON::CPerformer {
	OF_SETUP_REGISTRAR(WWCOMMON::CSobFactory, WWCOMMON::ISimulationObj, CActor, std::string);
public:
	static WWCOMMON::ISimulationObj *Create();

	uint32 getPlayerId() { return m_PlrId; }

	void update();

protected:
	friend class CServerSimulation;

	uint32 m_PlrId;
	CActor();
};

#endif // __CACTOR_H__
