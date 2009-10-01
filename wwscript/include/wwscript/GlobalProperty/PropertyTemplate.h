/**
 * \file PropertyTemplate.h
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
#ifndef __PROPERTYTEMPLATE_H__
#define __PROPERTYTEMPLATE_H__

//
// Standard Includes
//
#include <vector>
#include <sstream>

//
// System Includes
//
#include <angelscript.h>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwscript/ScriptEngine/ScriptVariable.h"

//
// Namespaces
//

namespace WWSCRIPT {

#define SCRIPT_PROPERTY(type,value,name) WWSCRIPT::PropertyTemplate<type>(value,name)

template<class T>
class PropertyTemplate : public WWSCRIPT::IProperty {
public:	
	PropertyTemplate(T value, const char *name) { m_value=value; m_name = name; };
	void setScriptParameter(WWSCRIPT::ScriptVariable* var) { var->setValue((void *)(m_value)); };
protected:
	T	m_value;
};


}; // END OF NAMESPACE WWSCRIPT

#endif // __PROPERTYTEMPLATE_H__
