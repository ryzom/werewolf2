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

#ifndef __CSCRIPTEDGAMEEVENTLISTENER_H__
#define __CSCRIPTEDGAMEEVENTLISTENER_H__

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

class CScriptedGameEventListener : public WWCOMMON::IGameEventListener {
public:
	CScriptedGameEventListener(const WWSCRIPT::ScriptFunction *function) {
		if(function == NULL) {
			nlwarning("Invalid function passed to scripted listener.");
		} else {
			nlinfo("**** Initializing scripted game event listener ****");
			m_ScriptFunction = function;
			WWCOMMON::CGameEventServer::instance().addListener(this,WWCOMMON::CGameEventServer::ALL_TYPES);
		}
	}

	virtual bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) { return true; };
	virtual bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) { return true; };

	virtual bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
		// Get a function instance to execute.
		WWSCRIPT::ScriptFunctionInstance *inst = m_ScriptFunction->getInstance();

		// Pass the event object to the script.
		inst->getArg("gameEvent")->setValue((WWCOMMON::IGameEvent*)event);
		inst->execute();
		
		// Delete the instance once we're finished.
		delete inst;

		return true;
	}
private:
	const WWSCRIPT::ScriptFunction *m_ScriptFunction;
	std::string m_scriptName;
	std::string m_functionName;
};

#endif __CSCRIPTEDGAMEEVENTLISTENER_H__