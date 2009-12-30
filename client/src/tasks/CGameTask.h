/**
 * \file CGameTask.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Some code adapted from MTP-Target.
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

#ifndef __CGAMETASK_H__
#define __CGAMETASK_H__

//
// System Includes
//
#include "CEGUI.h"

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "controllers/CPlayerController.h"

#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"
#include <wwcommon/CGameChatEvent.h>

//
// Namespaces
//

namespace WWCLIENT {

//
// Class
//

/**
 * \class CGameTask CGameTask.h "tasks\CGameTask.h"
 * \brief The core game logic task.
 */
class CGameTask : public WWCOMMON::ISingleton<CGameTask>, public WWCOMMON::ITask {
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual void stop();
	
	virtual std::string name() { return "CGameTask"; }
	
	// network related calls
	void recvChat(std::string chatmsg);
	void recvChat(WWCOMMON::CGameChatEvent *event);
	void parseConsole(std::string chatmsg);

private:
	CPlayerController	m_Controller;

	CEGUI::DefaultWindow *m_FpsCounter;
	CEGUI::DefaultWindow *m_Location;
	CEGUI::DefaultWindow *m_Zone;
};

}; // END NAMESPACE WWCLIENT

#endif // __CGAMETASK_H__
