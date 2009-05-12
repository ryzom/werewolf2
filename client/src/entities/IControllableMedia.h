/**
 * \file IControllableMedia.h
 * \date July 2006
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

#ifndef __ICONTROLLABLEMEDIA_H__
#define __ICONTROLLABLEMEDIA_H__

//
// Standard Includes
//

//
// System Includes
//
#include <hash_map>

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include "entities/IMediaController.h"
#include <wwcommon/IDirtyPropagator.h>
#include <wwcommon/general.h>

//
// Namespaces
//

namespace WWCLIENT {

class IControllableMedia : public WWCOMMON::IDirtyPropagator {
public:
	typedef std::hash_map<const char*, IMediaController*, std::hash<const char*>, streqpred> mediaControllerMap;

	virtual void update() = 0;
	void updateControllers();

	void addController(IMediaController* controller);
	void removeController(const char* name);

protected:
	mediaControllerMap m_Controllers;
};

}; // END NAMESPACE WWCLIENT

#endif // __ICONTROLLABLEMEDIA_H__
