/**
 * \file MediaScriptController.h
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

#ifndef __MEDIASCRIPTCONTROLLER_H__
#define __MEDIASCRIPTCONTROLLER_H__

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
#include "CScriptEngine.h"

//
// Namespaces
//

template <class T>
class MediaScriptController : public IMediaController {
public:
	MediaScriptController(const char* name, T* obj, const char* init, const char* update)
		: m_Name(name), m_Obj(obj), m_InitScript(init), m_UpdateScript(update) {
			;
	}
	virtual ~MediaScriptController() {
		;
	}

	void initScript() {
		CScriptEngine::instance().execScript(m_InitScript);
	}

	void init() {
		using namespace boost::python::api;
		using namespace boost::python;
		if(CScriptEngine::instance().execScript(m_InitScript)) {
	//		CScriptEngine::instance().execCommand(m_InitScript);
			object func = CScriptEngine::instance().evalCommand("doIt");
			try {
				call<void>(func.ptr(), ptr(this));
			} catch(error_already_set)  {
				PyErr_Print();
			}
		}
	}

	void update() {
		if(isDirty()) {
			using namespace boost::python::api;
			using namespace boost::python;
			CScriptEngine::instance().execScript(m_UpdateScript);
//			CScriptEngine::instance().execCommand(m_UpdateScript);
			object func = CScriptEngine::instance().evalCommand("doIt");
			try {
				call<void>(func.ptr(), ptr(this));
			} catch(error_already_set)  {
				PyErr_Print();
			}
			setClean();
		}
	}
	
	const char* getControllerName() { return m_Name; }
	T* getObj() { return m_Obj; }

	static void init_script_class(const char* typeName) {
		using namespace boost::python::api;
		using namespace boost::python;
		class_<MediaScriptController<T>, bases<IMediaController> > w(typeName, no_init);
		w.def("getObj", &MediaScriptController<T>::getObj, return_value_policy<reference_existing_object>());
		w.add_property("obj", w.attr("getObj"));
	}

private:
	T* m_Obj;
	const char* m_Name;
	const char* m_InitScript;
	const char* m_UpdateScript;
};

#endif // __MEDIASCRIPTCONTROLLER_H__