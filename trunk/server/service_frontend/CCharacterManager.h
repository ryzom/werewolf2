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
#include <nel/misc/stream.h>

//
// Werewolf Includes
//	
#include "wwcommon/ISingleton.h"
#include "wwcommon/CCharacterData.h"

//
// Namespaces
//

class CCharacterManager : public WWCOMMON::ISingleton<CCharacterManager> {
public:
	/// Used as a return type for a UID->Character matcher.
	typedef std::vector<std::string> TCharacterStrings;
	/// Defines the type of the list for characters.
	typedef std::vector<WWCOMMON::CCharacterData> TCharacterList;
	
	/// Initialize the character manager - creates a static list for a test.
	void init();

	/// Find out if a character is online.
	bool isOnline(uint32 objectID);
	/// Flag someone as online
	bool setOnline(uint32 objectID);
	/// Flag someone as offline
	bool setOffline(uint32 objectID);

	/// Find out if a character objId belongs to a player.
	bool isPlayers(uint32 objid, uint32 id);

	/// Returns a std::vector of character names available to the specified uid.
	const TCharacterList getListForPlayer(uint32 id);

	/// Adds a new character and returns an updated CCharacterData object with the character ID populated.
	WWCOMMON::CCharacterData *addCharacter(WWCOMMON::CCharacterData *charData);

	/// Retrieve a character object by Character ID.
	WWCOMMON::CCharacterData *getCharacter(uint32 id);
private:

};

#endif // __CCHARACTERMANAGER_H__

