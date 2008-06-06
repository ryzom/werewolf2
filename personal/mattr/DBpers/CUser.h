/**
 * \file CUser.h
 * \author Matt Raykowski
 *
 * Definition of the class that interfaces to the database.
 */

#ifndef __CUSER_H__
#define __CUSER_H__

#include <nel/misc/types_nl.h>

#include "IRecordSet.h"

class CUser : public IRecordSet {
public:
	CUser();

	/// Using the variables already set, create a new entry. Returns false if unable to create.
	virtual bool create();

	/// Assuming it has a valid ID, save to DB.
	virtual void save();

	/// Using id load the record from the DB.
	virtual void load(uint id);

	/// Using variables set, get the ID of this object. 0 if it doesn't exist.
	virtual uint query();

	/// Set the username var.
	void setUsername(std::string name) { m_Username=name; }

	/// Set the password var.
	void setPassword(std::string pass) { m_Password=pass; }

	/// Set the password var.
	void setUserCookie(std::string cookie) { m_UserCookie=cookie; }

	/// Get the username var.
	std::string getUsername() { return m_Username; }
	/// Get the username var.
	std::string getPassword() { return m_Password; }
	/// Get the username var.
	int getShardId() { return m_ShardId; }
	/// Get the username var.
	std::string getUserState() { return m_UserState; }
	/// Get the username var.
	std::string getUserPrivs() { return m_UserPrivs; }
	/// Get the username var.
	std::string getUserCookie() { return m_UserCookie; }
	/// Get the username var.
	std::string getUserAddr() { return m_UserAddr; }

protected:
	uint		m_UserId;
	std::string	m_Username;
	std::string	m_Password;
	int			m_ShardId;
	std::string	m_UserState;
	std::string	m_UserPrivs;
	std::string	m_UserCookie;
	std::string	m_UserAddr;
};

#endif // __CUSER_H__