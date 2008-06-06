/**
 * \file CDataRow.h
 * \author Matt Raykowski
 *
 * Definition of the class that interfaces to the database.
 */

#ifndef __CDATAROW_H__
#define __CDATAROW_H__

#include <nel/misc/types_nl.h>

#include "CDatabase.h"
#include "CDataResult.h"

#ifdef NL_OS_WINDOWS
#include <winsock.h>
#endif

#include <mysql.h>

class CDataRow {
public:
	~CDataRow() {}

	std::string operator[](uint col) {
		if(!isValid())
			return "";
		return m_Row[col];
	}

	std::string operator[](std::string col) {
		return getColumn(col);
	}

	std::string getColumn(std::string col) {
		if(!isValid())
			return "";

		for(uint idx=0 ; idx<m_NumFields ; idx++) {
			if(col == m_Fields[idx].name) {
				if(m_Row[idx] == NULL)
					return "";
				return m_Row[idx];
			}
		}
		return "";
	}

	std::string getColumn(uint col) {
		if(!isValid())
			return "";

		return m_Row[col];
	}

	bool isValid() {
		if(m_Row==NULL) {
			return false;
		} else if(m_Fields==NULL) {
			return false;
		} else if(m_Res==NULL) {
			return false;
		} else if(m_NumFields<=0) {
			return false;
		}
		return true;
	}

	uint getNumCols() {
		if(!isValid())
			return 0;
		return m_NumFields;
	}

	std::string getColName(uint idx) {
		if(!isValid())
			return "";
		return m_Fields[idx].name;
	}

private:
	friend class CDataResult;
	MYSQL_ROW m_Row;
	MYSQL_RES *m_Res;

	MYSQL_FIELD *m_Fields;
	uint m_NumFields;

	CDataRow() {
		m_Row=NULL;
		m_Res=NULL;
		m_Fields=NULL;
		m_NumFields=0;
	}

	CDataRow(MYSQL_ROW row, MYSQL_RES *res) {
		m_Row=row;
		m_Res=res;

		if(m_Res==NULL||m_Row==NULL) { // if there's no result, just return
			m_NumFields=0;
			m_Fields=NULL;
			return;
		}

		// initializes the list of columnn names.
		m_NumFields=mysql_num_fields(m_Res);
		m_Fields=mysql_fetch_fields(m_Res);
	}

};

#endif // __CDATAROW_H__