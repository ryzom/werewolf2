/**
 * \file ScriptManager.cpp
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
#include "wwscript/ScriptEngine/ScriptManager.h"
#include "wwscript/ScriptEngine/ScriptLoader.h"
#include "wwscript/ScriptBindings/ScriptNelBindery.h"

// private.
#include "angelscript/scriptstring.h"

//
// Namespaces
//

namespace WWSCRIPT {

NLMISC_SAFE_SINGLETON_IMPL(ScriptManager)

void ScriptManager::initialize() {
	nlinfo("initializing script manager.");
	// Create the script engine
	m_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if( m_engine == 0 ) {
		nlerror("Failed to create script engine.");
		return;
	}

	// The script compiler will send any compiler messages to the outstream
	//asCOutputStream out;
	asCOutStream out;
	//m_engine->SetCommonMessageStream(&out);
	m_engine->SetMessageCallback(asMETHOD(asCOutStream,Callback), &out, asCALL_THISCALL);
	RegisterScriptString(m_engine);
};

//ScriptManager::~ScriptManager() {
//	for(ScriptManager::scriptMap::iterator iter = m_scripts.begin(); iter != m_scripts.end(); iter++) {
//		delete iter->second;
//	}
//	m_scripts.clear();
//	m_engine->Release();
//};

asIScriptEngine* ScriptManager::getEngine() const {
	return m_engine;
}

Script* ScriptManager::loadScript(std::string file, const char* section) {
//	Script* script = new Script(file, section);
//	m_scripts[script->getName().c_str()] = script;
//	return script;

	// this will need to be modified to use loadForm manually. If we even want to bother.
	return NULL;
}

const Script* ScriptManager::getScript(const char* name) const {
	ScriptManager::scriptMap::const_iterator iter = m_scripts.find(name);
	if(iter == m_scripts.end())
		return NULL;
	return iter->second;
}

bool ScriptManager::initializeScripts() {
	
	// script container.
	nlinfo("Initializing script container.");
	std::map<std::string, TScriptLoader> container;

	// load the scripts
	nlinfo("Loading scripts.");
	::loadForm("script", "data/script.packed_sheets", container, true, false);

	// process NeL static bindery.
	nlinfo("Initializing NeL Bindery");
	ScriptNelBindery nelBindery;
	nelBindery.bindNel();

	nlinfo("Creating script objects from loader.");
	// Now go throught the list of scripts and create them.
	std::map<std::string, TScriptLoader>::iterator first(container.begin()), last(container.end());
	for(; first != last; ++first) {
		nlinfo("Creating script: %s", first->second.Name.c_str());
		Script *script=new Script(first->second);
		m_scripts[script->getName().c_str()] = script;
	}

	return true;
}

ScriptManager::scriptMap::const_iterator ScriptManager::begin() const {
	return m_scripts.begin();
}

ScriptManager::scriptMap::const_iterator ScriptManager::end() const {
	return m_scripts.end();
}

}; // END NAMESPACE WWSCRIPT
