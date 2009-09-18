/**
 * \file CUserManager.h 
 * \date September 2009
 * \author Matt Raykowski
 */

/* Copyright, 2009 Werewolf
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

#ifndef __CUSERMANAGER_H__
#define __CUSERMANAGER_H__

//
// Standard Includes
//
#include <string>
#include <list>
#include <vector>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/stream.h>

//
// Werewolf Includes
//	
#include "wwcommon/ISingleton.h"

//
// Namespaces
//

/**
 * \brief Contains network-level user information.
 */
struct CUser {
	/// Data-filling constructor.
	CUser(uint32 uid, uint32 plrallowed) : UserID(uid), CharactersAllowed(plrallowed) { };

	/// Default destructor.
	~CUser() { };

	/// The userid of this player.
	uint32 UserID;

	/// The Simulation Object ID that this player is controlling.
	uint32 CharactersAllowed;
};

class CUserManager : public WWCOMMON::ISingleton<CUserManager> {
public:
	/// Initialize the user manager.
	void init();

	/**
	 * \brief Get a user.
	 *
	 * \param id The UserID to find.
	 * \return The CUser the id represents.
	 */
	CUser *getUser(uint32 uid);

	/**
	 * \brief Creates a new user object.
	 *
	 * \return The new CUser generated by the database.
	 */
	CUser *getNewUser();

	/**
	 * \brief Save a user to the users table.
	 *
	 * \param user The user to be saved/updated.
	 * \return Whether the user was saved or not.
	 */
	bool saveUser(CUser *user);
private:

};

#endif // __CUSERMANAGER_H__

