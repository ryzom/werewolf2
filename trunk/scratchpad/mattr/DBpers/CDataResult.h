/**
 * \file CDataResult.h
 * \author Matt Raykowski
 *
 * Definition of the class that interfaces to the database.
 */

#ifndef __CDATARESULT_H__
#define __CDATARESULT_H__

#include <nel/misc/types_nl.h>

#include "CDatabase.h"
#include "CDataResult.h"
#include "CDataRow.h"

#ifdef NL_OS_WINDOWS
#include <winsock.h>
#endif

#include <mysql.h>

//
// Forward declarations.
//
class CDataRow;

/**
 * \brief Handles query results.
 */
class CDataResult {
public:
	bool success() const {
		return (m_Result!=NULL);
	}

	bool failed() const {
		return !success();
	}

	uint numRows() {
		return (uint)mysql_num_rows(m_Result);
	}

	CDataRow *fetchRow() {
		if(m_Result==NULL)
			return new CDataRow();
		return new CDataRow(mysql_fetch_row(m_Result), m_Result);
	}

	/**
	 * \brief Updates the list of rows. Should be called only once.
	 */
	void updateResult() {
		if(m_Result==NULL) { // if there's no result, just return
			return;
		}
	}

private:
	friend class CDatabase;

	// private members
	MYSQL_RES	*m_Result;

	// private methods, ctor and dtor
	CDataResult() {
		m_Result=NULL;
		updateResult();
	}
	CDataResult(MYSQL *database) {
		m_Result = mysql_store_result(database);
		updateResult();
	}
	~CDataResult() {
		if (m_Result != NULL)
			mysql_free_result(m_Result);
	}

	operator MYSQL_RES*() {
		return m_Result;
	}

	CDataResult &operator = (MYSQL_RES* res) {
		if(res == m_Result)
			return *this;
		if(m_Result != NULL)
			mysql_free_result(m_Result);
		m_Result = res;
		updateResult();
		return *this;
	}
};

#endif // __CDATARESULT_H__