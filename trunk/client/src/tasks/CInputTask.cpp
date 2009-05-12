/**
 * \file CInputTask.cpp
 * \date May 2004
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
// System Includes
//
#include "stdpch.h"

//
// NeL Includes
//
#include <nel/misc/hierarchical_timer.h>

//
// Werewolf Includes
//
#include "tasks/CInputTask.h"
#include "tasks/C3DTask.h"
#include <wwcommon/CTaskManager.h>

//
// Class
//

namespace WWCLIENT {

void CInputTask::init() {
	// reset the key listener, we probably don't care about key strokes
	// that happened before this was started.
	C3DTask::instance().driver().AsyncListener.reset();
}

void CInputTask::update() {
		if(kbPressed(NLMISC::KeyF5)) {
			WWCOMMON::CTaskManager::instance().benchmark(true);
		}
		if(kbPressed(NLMISC::KeyB)) {
			NLMISC::CHTimer::display();
		}
}

void CInputTask::render() {
	;
}

void CInputTask::release() {
	;
}

std::string CInputTask::name() {
	return "CInputTask";
}

bool CInputTask::kbPressed(NLMISC::TKey key) const {
	return C3DTask::instance().driver().AsyncListener.isKeyPushed(key);
}

bool CInputTask::kbDown(NLMISC::TKey key) const {
	return C3DTask::instance().driver().AsyncListener.isKeyDown(key);
}

}; // END NAMESPACE WWCLIENT
