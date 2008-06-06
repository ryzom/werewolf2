/**
 * \file IRecordSet.h
 * \author Matt Raykowski
 *
 * Definition of the class that interfaces to the database.
 */

#ifndef __IRECORDSET_H__
#define __IRECORDSET_H__

#include <nel/misc/types_nl.h>

class IRecordSet {
public:
	/// Using the variables already set, create a new entry. Returns false if unable to create.
	virtual bool create()=0;

	/// Assuming it has a valid ID, save to DB.
	virtual void save()=0;

	/// Using id load the record from the DB.
	virtual void load(uint id)=0;

	/// Using variables set, get the ID of this object. 0 if it doesn't exist.
	virtual uint query()=0;

	/// Drop a record that isn't needed anymore.
	virtual void drop()=0;
protected:
	uint m_RecID;
};

#endif // __IRECORDSET_H__