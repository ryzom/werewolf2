/**
 * \file CUserManager.cpp
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
#include "CUserManager.h"
#include "CFrontendService.h"

#include <mysql.h>

//
// Namespaces
//

void CUserManager::init() {
	; // no logic here anymore.
}


CUser *CUserManager::getUser(uint32 uid) {
	CUser *user = NULL;
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery(
		"SELECT userid,plrallowed,state FROM users WHERE userid=" +
		NLMISC::toString(uid), nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return player;
	}

	if(nbrow == 0) {
		nlinfo("No user found for id: %d", uid);
		user=getNewUser();
		//player = new CPlayer(0, uid, 0);
		//savePlayer(player);
		return user;
	}

	uint32 uid;
	NLMISC::fromString(row[0], uid);
	NLMISC::fromString(row[1], plrallowed);
//	NLMISC::fromString(row[2], online);
			
	user = new CUser(uid,plrallowed)
		
	return user;
}

bool CPlayerManager::savePlayer(CPlayer *plr) {
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery(
			"SELECT count(*) FROM users WHERE userid=" +
			NLMISC::toString(plr->UserID) + " AND active=0", nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());
		return false;
	}
	
	// Retrieve the number of matches for this player's userid. Should be 1 or 0.
	uint32 userCount;
	NLMISC::fromString(row[0], userCount);

	if(userCount != 1) {
		nlinfo("Player not found in database. Save it.");
	} else {
		nlinfo("Player already in database. Update it.");
	}
}





const CPlayerManager::TCharacterList CPlayerManager::getListForPlayer(uint32 id) {
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

bool CPlayerManager::isPlayers(uint32 objid, uint32 id) {
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

bool CPlayerManager::isOnline(uint32 objectID) {
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

bool CPlayerManager::setOnline(uint32 objectID) {
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

bool CPlayerManager::setOffline(uint32 objectID) {
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
