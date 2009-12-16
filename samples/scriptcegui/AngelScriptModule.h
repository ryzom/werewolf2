/**
 * \file AngelScriptModule.h
 * \date December 2009
 * \author Matt Raykowski
 */

/* Copyright, 2009 Werewolf
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

#ifndef __ANGELSCRIPTMODULE_H__
#define __ANGELSCRIPTMODULE_H__

//
// NeL Includes
//

class AngelScriptCeguiListener {
public:
	bool operator()(const CEGUI::EventArgs& args) const {
		return true;
	}

	static CEGUI::Event::Connection SubscribeEvent(	CEGUI::EventSet* self, 
													const CEGUI::String &eventName,
													const CEGUI::String &scriptName,
													const CEGUI::String &functionName) {
		CEGUI::Event::Connection con;
		AngelScriptCeguiListener listener(scriptName, functionName);
		con=self->subscribeEvent(CEGUI::String(eventName), CEGUI::Event::Subscriber(listener));
		return con;
	}
protected:
	AngelScriptCeguiListener(const CEGUI::String &scriptName, const CEGUI::String &functionName) {
		const WWSCRIPT::Script *script = WWSCRIPT::ScriptManager::getInstance().getScript(scriptName.c_str());
		m_ScriptFunction = script->getFunction(functionName.c_str());
		delete script;
	}

	const WWSCRIPT::ScriptFunction *m_ScriptFunction;
};

class AngelScriptModule : public CEGUI::ScriptModule {
public:
	AngelScriptModule(std::string scriptName) {
		m_asEngine=WWSCRIPT::ScriptManager::getInstance().getEngine();
		m_scriptName = scriptName;
		setModuleIdentifierString();
	}

	~AngelScriptModule() { }

	/**
	 * Execute an AS Script Function.
	 *
	 * We can't actually execute a file in CEGUI, so we'll assume the filename is 
	 * the name of a script function and that the Script name is CEGUI. I know it is
	 * hard coding things but this is the way CEGUI does it and I need to make it jive
	 * with WWSCRIPT and AngelScript. Plus this is quicker and cleaner.
	 */
	void executeScriptFile(const CEGUI::String &filename, const CEGUI::String &resourceGroup="") {
		nlinfo("CEGUI Executing Script File: %s", filename.c_str());
		const WWSCRIPT::Script *scr = WWSCRIPT::ScriptManager::getInstance().getScript("CEGUI");
		const WWSCRIPT::ScriptFunction *func = scr->getFunction(filename.c_str());
		WWSCRIPT::ScriptFunctionInstance *inst = func->getInstance();
		inst->execute();

		return;
	}

	int executeScriptGlobal(const CEGUI::String &function_name) {
		nlinfo("cegui angelscript module executescriptglobal");
		return 0;
	}

	bool executeScriptedEventHandler(const CEGUI::String &handler_name, const CEGUI::EventArgs &e) {
		nlinfo("cegui angelscript module executescriptedeventhandler");
		return true;
	}

	void executeString(const CEGUI::String &str) {
		nlinfo("cegui angelscript module executestring");
		return;
	}

	void createBindings(void) {
		CEGUI::Logger::getSingleton().logEvent("---- Creating AngelScript bindings ----");		
	}

	void destroyBindings(void) {
		CEGUI::Logger::getSingleton().logEvent("---- Destroying AngelScript bindings ----");
	}

	CEGUI::Event::Connection subscribeEvent(	CEGUI::EventSet *target, 
												const CEGUI::String &name, 
												const CEGUI::String &subscriber_name) {
		return AngelScriptCeguiListener::SubscribeEvent(target,name,m_scriptName,subscriber_name);
	}

	CEGUI::Event::Connection subscribeEvent(	CEGUI::EventSet *target, 
												const CEGUI::String &name, 
												CEGUI::Event::Group group, 
												const CEGUI::String &subscriber_name) {
		return AngelScriptCeguiListener::SubscribeEvent(target,name,m_scriptName,subscriber_name);
	}

	void setModuleIdentifierString() {
		d_identifierString = "AngelScriptModule - Werewolf's AngelScript-based scripting module for CEGUI.";
		d_language = "angelscript";
	}

	/* This is non-CEGUI stuff, specific to AngelScript. */

	/// Get the pointer to the script engine.
	asIScriptEngine *getAngelScriptEngine(void) { return m_asEngine; }

	/// Get the name of the script that contains the functions used for this scripting module.
	std::string getScriptName() { return m_scriptName; };

	/// Set the name of the script that contains the functions used for this scripting module.
	void setScriptName(std::string scriptName) { m_scriptName = scriptName; }


private:
	asIScriptEngine *m_asEngine;
	std::string m_scriptName;
};

#endif // __ANGELSCRIPTMODULE_H__

