/**
 * \file ScriptWwcommonSimulationBindery.h
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
#ifndef __SCRIPTWWCOMMONSIMULATIONBINDERY_H__
#define __SCRIPTWWCOMMONSIMULATIONBINDERY_H__

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
#include "wwcommon/IBaseSimulation.h"

//
// Namespaces
//

namespace WWSCRIPT {

//WWCOMMON::CGameEventServer &getGameEventServer() {
//	return WWCOMMON::CGameEventServer::instance();
//}

class ScriptWwcommonSimulationBindery : public ScriptBinding {
public:
	ScriptWwcommonSimulationBindery() { }

	bool bindObjects() {
		// Simulation
		bindISimulationObj();
		bindCSobManager();
		bindIBaseSimulation();
		nlinfo("Done binding.");
		return true;
	}

	void bindISimulationObj() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding ISimulationObj");
		// Register Object Type
		r = engine->RegisterObjectType("ISimulationObj", sizeof(WWCOMMON::ISimulationObj), asOBJ_REF); nlassert(r>=0);

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("ISimulationObj", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("ISimulationObj", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);

		// Register Methods
		r = engine->RegisterObjectMethod("ISimulationObj", "void init()", asMETHODPR(WWCOMMON::ISimulationObj, init, (void), void), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod("ISimulationObj", "uint32 getSobId()", asMETHODPR(WWCOMMON::ISimulationObj, getSobId, (void), uint32), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod("ISimulationObj", "uint32 getOwnerSobId()", asMETHODPR(WWCOMMON::ISimulationObj, getOwnerSobId, (void), uint32), asCALL_THISCALL); nlassert(r>=0);		
	}

	void bindCSobManager() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding CSobManagerWhat do");
		// Register Object Type
		r = engine->RegisterObjectType("CSobManager", sizeof(WWCOMMON::CSobManager), asOBJ_REF); nlassert(r>=0);

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CSobManager", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CSobManager", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);

	}

	template<class T>
	void registerIBaseSimulation(std::string typeName) {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		r = engine->RegisterObjectMethod(typeName.c_str(), "void update()", asMETHODPR(T, update, (void), void), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "void updateTime()", asMETHODPR(T, updateTime, (void), void), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "double time()", asMETHODPR(T, time, (void) const, double), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "double adjustedTime()", asMETHODPR(T, adjustedTime, (void) const, double), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "double deltaTime()", asMETHODPR(T, deltaTime, (void) const, double), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "double smoothDeltaTime()", asMETHODPR(T, smoothDeltaTime, (void) const, double), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "double adjustSimulationTime()", asMETHODPR(T, adjustSimulationTime, (void) const, double), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "CSobManager @getActorManager()", asMETHODPR(T, getActorManager, (void), const WWCOMMON::CSobManager *), asCALL_THISCALL); nlassert(r>=0);	

	}

	void bindIBaseSimulation() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding IBaseSimulation");
		// Register Object Type
		r = engine->RegisterObjectType("IBaseSimulation", sizeof(WWCOMMON::IBaseSimulation), asOBJ_REF); nlassert(r>=0);

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("IBaseSimulation", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("IBaseSimulation", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);

		r = engine->RegisterGlobalFunction("IBaseSimulation @getSimulation()", asFUNCTION(getSimulation), asCALL_CDECL); nlassert(r>=0);
		registerIBaseSimulation<WWCOMMON::IBaseSimulation>("IBaseSimulation");
	}
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTWWCOMMONSIMULATIONBINDERY_H__
