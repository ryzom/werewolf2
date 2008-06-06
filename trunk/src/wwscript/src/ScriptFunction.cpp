/**
 * \file ScriptFunction.cpp
 * \date February 2006
 * \author Henri Kuuste
 * \author Matt Raykowski
 */

/* Copyright, 2004 Werewolf
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
#include "stdpch.h"

//
// System Includes
//
#include <angelscript.h>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "ScriptEngine/ScriptFunction.h"
#include "ScriptEngine/ScriptManager.h"
#include "ScriptEngine/ScriptFunctionInstance.h"
#include "ScriptEngine/ScriptLoader.h"

//
// Namespaces
//

namespace WWSCRIPT {

ScriptFunction::ScriptFunction(const TScriptFunction func, Script* parent) {
	m_name = func.FunctionName;
	m_return = ScriptArg::Helper::instance().find(func.ReturnType.c_str());

	TScriptFunction::ArgumentVector::const_iterator itr=func.Arguments.begin();
	while(itr != func.Arguments.end()) {
		ScriptArg* arg = new ScriptArg((*itr));
		m_args[arg->getName().c_str()] = arg;
		itr++;
	}

	asIScriptEngine* engine = ScriptManager::instance().getEngine();
	m_id = engine->GetFunctionIDByName(parent->getName().c_str(), m_name.c_str());
}

ScriptFunction::~ScriptFunction() {
	for(ArgMap::iterator iter = m_args.begin(); iter != m_args.end(); iter++) {
		delete iter->second;
	}
	m_args.clear();
}

int ScriptFunction::getId() const {
	return m_id;
}

const std::string& ScriptFunction::getName() const {
	return m_name;
}

const ScriptArg* ScriptFunction::getArgument(const char* name) const {
	ScriptFunction::ArgMap::const_iterator iter = m_args.find(name);
	if(iter == m_args.end())
		return NULL;
	return iter->second;
}

ScriptFunctionInstance* ScriptFunction::getInstance() const {
	return new ScriptFunctionInstance(this);
}

ScriptArg::eType ScriptFunction::getRetValType() const {
	return m_return;
}

ScriptFunction::ArgMap::const_iterator ScriptFunction::begin() const {
	return m_args.begin();
}

ScriptFunction::ArgMap::const_iterator ScriptFunction::end() const {
	return m_args.end();
}

}; // END NAMESPACE WWSCRIPT
