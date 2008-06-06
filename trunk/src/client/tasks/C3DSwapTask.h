/**
 * \file C3DSwapTask.h
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

#ifndef __C3DSWAPTASK_H__
#define __C3DSWAPTASK_H__

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Class
//

namespace WWCLIENT {

class C3DSwapTask : public WWCOMMON::ISingleton<C3DSwapTask>, public WWCOMMON::ITask {
public:
	/**
	 * \brief Initializes swapping subsystem.
	 *
	 * This initializes the 3D swapper. This method simple sets a callback for
	 * the "Sleep" parameter in the config file. A special note must be made that
	 * the render() method also handles creating screenshots if the F2 key is
	 * pressed between cycles.
	 * @see CConfigTask
	 */
	virtual void init();

	/**
	 * \brief From ITask, does nothing.
	 */
	virtual void update();

	/**
	 * \brief Swaps the buffers, sleeps, and spits out screenshots.
	 *
	 * This method swaps the buffers, checks C3DTask to see if a screenshot
	 * is necessary, and then sleeps for the time defined in init().
	 * @see init()
	 * @see C3DTask
	 */
	virtual void render();

	/**
	 * \brief From ITask, does nothing.
	 */
	virtual void release();

	/**
	 * \brief Returns the name of the task.
	 */
	virtual std::string name();

private:
	sint32	m_SleepTime;
};

}; // END NAMESPACE WWCLIENT

#endif // __C3DSWAPTASK_H__
