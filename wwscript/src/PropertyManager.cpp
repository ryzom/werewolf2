/**
 * \file PropertyManager.cpp
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

//
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//
#include <stdio.h>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwscript/GlobalProperty/PropertyManager.h"

//
// Namespaces
//

namespace WWSCRIPT {

PropertyManager::PropertyManager() {
	// do nothing
}

PropertyManager::~PropertyManager() {
/*	propertyMapMap::iterator iter;
	for(iter = m_maps.begin(); iter != m_maps.end(); iter++) {
		delete iter->second;
	}
	m_maps.clear();
*/
}

void PropertyManager::registerProperty(const char* map, IProperty* property) {
	PropertyMap* m = getPropertyMap(map);
	if(m == NULL) // TODO throw an exception here.
		return;
	m->registerProperty(property);
}

IProperty* PropertyManager::getProperty(const char* map, const char* property) {
	PropertyMap* m = getPropertyMap(map);
	if(m == NULL) { // TODO throw an exception ehre.
		nlwarning("Unable to find property map '%s'", map);
		return NULL;
	}
	return m->getProperty(property);
}

PropertyMap* PropertyManager::getPropertyMap(const char* name) {
	propertyMapMap::iterator iter = m_maps.find(name);
	if(iter == m_maps.end())
		return NULL; // throw an exception here?
	return iter->second;
}

void PropertyManager::setPropertyMap(const char* name, PropertyMap* map) {
	m_maps[name] = map;
}

}; // END OF NAMESPACE WWSCRIPT
