/**
 * \file IView.h
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

#ifndef __IVIEW_H__
#define __IVIEW_H__

//
// Standard Includes
//

//
// System Includes
//
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

class IView {
public:
	virtual void update() = 0;
	virtual void render() = 0;

	virtual const char* getViewName() = 0;
};

class IViewWrap : public IView, public boost::python::wrapper<IView> {
public:
	const char* getViewName() {
		return boost::python::call<const char*>(this->get_override("getViewName").ptr());
	}

	void update() {
		return boost::python::call<void>(this->get_override("update").ptr());
	}

	void render() {
		return boost::python::call<void>(this->get_override("render").ptr());
	}

	static void init_script_class() {
		using namespace boost::python::api;
		using namespace boost::python;
		class_<IViewWrap, boost::noncopyable> ivw("IView", no_init);
		ivw.def("update", pure_virtual(&IView::update));
		ivw.def("render", pure_virtual(&IView::render));
		ivw.def("getViewName", pure_virtual(&IView::getViewName));
		ivw.add_property("name", ivw.attr("getViewName"));	
	}
};

#endif // __IVIEW_H__
