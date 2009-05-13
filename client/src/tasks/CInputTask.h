/**
 * \file CInputTask.h
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

#ifndef __CINPUTTASK_H__
#define __CINPUTTASK_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/events.h>

//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Class
//

namespace WWCLIENT {

/**
 * \class CInputTask CInputTask.h "tasks/CInputTask.h"
 * \brief A task that provides input polling and event registration.
 *
 * This class performs input polling for things such as the keyboard
 * and mouse. It relies upon the ISubject and IObserver interfaces to
 * facilitate registration and updating.
 */
class CInputTask : public WWCOMMON::ISingleton<CInputTask>, public WWCOMMON::ITask {
public:
	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();
	virtual std::string name();

	/**
	 * \brief Determines whether a specified key was pressed.
	 *
	 * When requested it will return whether or not a certain key has
	 * been pressed. This is dependent on whether it has been pushed since
	 * the last time this method was called. Each time this method is called
	 * the push-state is reset.
	 *
	 * @param key A NLMISC::TKey specified key, eg. KeyESCAPE
	 * @return bool, true if pressed, false if not.
	 */
	bool				kbPressed(NLMISC::TKey key) const;

	/**
	 * \brief Determines whether a specified key is pressed.
	 *
	 * Will return true or false depending on whether the key requested is
	 * currently pressed.
	 *
	 * @param key A NLMISC::TKey specified key, eg. KeyESCAPE
	 * @return bool, true if it is pressed, false if not.
	 */
	bool				kbDown(NLMISC::TKey key) const;

private:

};

}; // END NAMESPACE WWCLIENT

#endif // __CINPUTTASK_H__
