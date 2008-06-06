/**
 * \file WHATEVER.EXTENSION
 * \date February 2005
 * \author Henri Kuuste
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

#ifndef __CCHARACTERMANAGER_H__
#define __CCHARACTERMANAGER_H__

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

//
// Werewolf Includes
//	
#include "../Shared/ISingleton.h"

//
// Namespaces
//

class CCharacterManager : public ISingleton<CCharacterManager> {
public:
	/// Defines characters. This will be in the DB in the game/demo.
	struct TCharacterData {
		/// Default value population ctor.
		TCharacterData(std::string nm, uint32 eid, uint uid) : name(nm), objectID(eid), userID(uid), online(false) { }
		std::string name;	/*!< The name of the character. */
		uint32 objectID;	/*!< The ID of this character. */
		uint32 userID;		/*!< The ID of the user who owns this character. */
		bool online;
	};

	/// Used as a return type for a UID->Character matcher.
	typedef std::vector<std::string> TCharacterStrings;
	/// Defines the type of the list for characters.
	typedef std::list<TCharacterData> TCharacterMap;
	
	/// Initialize the character manager - creates a static list for a test.
	void init();

	/// Find out if a character is online.
	bool isOnline(uint32 objectID);
	/// Flag someone as online
	bool setOnline(uint32 objectID);
	/// Flag someone as offline
	bool setOffline(uint32 objectID);

	/// Returns a std::vector of character names available to the specified uid.
	TCharacterStrings sendCharacterList(uint32 uid);
	const TCharacterMap sendCharacterMap();
private:
	/// A list of characters a client can connect as.
	TCharacterMap m_CharacterList;
};

#endif // __CCHARACTERMANAGER_H__