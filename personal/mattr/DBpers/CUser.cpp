/**
 * \file CUser.cpp
 * \author Matt Raykowski
 *
 * Definition of the class that interfaces to the database.
 */

#include "stdpch.h"

#include <nel/misc/types_nl.h>

#include "CUser.h"

// db access
#include "CDatabase.h"
#include "CDataRow.h"
#include "CDataResult.h"

CUser::CUser() {
	m_RecID=0;
}

bool CUser::create() {
	if(m_RecID != 0)
		return false;
	
	if(m_Username.empty() || m_Password.empty())
		return false;

	uint persid=query();
	if(persid != 0)
		return false;

	CDataResult *res=CDatabase::instance().query(
		"insert into werewolf.users (username, password, shardid, state, privilege, cookie, address) values('%s','%s',%d,'%s','%s','%s','%s')",
		m_Username.c_str(), m_Password.c_str(), m_ShardId, m_UserState.c_str(), m_UserPrivs.c_str(), m_UserCookie.c_str(), m_UserAddr.c_str());
	CDatabase::instance().clearResult(res);

	persid=query();
	if(persid==0)
		return false;
	return true;
}

void CUser::save() {
	if(m_RecID == 0) {
		if(!create())
			nlerror("Catastrophe! Creation failed!\n\r");
	}

	if(m_RecID != m_UserId) {
		nlerror("Fatal error: Record ID should always be the same as the User ID!");
		return;
	}

	std::string qrystr=NLMISC::toString("update werewolf.users set username='%s', passwd='%s', shardid=%d, state='%s', privilege='%s', cookie='%s', address='%s' where userid=%d",
		m_Username.c_str(), m_Password.c_str(), m_ShardId, m_UserState.c_str(), m_UserPrivs.c_str(), m_UserCookie.c_str(), m_UserAddr.c_str(), m_UserId);

	CDataResult *res=CDatabase::instance().query(qrystr.c_str());
	CDatabase::instance().clearResult(res);
}

void CUser::load(uint id) {
	CDataResult *res=CDatabase::instance().query("select * from werewolf.users where userid=%d",id);
	if(res->numRows()<=0 || res->numRows()>1) {
		printf("Error occured retrieving rows.\n\r");
		return;
	}

	CDataRow *row=res->fetchRow();

	m_RecID = m_UserId = atoi(row->getColumn("userid").c_str());
	m_Username=row->getColumn("username");
	m_Password=row->getColumn("passwd");
	m_ShardId=atoi(row->getColumn("shardid").c_str());
	m_UserState=row->getColumn("state");
	m_UserPrivs=row->getColumn("privilege");
	m_UserCookie=row->getColumn("cookie");
	m_UserAddr=row->getColumn("address");
}

uint CUser::query() {
	if(m_Username.empty())
		return 0;

	CDataResult *res=CDatabase::instance().query("select userid from werewolf.users where username='%s'",m_Username.c_str());
	if(res->numRows()<=0 || res->numRows()>1)
		return 0; // no matches.

	CDataRow *row=res->fetchRow();
	
	return atoi(row->getColumn("userid").c_str());
}
