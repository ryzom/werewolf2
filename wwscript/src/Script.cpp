/**
 * \file Script.cpp
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
#include <fstream>
#include <sstream>

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
#include "wwscript/ScriptEngine/Script.h"
#include "wwscript/ScriptEngine/ScriptManager.h"

//
// Namespaces
//

namespace WWSCRIPT {

Script::Script(TScriptLoader script) {
	std::string scriptContent;
	
	// Save script settings.
	m_name = script.Name;
	m_section = script.Section;
	m_scriptfile = NLMISC::CPath::lookup(script.Source, false);

	// load the actual script
	std::ifstream scriptFile(m_scriptfile.c_str());
	if(scriptFile.is_open()) {
		std::stringstream strStream;

		// read in the script.
		strStream << scriptFile.rdbuf();

		scriptContent = strStream.str();

		// close the file handle.
		scriptFile.close();	
	} else {
		// for some reason was unable to read the script, alert via logging.
		nlwarning("Unable to load script file %s (%s)", m_scriptfile.c_str(), script.Source.c_str());
		return;
	}

	asIScriptEngine* engine = ScriptManager::getInstance().getEngine();
	asIScriptModule* mod = engine->GetModule(m_name.c_str(), asGM_CREATE_IF_NOT_EXISTS);

	// Compile the script
	mod->AddScriptSection(m_section.c_str(), scriptContent.c_str(), scriptContent.size());
	mod->Build();

	// Add functions
	TScriptLoader::FunctionVector::iterator itr=script.ScriptFunctions.begin();
	while(itr != script.ScriptFunctions.end()) {
		ScriptFunction* func = new ScriptFunction((*itr), this);
		m_functions[func->getName().c_str()] =  func;

		itr++;
	}

}

Script::~Script() {
	for(functionMap::iterator iter = m_functions.begin(); iter != m_functions.end(); iter++) {
		delete iter->second;
	}
	m_functions.clear();
}

const ScriptFunction* Script::getFunction(const char* name) const {
	Script::functionMap::const_iterator iter = m_functions.find(name);
	if(iter == m_functions.end())
		return NULL;
	return iter->second;
}

const std::string& Script::getName() const {
	return m_name;
}

Script::functionMap::const_iterator Script::begin() const {
	return m_functions.begin();
}

Script::functionMap::const_iterator Script::end() const {
	return m_functions.end();
}

}; // END NAMESPACE WWSCRIPT
