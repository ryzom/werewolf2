/**
 * \file CharacterData.h
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

#ifndef __CHARACTERDATA_H__
#define __CHARACTERDATA_H__

//
// Standard Includes
//

//
// System Includes
//
#include <cstdio>
#include <boost/python.hpp>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "IDataContainer.h"
#include "SOB.h"
#include <wwproperty/CBaseProperty.h>

//
// Namespaces
//

class CharacterData : public IDataContainer {
public:
	CharacterData(SOB* sob) : m_Owner(sob), m_Fat("fat"), m_Age("age") { 
		setFat(10);
		setAge(25);
		addProperty(&m_Fat);
		addProperty(&m_Age);
	}

	virtual ~CharacterData() { ; }

	const char* getContainerName() { return m_Name; }

	float getAge() { return m_Age.getValue(); }
	void setAge(float age) { m_Age.setValue(age); }

	float getFat() { return m_Fat.getValue(); }
	void setFat(float fat) { m_Fat.setValue(fat); printf("Fat value changed to %f\n", m_Fat.getValue()); }

	WWPROPERTY::IProperty *getFatProperty() { return &m_Fat; }
	WWPROPERTY::IProperty *getAgeProperty() { return &m_Age; }

	static void init_script_class() {
		using namespace boost::python::api;
		using namespace boost::python;
		class_<CharacterData, bases<IDataContainer> > cdw("CharacterData", no_init);
		cdw.add_property("age", &CharacterData::getAge, &CharacterData::setAge);
		cdw.add_property("fat", &CharacterData::getFat, &CharacterData::setFat);
		cdw.def("getFatProperty", &CharacterData::getFatProperty, return_value_policy<reference_existing_object>());
		cdw.def("getAgeProperty", &CharacterData::getAgeProperty, return_value_policy<reference_existing_object>());
		cdw.add_property("fatProperty", cdw.attr("getFatProperty"));
		cdw.add_property("ageProperty", cdw.attr("getAgeProperty"));
	}

private:
	static const char* m_Name;
	
	SOB* m_Owner;
	WWPROPERTY::CBaseProperty<float> m_Fat;
	WWPROPERTY::CBaseProperty<float> m_Age;
};

#endif // __CHARACTERDATA_H__
