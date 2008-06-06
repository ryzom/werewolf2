/**
 * \file IDataContainer.h
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

#ifndef __IDATACONTAINER_H__
#define __IDATACONTAINER_H__

//
// Standard Includes
//

//
// System Includes
//
#include <boost/python.hpp>
#include <hash_map>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "general.h"
#include <wwproperty/IDirtyPropagator.h>
#include <wwproperty/IProperty.h>

//
// Namespaces
//

class IDataContainer : public WWPROPERTY::IDirtyPropagator {
public:
	virtual ~IDataContainer() {
		m_Properties.clear();
	}

	virtual const char* getContainerName() = 0;

	WWPROPERTY::IProperty* getProperty(const char* name) {
		propertyMap::iterator iter = m_Properties.find(name);
		if(iter == m_Properties.end())
			return NULL;
		return iter->second;
	}

	typedef std::hash_map<const char*, WWPROPERTY::IProperty*, std::hash<const char*>, streqpred> propertyMap;

	propertyMap* getPropertyMap() { return &m_Properties; }

protected:
	void addProperty(WWPROPERTY::IProperty* property) {
		m_Properties[property->getName()] = property;
		makeInputConnection(property);
	}

private:
	propertyMap m_Properties;
};

class IDataContainerWrap : public IDataContainer, public boost::python::wrapper<IDataContainer> {
public:
	const char* getContainerName() {
		return boost::python::call<const char*>(this->get_override("getContainerName").ptr());
	}

	static void init_script_class() {
		using namespace boost::python::api;
		using namespace boost::python;
		class_<IDataContainerWrap, boost::noncopyable, bases<WWPROPERTY::IDirtyPropagator> > idcw("IDataContainer", no_init);
		idcw.def("getContainerName", pure_virtual(&IDataContainer::getContainerName));
		idcw.add_property("name", idcw.attr("getContainerName"));
		idcw.def("getProperty", &IDataContainer::getProperty, return_value_policy<reference_existing_object>());
		idcw.def("getPropertyMap", &IDataContainer::getPropertyMap, return_value_policy<reference_existing_object>());
		idcw.add_property("properties", idcw.attr("getPropertyMap"));
	}
};

#endif // __IDATACONTAINER_H__
