/**
 * \file IProperty.h
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

#ifndef __COMMONPROPERTY_H__
#define __COMMONPROPERTY_H__

//
// Standard Includes
//

//
// System Includes
//
#include <list>
//#include <boost/python.hpp>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "IDirtyPropagator.h"

//
// Namespaces
//

namespace WWCOMMON {

class CommonProperty : public IDirtyPropagator {
protected:
	CommonProperty(const char* name);

public:
	virtual ~CommonProperty();

	bool makeInputConnection(IDirtyPropagator *input) {
		return false;
	}

//	static void init_script_class() {
//		using namespace boost::python;
//		using namespace boost::python::api;
//		class_<IProperty, bases<IDirtyPropagator>, boost::noncopyable > w("IProperty", no_init);
//		w.add_property("name", &IProperty::getName);
//	}

	const char* getName();

private:
	const char* m_Name;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __COMMONPROPERTY_H__
