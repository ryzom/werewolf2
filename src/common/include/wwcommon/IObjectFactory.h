/**
 * \file IObjectFactory.h
 * \date May 2004
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

#ifndef __IOBJECTFACTORY_H__
#define __IOBJECTFACTORY_H__

//
// System Includes
//
#include <map>
#include <string>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "ISingleton.h"

//
// Class
//

namespace WWCOMMON {

/**
 * \interface IObjectFactory IObjectFactory.h "common/IObjectFactory.h"
 * \brief An abstract object factory.
 */
template<class ManufactureClass, typename ClassKey=std::string>
class IObjectFactory : ISingleton<IObjectFactory> {
	/// Define the type used to store the function.
	typedef ManufactureClass* (*CreateFn)();
	/// Define the type of map used to organize functions.
	typedef std::map<ClassKey, CreateFn> TObjMap;
public:
	
	/**
	 * Inserts a object->key pair into the factory map.
	 *
	 * @param key The key used to look up the object.
	 * @param func The function to call to create a new copy of this object.
	 * @returns true if inserted, false if insertion into the map has failed.
	 */
	bool Register(const ClassKey &key, CreateFn func) {
		return m_ObjMap.insert( TObjMap::value_type(key, func)).second;
	}

	/**
	 * Finds the object type specified by the key and returns a newly created
	 * instance of that object.
	 *
	 * @param key The key used to look up the object type.
	 * @returns A new object of the type specified in the ManufactureClass template type.
	 */
	ManufactureClass* Create(const ClassKey &key) const {
		// Find the object by key, make sure we can find it.
		TSobMap::const_iterator i = m_ObjMap.find(key);
		
		// If unable to find the object, just return NULL.
		if( i == m_ObjMap.end())
			return NULL;

		// return the controller to the requestor.
		return (i->second)();;
	}

private:
	/// The map registered functions are stored in.
	TObjMap m_ObjMap;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __IOBJECTFACTORY_H__
