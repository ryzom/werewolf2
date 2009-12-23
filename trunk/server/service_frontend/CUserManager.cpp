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

CUser *CUserManager::getUserFromList(uint32 uid) {
	TUserList::iterator itr = m_Users.begin();
	while(itr != m_Users.end()) {
		CUser *user = (*itr);
		if(user->UserID == uid) {
			return user;
		}
		itr++;
	}
	return NULL;
}

bool CUserManager::isUserInList(uint32 uid) {
	TUserList::iterator itr = m_Users.begin();
	while(itr != m_Users.end()) {
		CUser *user = (*itr);
		if(user->UserID == uid) {
			return true;
		}
		itr++;
	}
	return false;
}

CUser *CUserManager::getUserById(uint32 uid) {
	CUser *user = NULL;

	// Try to get one that we've loaded already.
	user = getUserFromList(uid);
	
	// If we didn't find it in the list...
	if(user == NULL) {
		user = getUserFromDatabase(uid);

		// If we found the user in the database add it to the list.	
		if(user != NULL)
			m_Users.push_back(user);
	}

	// Return the user object.
	return user;
}

CUser *CUserManager::getUserBySobId(uint32 sobid) {
	CUser *user = NULL;

	TUserList::iterator itr = m_Users.begin();
	while(itr != m_Users.end()) {
		CUser *user = (*itr);
		if(user->SobID == sobid) {
			return user;
		}
		itr++;
	}

	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery(
		"SELECT userid FROM users WHERE sobid=" +
		NLMISC::toString(sobid), nbrow, row, result);

	if(!reason.empty()) {
		nlwarning("SQL Query Failed: %s", reason.c_str());	
		return user;
	}

	if(nbrow != 1) {
		nlwarning("No user found for using SobID: %d", sobid);
		return user;
	}

	uint32 uid;
	NLMISC::fromString(row[0], uid);
	return getUserById(uid);
}
	

CUser *CUserManager::getUserFromDatabase(uint32 uid) {
	CUser *user = NULL;
	CFrontendService *fs=(CFrontendService *)NLNET::IService::getInstance();
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery(
		"SELECT userid,plrallowed,state,sobid,characterid FROM users WHERE userid=" +
		NLMISC::toString(uid), nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());	
		return user;
	}

	if(nbrow == 0) {
		nlinfo("No user found for id: %d", uid);
		// TODO drive defaults like plrallowed off of config file or shard-wide settings.
		user = new CUser(uid, USERMANAGER_DEFAULT_MAX_CHARACTERS);
		saveUser(user);
		return user;
	}

	uint32 plrallowed;
	std::string userStateStr = row[2];
	NLMISC::fromString(row[1], plrallowed);


	user = new CUser(uid,plrallowed,CUser::convertUserStateFromString(row[2]));
		
	return user;
}

bool CUserManager::saveUser(CUser *user) {
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery(
			"SELECT count(*) FROM users WHERE userid=" +
			NLMISC::toString(user->UserID), nbrow, row, result);

	if(!reason.empty()) {
		nlinfo("SQL Query Failed: %s", reason.c_str());
		return false;
	}
	
	// Retrieve the number of matches for this player's userid. Should be 1 or 0.
	uint32 userCount;
	NLMISC::fromString(row[0], userCount);

	if(userCount != 1) {
		nlinfo("User not found in database. Save it.");
		std::string query = "INSERT INTO users (userid, plrallowed, state) values (" + 
			NLMISC::toString(user->UserID) + 
			", " + NLMISC::toString(user->CharactersAllowed) +", '"+ 
			CUser::convertUserStateToString(user->UserState)+"')";
		reason = sqlQuery(query, nbrow, row, result);
		if(!reason.empty()) {
			nlwarning("Failed to create new user entry! Reason: %s", reason.c_str());
			return false;
		}
	} else {
		nlinfo("User already in database. Update it.");
		std::string query = "UPDATE users SET "
					" plrallowed="+NLMISC::toString(user->CharactersAllowed) +
					" state='"+CUser::convertUserStateToString(user->UserState)+"' " +
				    "WHERE userid="+NLMISC::toString(user->UserID);

		reason = sqlQuery(query, nbrow, row, result);
		if(!reason.empty()) {
			nlwarning("Failed to update existing user entry! Reason: %s", reason.c_str());
			return false;
		}
	}

	// If the user isn't in the list, add it.
	if(!isUserInList(user->UserID))
		m_Users.push_back(user);

	return true;
}


