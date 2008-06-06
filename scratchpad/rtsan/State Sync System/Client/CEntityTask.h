/**
 * \file CEntityTask.h
 * \date February 2005
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

#ifndef __CENTITYTASK_H__
#define __CENTITYTASK_H__

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
#ifdef NL_OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif
#include <nel/net/message.h> 

//
// Werewolf Includes
//	
//#include "../Shared/CState.h"
#include "../Shared/ISingleton.h"
#include "../Shared/CEntityMap.h"
#include "../Shared/CMsgEntity.h"



//
// Namespaces
//

/// This task functions for the sole purpose of directing messages to controllers and sending updates to the map.
class CEntityTask : public ISingleton<CEntityTask> {
public:
	void init();
	void update();

	void recvMessage(CMsgEntity msgent);

	static const uint8 EID_NONE = 0;

private:
	TEntityData		*m_Self;
};

#endif // __CENTITYTASK_H__