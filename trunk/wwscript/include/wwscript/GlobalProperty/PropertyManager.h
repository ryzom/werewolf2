/**
 * \file PropertyManager.h
 * \date February 2006
 * \author Henri Kuuste
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
#ifndef __PROPERTY_MANAGER_H__
#define __PROPERTY_MANAGER_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwcommon/general.h"
#include "PropertyMap.h"
#include "IProperty.h"
#include "wwcommon/ISingleton.h"

//
// Namespaces
//

namespace WWSCRIPT {

class PropertyManager : public WWCOMMON::ISingleton<PropertyManager> {
public:
	typedef CHashMap<const char*, PropertyMap*, strltpred_hash_compare> propertyMapMap;
	
	PropertyManager();
	~PropertyManager();

	IProperty* getProperty(const char* map, const char* property);
	/**
	 * Adds a new property to the specified map if the map exists.
	 */
	void registerProperty(const char* map, IProperty* property);

	PropertyMap* getPropertyMap(const char* name);
	void setPropertyMap(const char* name, PropertyMap* map);

private:
	propertyMapMap m_maps;
};

}; // END OF NAMESPACE WWSCRIPT

#endif // __PROPERTYMANAGER_H__
