/**
 * \file CBaseProperty.h
 * \date June 2006
 * \author Henri Kuuste
 */

/* Copyright, 2006 Werewolf
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

#ifndef __CBASEPROPERTY_H__
#define __CBASEPROPERTY_H__

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

namespace WWCOMMON {

template <class T>
class CBaseProperty : public IProperty {
public:
	CBaseProperty(const char* name) : IProperty(name) { ; }
	virtual ~CBaseProperty() { ; }

	T& getValue() { return m_Value; }
	void setValue(T val) { m_Value = val; propagateDirtyState(); }

private:

	T m_Value;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CBASEPROPERTY_H__
