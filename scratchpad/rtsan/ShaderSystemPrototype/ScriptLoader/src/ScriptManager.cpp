// Local includes
#include "ScriptLoader/ScriptManager.h"

// Global includes
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <iostream>

XERCES_CPP_NAMESPACE_USE

ScriptManager::ScriptManager() {
	// Create the script engine
	m_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if( m_engine == 0 ) {
		std::cout << "Failed to create script engine." << std::endl;
		exit(-1);
	}

		// The script compiler will send any compiler messages to the outstream
	asCOutputStream out;
	m_engine->SetCommonMessageStream(&out);
	RegisterScriptString(m_engine);
	
	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during initialization! :\n"
			<< message << "\n";
		XMLString::release(&message);
		exit(-1);
	}
};

ScriptManager::~ScriptManager() {
	for(ScriptManager::scriptMap::iterator iter = m_scripts.begin(); iter != m_scripts.end(); iter++) {
		delete iter->second;
	}
	m_scripts.clear();
	m_engine->Release();
	XMLPlatformUtils::Terminate();
};

asIScriptEngine* ScriptManager::getEngine() const {
	return m_engine;
}

Script* ScriptManager::loadScript(std::string file, const char* section) {
	Script* script = new Script(file, section);
	m_scripts[script->getName().c_str()] = script;
	return script;
}

const Script* ScriptManager::getScript(const char* name) const {
	ScriptManager::scriptMap::const_iterator iter = m_scripts.find(name);
	if(iter == m_scripts.end())
		return NULL;
	return iter->second;
}

ScriptManager::scriptMap::const_iterator ScriptManager::begin() const {
	return m_scripts.begin();
}

ScriptManager::scriptMap::const_iterator ScriptManager::end() const {
	return m_scripts.end();
}