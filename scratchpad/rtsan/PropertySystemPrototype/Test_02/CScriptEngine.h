/**
 * \file CScriptEngine.h
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

#ifndef __CSCRIPTENGINE_H__
#define __CSCRIPTENGINE_H__

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
#include "ISingleton.h"

//
// Namespaces
//

class CScriptEngine : public ISingleton<CScriptEngine> {
public:
	CScriptEngine();
	~CScriptEngine();

	void						init();

	void						execCommand(const char *cmd);
	boost::python::api::object	evalCommand(const char *cmd);
	bool						execScript(const char *script);

	boost::python::api::object* getMainModule();
	boost::python::dict&		getMainNamespace();

private:
	boost::python::api::object* main_module;
	boost::python::dict			main_namespace;
};

#endif // __CSCRIPTENGINE_H__
