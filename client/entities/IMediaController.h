/**
 * \file IMediaController.h
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

#ifndef __IMEDIACONTROLLER_H__
#define __IMEDIACONTROLLER_H__

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
#include <wwcommon/IDirtyPropagator.h>

//
// Namespaces
//

namespace WWCLIENT {

class IMediaController : public WWCOMMON::IDirtyPropagator {
public:
	virtual void update() = 0;
	virtual void init() = 0;
	virtual const char* getControllerName() = 0;
};

class IMediaControllerWrap : public IMediaController, public boost::python::wrapper<IMediaController> {
public:
	const char* getControllerName() {
		return boost::python::call<const char*>(this->get_override("getControllerName").ptr());
	}

	void update() {
		return boost::python::call<void>(this->get_override("update").ptr());
	}

	void init() {
		return boost::python::call<void>(this->get_override("init").ptr());
	}

	static void init_script_class() {
		using namespace boost::python::api;
		using namespace boost::python;
		class_<IMediaControllerWrap, boost::noncopyable, bases<WWCOMMON::IDirtyPropagator> > imcw("IMediaController", no_init);
		imcw.def("update", pure_virtual(&IMediaController::update));
		imcw.def("init", pure_virtual(&IMediaController::init));
		imcw.def("getControllerName", pure_virtual(&IMediaController::getControllerName));
		imcw.add_property("name", imcw.attr("getControllerName"));	
	}
};

}; // END NAMESPACE WWCLIENT

#endif // __IMEDIACONTROLLER_H__
