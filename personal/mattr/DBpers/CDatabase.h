/**
 * \file CDatabase.h
 * \author Matt Raykowski
 *
 * Definition of the class that interfaces to the database.
 */

#ifndef __CDATABASE_H__
#define __CDATABASE_H__

#include <nel/misc/types_nl.h>

#include "ISingleton.h"
#include "CDataResult.h"
#include "CDataRow.h"

#ifdef NL_OS_WINDOWS
#include <winsock.h>
#endif

#include <mysql.h>

// forward declare result and row classes
class CDataResult;
class CDataRow;

class CDatabase : public ISingleton<CDatabase> {
public:
	CDatabase();

	/**
	 * \brief Checks if a connection was established and pings the server.
	 */
	bool isConnected();

	/**
	 * \brief Connects to a database server.
	 */
	bool connect(std::string host, std::string user, std::string pass, std::string database, uint port=3306);

	/**
	 * \brief Disconnects from a database server.
	 */
	bool disconnect();

	/**
	 * \brief Runs a query using the format passed.
	 * \param format The format string, followed by the format values.
	 * \return A CDataResult wrapping the return result.
	 */
	CDataResult *query(const char *format, ...);

	void clearResult(CDataResult *res) { delete res; }

private:
	MYSQL *m_Database;
	bool m_Connected;
};

#endif
