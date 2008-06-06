/**
 * \file SOB.h
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

#ifndef __SOB_H__
#define __SOB_H__

//
// Standard Includes
//

//
// System Includes
//
#include <hash_map>
#include <boost/python.hpp>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "IView.h"
#include "IDataContainer.h"
#include "general.h"
#include "hash_map_item.h"

//
// Namespaces
//

class SOB {
public:
	SOB();
	virtual ~SOB();

	void update();

	void addDataContainer(IDataContainer* data);
	void removeDataContainer(const char* name);
	IDataContainer* getDataContainer(const char* name);

	void addView(IView* view);
	void removeView(const char* name);
	IView* getView(const char* name);

	typedef std::hash_map<const char*, IDataContainer*, std::hash<const char*>, streqpred> dataMap;
	typedef std::hash_map<const char*, IView*, std::hash<const char*>, streqpred> viewMap;

	dataMap* getDataMap() { return &m_Data; }

	static void init_script_class() {
		using namespace boost::python::api;
		using namespace boost::python;
		class_<SOB> w("SOB", no_init);
		w.def("update", &SOB::update);
		w.def("getView", &SOB::getView, return_value_policy<reference_existing_object>());
		w.def("getDataContainer", &SOB::getDataContainer, return_value_policy<reference_existing_object>());
		w.def("getDataMap", &SOB::getDataMap, return_value_policy<reference_existing_object>());
		w.add_property("data", w.attr("getDataMap"));
		w.def("__getattr__", w.attr("getDataContainer"));

		hash_map_item<const char*, IDataContainer*, std::hash<const char*>, streqpred>::init_script_class("DataMap");
	}

private:
	void updateViews();

	dataMap	m_Data;
	viewMap m_Views;
};

#endif // __SOB_H__
