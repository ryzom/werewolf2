/**
 * \file PropertyMap.h
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
#ifndef __PROPERTYMAP_H__
#define __PROPERTYMAP_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/smart_ptr.h>

//
// Werewolf Includes
//
#include "wwcommon/general.h"
#include "IProperty.h"

//
// Namespaces
//

namespace WWSCRIPT {

class PropertyMap : public NLMISC::CRefCount {
public:
	typedef CHashMap<const char*, IProperty*, strltpred_hash_compare> propMap;

	PropertyMap() {};
	PropertyMap(std::string file);
	~PropertyMap();

	void loadXML(std::string file);
	void registerProperty(IProperty* property);
	void removeProperty(const char* name);
	IProperty* getProperty(const char* name);
	//void traverseGLParameters(CGparameter parameter);
	//void traverseD3DParameters(CGparameter parameter);

private:
	propMap m_propertyMap;

};

}; // END OF NAMESPACE WWSCRIPT

#endif // __PROPERTYMAP_H__
