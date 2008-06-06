/**
 * \file CActorController.h
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

#ifndef __CACTORCONTROLLER_H__
#define __CACTORCONTROLLER_H__

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
#include "../Shared/CMsgEntity.h"

//
// Namespaces
//

class CActorController : public IController {
public:
	void update();

	bool recv(CMsgEntity msgin);

	static IController *Create(CEntity *entity);

protected:
	CActorController(CEntity *entity);

	CEntity *m_Controlled;
};

#endif // __CACTORCONTROLLER_H__
