/**
 * \file CActorController.cpp
 * \date March 2005
 * \author Henri Kuuste
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

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include "../Shared/IController.h"
#include "../Shared/CControllerFactory.h"
#include "CActorController.h"


//
// Namespaces
//

namespace {
	const bool unused=CControllerFactory::instance().registerController(CActorController::Create, "actorController");
};

void CActorController::update() {
	; // no logic yet.
}

bool CActorController::recv(NLNET::CMessage msgin) {
	return false; // no logic yet.
}

IController *CActorController::Create(CEntity *entity) { 
	return new CActorController(entity);
}

CActorController::CActorController(CEntity *entity) {
	m_Controlled=entity;
}

