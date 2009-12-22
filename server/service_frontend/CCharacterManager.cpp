/**
 * \file CCharacterManager.cpp
 * \date April 2005
 * \author Henri Kuuste
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
// Standard Includes
//
#include "stdpch.h"
#include <vector>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>
#include <nel/misc/stream.h>

//
// Werewolf Includes
//	
#include "CCharacterManager.h"
#include "CFrontendService.h"

#include <mysql.h>

//
// Namespaces
//

void CCharacterManager::init() {
	; // no logic here anymore.
}

const CCharacterManager::TCharacterList CCharacterManager::getListForPlayer(uint32 id) {
	TCharacterList tmpList;
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;


	reason = sqlQuery(
		"SELECT characterid,userid,charname,active,emdtype FROM characters WHERE userid=" +
		NLMISC::toString(id) + " AND active=0", nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return tmpList;
	}

	if(nbrow == 0) {
		nlinfo("No characters found for id: %d", id);
		return tmpList;
	}

	while(row != 0) {
		WWCOMMON::CCharacterData character;
		NLMISC::fromString(row[0], character.CharacterID);
		NLMISC::fromString(row[1], character.UserID);
		character.Name=row[2];
		NLMISC::fromString(row[3], character.Online);
		character.EmdType=row[4];
		
		
		nlinfo("Pushing back %s to character list for %d", character.Name.c_str(), id);
		tmpList.push_back( character );
		
		row = mysql_fetch_row(result);
	}

	return tmpList;
}

bool CCharacterManager::isPlayers(uint32 objid, uint32 id) {
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery(
		"SELECT characterid,userid FROM characters WHERE userid=" + NLMISC::toString(id) + 
		" AND characterid=" + NLMISC::toString(objid), nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return false;
	}

	if(nbrow == 0) {
		nlinfo("No characters found for id: %d", id);
		return false;
	}

	if(nbrow == 1)
		return true;
	
	return false;
}

bool CCharacterManager::isOnline(uint32 objectID) {
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery( "SELECT active FROM characters WHERE characterid=" + NLMISC::toString(objectID), 
		nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return false;
	}

	if(nbrow == 0) {
		nlinfo("No character found for character id: %d", objectID);
		return false;
	}

	if(nbrow == 1)
		return true;

	return false;
}

bool CCharacterManager::setOnline(uint32 objectID) {
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;
	std::string reason=sqlQuery(
		"UPDATE characters SET active=1 WHERE characterid="+NLMISC::toString(objectID), nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return false;
	}

	return true;
}

bool CCharacterManager::setOffline(uint32 objectID) {
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;
	std::string reason=sqlQuery(
		"UPDATE characters SET active=0 WHERE characterid="+NLMISC::toString(objectID), nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return false;
	}

	return true;
}

WWCOMMON::CCharacterData *CCharacterManager::addCharacter(WWCOMMON::CCharacterData *charData) {
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;
	std::string qry = "INSERT INTO characters (userid,charname,active,emdtype) values (" +
		NLMISC::toString(charData->UserID) + "," +
		"'"+charData->Name+"'," +
		NLMISC::toString((uint32)(0)) + "," +
		"'"+charData->EmdType+"')";

	std::string reason=sqlQuery(qry , nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return false;
	}

	qry = "SELECT MAX(characterid) FROM characters WHERE userid="
		+NLMISC::toString(charData->UserID)+" AND charname='"
		+charData->Name+"'";

	reason = sqlQuery( qry , nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return false;
	}

	if(nbrow != 1) {
		nlwarning("Unable to find the new character we added!");
		return NULL;
	}

	// Map the character ID we retrieved:
	NLMISC::fromString(row[0], charData->CharacterID);

	return charData;

}

WWCOMMON::CCharacterData *CCharacterManager::getCharacter(uint32 id) {
	WWCOMMON::CCharacterData *character = new WWCOMMON::CCharacterData();
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;


	reason = sqlQuery(
		"SELECT characterid,userid,charname,active,emdtype FROM characters WHERE characterid=" +
		NLMISC::toString(id), nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return NULL; // TODO throw an exception instead?
	}

	if(nbrow == 0) {
		nlinfo("No characters found for id: %d", id);
		return NULL; // TODO throw an exception instead?

	}

	if(nbrow > 1) {
		nlinfo("Too many characters found: %d", nbrow);
		return NULL;
	}

	NLMISC::fromString(row[0], character->CharacterID);
	NLMISC::fromString(row[1], character->UserID);
	character->Name=row[2];
	NLMISC::fromString(row[3], character->Online);
	character->EmdType=row[4];
		
		
	nlinfo("Pushing back %s to character list for %d", character->Name.c_str(), id);

	return character;
}