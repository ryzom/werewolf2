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

//
// Standard Includes
//
#include <vector>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>

//
// Werewolf Includes
//	
#include "CCharacterManager.h"

//
// Namespaces
//

void CCharacterManager::init() {
	// Add some default characters.
	m_CharacterList.push_back(TCharacterData("testuser1",1,0));
	m_CharacterList.push_back(TCharacterData("testuser2",2,0));
	m_CharacterList.push_back(TCharacterData("testuser3",3,1));
}

CCharacterManager::TCharacterStrings CCharacterManager::sendCharacterList(uint32 uid) {
	TCharacterStrings tmpList;
	TCharacterMap::iterator itr=m_CharacterList.begin();
	while( itr != m_CharacterList.end() ) {
		if( (*itr).userID == uid ) {
			nlinfo("Pushing back %s to character list for %d", (*itr).name.c_str(), uid);
			tmpList.push_back( (*itr).name );
		}
		itr++;
	}

	return tmpList;
	//if( itr == m_CharacterList.end() ) {
	//	nlwarning("Unable to find entity ID %d. Entity not spawned.", entityID);
	//	return false;
	//}
}

const CCharacterManager::TCharacterMap CCharacterManager::sendCharacterMap() {
	return m_CharacterList;
}


bool CCharacterManager::isOnline(uint32 objectID) {
	TCharacterMap::iterator itr=m_CharacterList.begin();
	while( itr != m_CharacterList.end() ) {
		if( (*itr).objectID == objectID )
			return (*itr).online;
		itr++;
	}
	nlwarning("Unable to find character %d in database.",objectID);
	return false;
}

bool CCharacterManager::setOnline(uint32 objectID) {
	TCharacterMap::iterator itr=m_CharacterList.begin();
	while( itr != m_CharacterList.end() ) {
		if( (*itr).objectID == objectID ) {
			(*itr).online=true;
			return true;
		}
		itr++;
	}
	nlwarning("Unable to find character %d in database.",objectID);
	return false;
}

bool CCharacterManager::setOffline(uint32 objectID) {
	TCharacterMap::iterator itr=m_CharacterList.begin();
	while( itr != m_CharacterList.end() ) {
		if( (*itr).objectID == objectID ) {
			(*itr).online=false;
			return false;
		}
		itr++;
	}
	nlwarning("Unable to find character %d in database.",objectID);
	return false;
}
