/**
 * \file ScriptNelBindery.h
 * \date February 2006
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
#ifndef __SCRIPTWWCOMMONEVENTBINDERY_H__
#define __SCRIPTWWCOMMONEVENTBINDERY_H__

//
// Standard Includes
//

//
// System Includes
//
#include <angelscript.h>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>
#include <nel/misc/matrix.h>

//
// Werewolf Includes
//
#include "wwscript/ScriptEngine/ScriptManager.h"
#include "wwscript/ScriptBindings/ScriptBinding.h"
#include "wwscript/GlobalProperty/IProperty.h"
#include "wwcommon/CGameSpawnRequestEvent.h"
#include "wwcommon/CGameUnspawnRequestEvent.h"

//#include "bindclassutil.h"
#include "registrar.h"

//
// Namespaces
//

//       class          , name     , handle, create
ASTRAITS(WWCOMMON::CGameSpawnRequestEvent, "CGameSpawnRequestEvent", true, true, true);
ASTRAITS(WWCOMMON::CGameUnspawnRequestEvent, "CGameUnspawnRequestEvent", true, true, true);

namespace WWSCRIPT {

class ScriptWwcommonEventBindery : public ScriptBinding {
public:
	ScriptWwcommonEventBindery() { }

	bool bindObjects() {
		bindCVector();
		bindCMatrix();

		bindCGameSpawnRequestEvent();
		bindCGameUnspawnRequestEvent();
		nlinfo("Done binding.");
		return true;
	}

	void bindCGameSpawnRequestEvent() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding CGameSpawnRequestEvent");
		REGISTER_TYPE(WWCOMMON::CGameSpawnRequestEvent, engine);
		r = engine->RegisterObjectMethod("CGameSpawnRequestEvent", "uint32 getPlayerID()", asMETHODPR(WWCOMMON::CGameSpawnRequestEvent, getPlayerID, (void) const, uint32), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod("CGameSpawnRequestEvent", "void setPlayerID(uint32)", asMETHODPR(WWCOMMON::CGameSpawnRequestEvent, setPlayerID, (uint32), void), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindCGameUnspawnRequestEvent() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding CGameSpawnRequestEvent");
		REGISTER_TYPE(WWCOMMON::CGameUnspawnRequestEvent, engine);
		r = engine->RegisterObjectMethod("CGameUnspawnRequestEvent", "uint32 getPlayerID()", asMETHODPR(WWCOMMON::CGameUnspawnRequestEvent, getPlayerID, (void) const, uint32), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod("CGameUnspawnRequestEvent", "void setPlayerID(uint32)", asMETHODPR(WWCOMMON::CGameUnspawnRequestEvent, setPlayerID, (uint32), void), asCALL_THISCALL); nlassert(r>=0);
	}
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTWWCOMMONEVENTBINDERY_H__
