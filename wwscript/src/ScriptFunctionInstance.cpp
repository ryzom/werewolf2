/**
 * \file ScriptFunctionInstance.cpp
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
#include <iostream>

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwscript/ScriptEngine/ScriptFunctionInstance.h"
#include "wwscript/ScriptEngine/ScriptManager.h"

//
// Namespaces
//

namespace WWSCRIPT {

ScriptFunctionInstance::ScriptFunctionInstance(const ScriptFunction* const function) :
m_function(function), m_executed(false), m_context(NULL), m_return(function->getRetValType(), "return") {
		for(ScriptFunction::ArgMap::const_iterator iter = function->begin(); iter != function->end(); iter++) {
			addArgument(iter->second);
		}
		reset();
}

ScriptFunctionInstance::~ScriptFunctionInstance() {
	clear();
}

void ScriptFunctionInstance::reset() {

	if(m_context)
		m_context->Release();

	m_executed = false;
	
	asIScriptEngine* engine = ScriptManager::getInstance().getEngine();
	m_context = engine->CreateContext();

	m_context->Prepare(m_function->getId());
	
	for(ScriptFunctionInstance::argInstMap::iterator iter = m_args.begin(); iter != m_args.end(); iter++) {
		iter->second->reset();
	}
}

void ScriptFunctionInstance::execute() {
	if(!m_context)
		return;
	prepare();
	for(ScriptFunctionInstance::argInstMap::iterator iter = begin(); iter != end(); iter++) {
		if(!iter->second->isSet()) {
			nlinfo("Execution failed : argument %s not set!" , iter->second->getName().c_str());
			return;
		}
		iter->second->setScriptArg(m_context);
	}
	int result = m_context->Execute();
	switch(result) {
		case asERROR:
			nlwarning("Script execution errored.");
			break;
		case asEXECUTION_ABORTED:
			nlwarning("Script execution aborted.");
			break;
		case asEXECUTION_SUSPENDED:
			nlwarning("Script execution suspended.");
			break;
		case asEXECUTION_EXCEPTION:
			nlwarning("Exception in '%s': '%s'", m_function->getName().c_str(),m_context->GetExceptionString());
			break;
	}
	m_executed = true;
}

ScriptFunctionInstance::argInstMap::iterator ScriptFunctionInstance::begin() {
	prepare();
	return m_args.begin();
}

ScriptFunctionInstance::argInstMap::iterator ScriptFunctionInstance::end() {
	prepare();
	return m_args.end();
}

ScriptVariable* ScriptFunctionInstance::getArg(const char* name) {
	prepare();
	ScriptFunctionInstance::argInstMap::iterator iter = m_args.find(name);
	if(iter == m_args.end())
		return NULL;
	return iter->second;
}

void ScriptFunctionInstance::prepare() {
	if(m_executed) {
		m_executed = false;
		m_context->Prepare(m_function->getId());
	}
}

void ScriptFunctionInstance::clear() {
	if(m_context)
		m_context->Release();
	for(ScriptFunctionInstance::argInstMap::iterator iter = m_args.begin(); iter != m_args.end(); iter++) {
		delete iter->second;
	}
	m_args.clear();
}

ScriptVariable* ScriptFunctionInstance::getRetVal() {
	if(m_return.getType() == ScriptArg::VOID_TYPE)
		return NULL;
	if(m_executed && m_context) {
		m_return.setValueFromReturn(m_context);
		return &m_return;
	}
	else
		return NULL;
}

void ScriptFunctionInstance::addArgument(ScriptArg* arg) {
	m_args[arg->getName().c_str()] = new ScriptVariable(arg);
}

void ScriptFunctionInstance::setBoundArgs() {
	prepare();
	for(ScriptFunctionInstance::argInstMap::const_iterator iter = begin(); iter != end(); iter++) {
		iter->second->setValueFromBinding();
	}
}

}; // END NAMESPACE WWSCRIPT
