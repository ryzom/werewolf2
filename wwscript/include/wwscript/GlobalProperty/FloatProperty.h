/**
 * \file FloatProperty.h
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
#ifndef __FLOATPROPERTY_H__
#define __FLOATPROPERTY_H__

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
#include "IProperty.h"

//
// Namespaces
//

namespace WWSCRIPT {

class FloatProperty : public IProperty {
private:
	unsigned int m_size;
	float* (*m_func)(void);

public:
	FloatProperty(float* (*func)(void), unsigned int size, const char* name) : m_size(std::min((uint)4,(uint)size)), m_func(func) { setName(name); };
	~FloatProperty() {};

	void setScriptParameter(ScriptVariable* var);
};

}; // END OF NAMESPACE WWSCRIPT

#endif // __FLOATPROPERTY_H__