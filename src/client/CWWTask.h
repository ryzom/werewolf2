/**
 * \file CWWTask.h
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

#ifndef __CWWTASK_H__
#define __CWWTASK_H__

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "controllers/CInputController.h"

#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Class
//

namespace WWCLIENT {

/**
 * \class CWWTask CWWTask.h "CWWTask.h"
 * \brief The main Werewolf task.
 *
 * This is the main Werewolf task. It handles some of the high level
 * state management and the pre-GUI input controller. It starts up
 * most of the core task managers.
 */
class CWWTask : public WWCOMMON::ISingleton<CWWTask>, public WWCOMMON::ITask {
public:
	virtual void init();
	virtual void update();
	virtual void render() { }
	virtual void release() { }

	void reset();
	void error(std::string reason);
	
	virtual std::string name();
	CInputController &controller() const;
	enum TState { eLoginPassword, eConnect, eBeforeFirstSession, eStartSession, eReady, eGame, eEndSession };

	TState State;

private:
	bool m_DoError;
	CInputController *m_Controller;
	void m_error();
	std::string m_ErrorReason;
};

}; // END NAMESPACE WWCLIENT

#endif // __CWWTASK_H__
