/**
 * \file CDatabase.cpp
 * \author Matt Raykowski
 *
 * Implementation of the class that interfaces to the database.
 */

#include "stdpch.h"

#include <cstdio>

#ifdef NL_OS_WINDOWS
#include <winsock.h>
#endif

#include <mysql.h>

#include <nel/misc/types_nl.h>

#include "CDatabase.h"

CDatabase::CDatabase() {
	m_Connected=false;
}

bool CDatabase::isConnected() {
	if(m_Connected == false) {
		return false;
	} else if(mysql_ping(m_Database) != 0) {
		nlwarning("Connection with MySQL Server lost.");
		return false;
	}
	return true;
}

bool CDatabase::connect(std::string host, std::string user, std::string pass, std::string database, uint port) {
	// init the MySQL driver.
	MYSQL *db = mysql_init(NULL);
	if(db == NULL) {
		nlerror("mysql_init() failed.");
		return false;
	}

	// connect to the specified database.
	m_Database = mysql_real_connect(db, host.c_str(), user.c_str(), pass.c_str(), database.c_str(),port,NULL,0);
	if(m_Database == NULL || m_Database != db) {
		nlerror("mysql_real_connect() failed.");
		return false;
	}
	m_Connected=true;
	return true;
}

bool CDatabase::disconnect() {
	mysql_close(m_Database);
	m_Connected=false;
	return true;
}

CDataResult *CDatabase::query(const char *format, ...) {
	char *query;
	NLMISC_CONVERT_VARGS (query, format, 1024);

	if(!isConnected()) {
		nlerror("Cannot process query, not connected to database.");
		return NULL;
	}

	int ret=mysql_query(m_Database, query);
	if(ret!=0) { // unsuccessful.
		nlwarning("Query failed: %s : %s",query, mysql_error(m_Database));
		return new CDataResult();
	}
	//MYSQL_RES *res=mysql_store_result(m_Database);
	return new CDataResult(m_Database);
}

