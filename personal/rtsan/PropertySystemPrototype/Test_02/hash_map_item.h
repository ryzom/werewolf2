/**
 * \file hash_map_item.h
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

#ifndef __HASH_MAP_ITEM_H__
#define __HASH_MAP_ITEM_H__

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

//
// Namespaces
//

/// For hash maps where Val is a pointer to something
template<class Key, class Val, class Hash, class Equal >
struct hash_map_item
{
	typedef std::hash_map<Key,Val,Hash,Equal> Map;

	static Val get(Map& self, Key idx) {
		if( self.find(idx) != self.end() ) return self[idx];
		PyErr_SetString(PyExc_KeyError,"Map key not found");
		boost::python::throw_error_already_set();
		return NULL;
	}

	static void set(Map& self, Key idx, Val val) { self[idx]=val; }

	static void del(Map& self, Key n) { self.erase(n); }

	static bool in(Map& self, Key n) { return self.find(n) != self.end(); }

	static boost::python::list keys(Map& self)
	{
		boost::python::list t;
		for(Map::const_iterator it=self.begin(); it!=self.end(); ++it)
			t.append(it->first);
		return t;
	}
	static boost::python::list values(Map& self)
	{
		boost::python::list t;
		for(Map::const_iterator it=self.begin(); it!=self.end(); ++it)
			t.append(boost::python::ptr(it->second));
		return t;
	}
	static boost::python::list items(Map& self)
	{
		boost::python::list t;
		for(Map::const_iterator it=self.begin(); it!=self.end(); ++it)
			t.append( boost::python::make_tuple(it->first, boost::python::ptr(it->second)) );
		return t;
	}

	static void init_script_class(const char* name) {
		using namespace boost::python;
		class_<Map>(name, no_init)
			.def("__len__", &Map::size)
			.def("__getitem__", &hash_map_item<Key,Val,Hash,Equal>().get, return_value_policy<reference_existing_object>() )
			.def("__getattr__", &hash_map_item<Key,Val,Hash,Equal>().get, return_value_policy<reference_existing_object>() )
			.def("__setitem__", &hash_map_item<Key,Val,Hash,Equal>().set)
			.def("__delitem__", &hash_map_item<Key,Val,Hash,Equal>().del)
			.def("clear", &Map::clear)
			.def("__contains__", &hash_map_item<Key,Val,Hash,Equal>().in)
			.def("has_key", &hash_map_item<Key,Val,Hash,Equal>().in)
			.def("keys", &hash_map_item<Key,Val,Hash,Equal>().keys)
			.def("values", &hash_map_item<Key,Val,Hash,Equal>().values)
			.def("items", &hash_map_item<Key,Val,Hash,Equal>().items)
			;
	}
};

#endif // __HASH_MAP_ITEM_H__
