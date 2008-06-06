/**
 * \file CInputController.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Some code adapted from MTP-Target.
 * \todo Seems like there should be some logic in the destructor.
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

#ifndef __CINPUTCONTROLLER_H__
#define __CINPUTCONTROLLER_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/vector.h>

//
// Werewolf Includes
//

//
// Class
//

namespace WWCLIENT {

/**
 * \class CInputController CInputController.h "controllers/CInputController.h"
 * \brief This controller handles system-level input response.
 *
 * The input controller queries the CInputTask for key presses so that it can
 * act appropriately to system level tasks. This is typically things like 
 * catching that the escape key was hit and signalling the task manager to quit.
 * It should only be instantiated by the main engine task, CWWTask.
 */
class CInputController {
public:
	/**
	 * \brief Default Constructor, calls reset()
	 *
	 * All this constructor does is call the reset() method to initialize
	 * the controller's default settings.
	 *
	 * \see reset()
	 */
	CInputController();
	/**
	 * \brief Default destructor.
	 *
	 * This default destructor does nothing.
	 */
	~CInputController();

	/**
	 * \brief Standard update and refresh cycle.
	 *
	 * This method goes through the system and updates entity acceleration,
	 * updates some time frames, and processes any keys thrown by C3DTask.
	 */
	void	update();

	/**
	 * \brief Resets the controller to it's default state.
	 * 
	 * This sets all of the internal private members to their default
	 * states. Typically the beginning of a game or scene.
	 */
	void	reset();

private:
	NLMISC::CVector m_Accel;
	float						Pique;
	float						RotZ;
	float						m_VirtualFrameTime;
	/**
	 * \brief Flag whether it's the first frame or not.
	 *
	 * When set to true this is the first frame of animation being processed
	 * and when false it's a game in progress. reset() flags this as true,
	 * and update() flags this as false and resets the Acceleration and frame
	 * times.
	 *
	 * \see reset()
	 * \see update()
	 */
	bool						m_FirstFrame;
	float						m_MissedTime;
};

}; // END NAMESPACE WWCLIENT

#endif // __CINPUTCONTROLLER_H__