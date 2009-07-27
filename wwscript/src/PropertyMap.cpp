/**
 * \file PropertyMap.cpp
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
#include "wwscript/GlobalProperty/PropertyMap.h"

//
// Namespaces
//

namespace WWSCRIPT {


void PropertyMap::registerProperty(IProperty* property) {
	IProperty *old = getProperty(property->getName());
	if(old)
		delete old;
	m_propertyMap[property->getName()] = property;
}

void PropertyMap::removeProperty(const char* name) {
	propMap::const_iterator iter;
	iter = m_propertyMap.find(name);
	if(iter != m_propertyMap.end()) {
		delete iter->second;
	}
	m_propertyMap.erase(name);
}

IProperty* PropertyMap::getProperty(const char* name) {
	propMap::const_iterator iter;
	iter = m_propertyMap.find(name);
	if(iter == m_propertyMap.end())
		return NULL;
	return iter->second;
}

PropertyMap::~PropertyMap() {
	propMap::iterator iter;
	for(iter = m_propertyMap.begin(); iter != m_propertyMap.end(); iter++) {
		delete iter->second;
	}
	m_propertyMap.clear();
}

}; // END OF NAMESPACE WWSCRIPT