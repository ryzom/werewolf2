/**
 * \file IController.h
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

#ifndef __ICONTROLLER_H__
#define __ICONTROLLER_H__

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
#include "../Shared/CEntity.h"
#include "../Shared/CMsgEntity.h"

//
// Namespaces
//

class IController {
public:
	/// Process any pending updates.
	virtual void update()=0;
	/// Receive incoming messages.
	virtual bool recv(CMsgEntity msgin)=0;
	/// Return a new controller for the factory.
	static IController *Create(CEntity *entity) { return NULL; }
protected:
	/// Protected ctor, enforces use of Create.
	IController() { ; }

};

#endif // __ICONTROLLER_H__
