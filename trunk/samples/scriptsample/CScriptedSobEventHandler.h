/**
 * \file CScriptedGameEventListener.h 
 * \date November 2009
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

#ifndef __CSCRIPTEDSOBEVENTHANDLER_H__
#define __CSCRIPTEDSOBEVENTHANDLER_H__

//
// Standard Includes
//
#include <iostream>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>

//
// Werewolf Includes
//	
#include "wwscript/ScriptEngine/ScriptFunction.h"
#include "wwscript/ScriptEngine/ScriptFunctionInstance.h"

#include "wwcommon/CGameEventRegistrar.h"
#include "wwcommon/IGameEvent.h"
#include "wwcommon/CGameEventServer.h"

//
// Namespaces
//

class CScriptedSobEventHandler : public WWCOMMON::ISobHandler {
public:
	CScriptedSobEventHandler(const WWSCRIPT::ScriptFunction *function) {
		nlinfo("**** Initializing scripted game event listener ****");
		// Save the function.
		if(function == NULL) {
			nlwarning("Invalid function passed to scripted listener.");
		} 	
		m_ScriptFunction = function;
		
		setPriority(0);
	}

	CScriptedSobEventHandler(std::string scriptName, std::string functionName) {
		nlinfo("**** Initializing scripted game event listener ****");

		const WWSCRIPT::Script *script = WWSCRIPT::ScriptManager::getInstance().getScript(scriptName.c_str());
		if(!script) {
			nlerror("Failed to retrieve: %s", scriptName.c_str());
			return;
		}
		m_ScriptFunction = script->getFunction(functionName.c_str());
	
		// Save the function.
		if(m_ScriptFunction == NULL) {
			nlwarning("Invalid function passed to scripted listener.");
		}
		setPriority(0);
	}

	void addHandledEvent(uint32 eventid) {
		m_HandledEvents.push_back(eventid);
	}


	bool handleSobEvent(NLMISC::CSmartPtr<WWCOMMON::ISobEvent> event, WWCOMMON::ISimulationObj* subject) {
		// Get a function instance to execute.
		WWSCRIPT::ScriptFunctionInstance *inst = m_ScriptFunction->getInstance();

		// Pass the event object to the script.
		inst->getArg("sobEvent")->setValue((WWCOMMON::ISobEvent*)event);
		inst->getArg("sobSubject")->setValue((WWCOMMON::ISimulationObj*)subject);
		inst->execute();
		
		// Delete the instance once we're finished.
		delete inst;

		return true;
	}

	eventList *getEventList() {
		return &m_HandledEvents;
	}
private:
	const WWSCRIPT::ScriptFunction *m_ScriptFunction;
	eventList m_HandledEvents;
};

#endif __CSCRIPTEDSOBEVENTHANDLER_H__