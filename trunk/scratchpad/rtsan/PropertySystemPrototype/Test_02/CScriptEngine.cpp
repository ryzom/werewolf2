/**
 * \file CScriptEngine.cpp
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

//
// Standard Includes
//
#include "stdafx.h"

//
// System Includes
//
#include <cstdio>

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

using namespace boost::python;
using namespace boost::python::api;

CScriptEngine::CScriptEngine() {
	Py_Initialize();
}

CScriptEngine::~CScriptEngine() {
	delete main_module;
	Py_Finalize();
}

void CScriptEngine::init() {
	main_module = new object((
    handle<>(borrowed(PyImport_AddModule("__main__")))));
	main_namespace = extract<dict>(main_module->attr("__dict__"));
}

void CScriptEngine::execCommand(const char *cmd) {
	try {
		object result((handle<>(PyRun_String(cmd,
			Py_single_input,
			main_namespace.ptr(),
			main_namespace.ptr()))));

	} catch(error_already_set) {
		PyErr_Print();
	}
}

object CScriptEngine::evalCommand(const char *cmd) {
	try {
		object result((handle<>(PyRun_String(cmd,
			Py_eval_input,
			main_namespace.ptr(),
			main_namespace.ptr()))));

			return result;
	} catch(error_already_set) {
		PyErr_Print();
	}
	return object();
}

bool CScriptEngine::execScript(const char* script) {
	try {
		FILE* fp  = fopen(script, "r");
		if(fp) {
//			PyRun_SimpleFileEx(fp,script, 1);
			object result((handle<>(PyRun_FileEx(fp,
				script,
				Py_file_input,
				main_namespace.ptr(),
				main_namespace.ptr(), 1))));
//			fclose(fp);
			return true;
		} else {
			printf("Could not execute script %s\n", script);
			return false;
		}
	} catch(error_already_set) {
		PyErr_Print();
		return false;
	}
}

object* CScriptEngine::getMainModule() {
	return main_module;
}

dict& CScriptEngine::getMainNamespace() {
	return main_namespace;
}