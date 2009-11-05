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
#include "wwcommon/CGameEventServer.h"
#include "wwcommon/CSobAddEvent.h"

//#include "bindclassutil.h"
//#include "registrar.h"

//
// Namespaces
//

//       class          , name     , handle, create
//ASTRAITS(WWCOMMON::CGameSpawnRequestEvent, "CGameSpawnRequestEvent", true, true, true);
//ASTRAITS(WWCOMMON::CGameUnspawnRequestEvent, "CGameUnspawnRequestEvent", true, true, true);
//ASTRAITS(WWCOMMON::IGameEvent, "IGameEvent", true, false, true);
//ASTRAITS(WWCOMMON::CGameEventServer, "CGameEventServer", true, false, true);

namespace WWSCRIPT {

template<class T>
T &createSingleton() {
	return T::instance();
}

WWCOMMON::CGameEventServer &getGameEventServer() {
	return WWCOMMON::CGameEventServer::instance();
}

template<class T1, class T2>
T2* refCast(T1 *t1) {
	if(!t1) return 0;

	T2 *t2 = dynamic_cast<T2*>(t1);
	if(t2 != 0) {
		//t2->addref();
	}
	return t2;
}

class asRefDummy {
public:
	void addRef() { };
	void release() { };
};

template<class T>
T *asCreateFactory() { return new T(); }

class ScriptWwcommonEventBindery : public ScriptBinding {
public:
	ScriptWwcommonEventBindery() { }

	bool bindObjects() {
		// Game Events
		bindIGameEvent();
		bindCGameUnspawnRequestEvent();
		bindCGameSpawnRequestEvent();
		
		// Sob Events
		bindISobEvent();
		bindCSobAddEvent();

		// Game Event Server
		bindCGameEventServer();
		nlinfo("Done binding.");
		return true;
	}

	void bindCGameEventServer() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding CGameEventServer");
		// Register Object Type
		r = engine->RegisterObjectType("CGameEventServer", sizeof(WWCOMMON::CGameEventServer), asOBJ_REF); nlassert(r>=0);

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CGameEventServer", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CGameEventServer", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);

		r = engine->RegisterGlobalFunction("CGameEventServer @getGameEventServer()", asFUNCTION(getGameEventServer), asCALL_CDECL); nlassert(r>=0);
		r = engine->RegisterObjectMethod("CGameEventServer", "void processEventQueue()", asMETHODPR(WWCOMMON::CGameEventServer, processEventQueue, (void), void), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod("CGameEventServer", "void postEvent(IGameEvent@)", asMETHODPR(WWCOMMON::CGameEventServer, postEvent, (WWCOMMON::IGameEvent*), void), asCALL_THISCALL); nlassert(r>=0);
	}	

	
	/*
	 * Begin Game Events.
	 */
	template<class T>
	void registerIGameEvent(std::string typeName) {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		r = engine->RegisterObjectMethod(typeName.c_str(), "uint32 getPlayerID()", asMETHODPR(T, getPlayerID, (void) const, uint32), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "void setPlayerID(uint32)", asMETHODPR(T, setPlayerID, (uint32), void), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "uint16 getId()", asMETHODPR(T, getId, (void) const, uint16), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod(typeName.c_str(), "string getClassName()", asMETHOD(T, getClassName), asCALL_THISCALL); nlassert(r>=0);
	}

	void bindIGameEvent() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding IGameEvent");
		//REGISTER_TYPE(WWCOMMON::IGameEvent, engine);
		r = engine->RegisterObjectType("IGameEvent", 0, asOBJ_REF); nlassert(r>=0);

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("IGameEvent", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("IGameEvent", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);

		registerIGameEvent<WWCOMMON::IGameEvent>("IGameEvent");
	}

	void bindCGameSpawnRequestEvent() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding CGameSpawnRequestEvent");		
		// Register Object Type
		r = engine->RegisterObjectType("CGameSpawnRequestEvent", 0, asOBJ_REF); nlassert(r>=0);
		
		// Register Behaviors.
		r = engine->RegisterObjectBehaviour("CGameSpawnRequestEvent", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CGameSpawnRequestEvent", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CGameSpawnRequestEvent", asBEHAVE_FACTORY, "CGameSpawnRequestEvent @f()", asFUNCTION((asCreateFactory<WWCOMMON::CGameSpawnRequestEvent>)), asCALL_CDECL); nlassert(r>=0);

		// Register parents methods.
		registerIGameEvent<WWCOMMON::CGameSpawnRequestEvent>("CGameSpawnRequestEvent");
		
		// Register object properties.
		r = engine->RegisterObjectProperty("CGameSpawnRequestEvent", "uint32 CharacterID", offsetof(WWCOMMON::CGameSpawnRequestEvent, CharacterID)); nlassert(r>=0);

		// Register inheritance.
		r = engine->RegisterObjectBehaviour("CGameSpawnRequestEvent", asBEHAVE_IMPLICIT_REF_CAST, "IGameEvent@ f()", asFUNCTION((refCast<WWCOMMON::CGameSpawnRequestEvent,WWCOMMON::IGameEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("IGameEvent", asBEHAVE_REF_CAST, "CGameSpawnRequestEvent@ f()", asFUNCTION((refCast<WWCOMMON::IGameEvent,WWCOMMON::CGameSpawnRequestEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);
	}

	void bindCGameUnspawnRequestEvent() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding CGameUnspawnRequestEvent");		
		// Register Object Type
		r = engine->RegisterObjectType("CGameUnspawnRequestEvent", 0, asOBJ_REF); nlassert(r>=0);
		
		// Register Behaviors.
		r = engine->RegisterObjectBehaviour("CGameUnspawnRequestEvent", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CGameUnspawnRequestEvent", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CGameUnspawnRequestEvent", asBEHAVE_FACTORY, "CGameUnspawnRequestEvent @f()", asFUNCTION((asCreateFactory<WWCOMMON::CGameUnspawnRequestEvent>)), asCALL_CDECL); nlassert(r>=0);

		// Register parents methods.
		registerIGameEvent<WWCOMMON::CGameUnspawnRequestEvent>("CGameUnspawnRequestEvent");
		
		// Register object properties.
		r = engine->RegisterObjectProperty("CGameUnspawnRequestEvent", "uint32 SobID", offsetof(WWCOMMON::CGameUnspawnRequestEvent, SobID)); nlassert(r>=0);

		// Register inheritance.
		r = engine->RegisterObjectBehaviour("CGameUnspawnRequestEvent", asBEHAVE_IMPLICIT_REF_CAST, "IGameEvent@ f()", asFUNCTION((refCast<WWCOMMON::CGameUnspawnRequestEvent,WWCOMMON::IGameEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("IGameEvent", asBEHAVE_REF_CAST, "CGameUnspawnRequestEvent@ f()", asFUNCTION((refCast<WWCOMMON::IGameEvent,WWCOMMON::CGameUnspawnRequestEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);
	}




	
	/*
	 * Begin Sob Events.
	 */
	template<class T>
	void registerISobEvent(std::string typeName) {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		
		// Register parents methods.
		registerIGameEvent<T>(typeName.c_str());

		// Register ISobEvent Properties.
		r = engine->RegisterObjectProperty(typeName.c_str(), "uint32 SourceID", offsetof(T, SourceID)); nlassert(r>=0);
		r = engine->RegisterObjectProperty(typeName.c_str(), "uint32 TargetID", offsetof(T, SourceID)); nlassert(r>=0);
	}

	void bindISobEvent() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding ISobEvent");
		r = engine->RegisterObjectType("ISobEvent", 0, asOBJ_REF); nlassert(r>=0);

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("ISobEvent", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("ISobEvent", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);

		// Register parents methods.
		registerISobEvent<WWCOMMON::ISobEvent>("ISobEvent");

		// Register inheritance.
		r = engine->RegisterObjectBehaviour("ISobEvent", asBEHAVE_IMPLICIT_REF_CAST, "IGameEvent@ f()", asFUNCTION((refCast<WWCOMMON::ISobEvent,WWCOMMON::IGameEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);		
		r = engine->RegisterObjectBehaviour("IGameEvent", asBEHAVE_REF_CAST, "ISobEvent@ f()", asFUNCTION((refCast<WWCOMMON::IGameEvent,WWCOMMON::ISobEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);

	}

	void bindCSobAddEvent() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding CSobAddEvent");		
		// Register Object Type
		r = engine->RegisterObjectType("CSobAddEvent", 0, asOBJ_REF); nlassert(r>=0);
		
		// Register Behaviors.
		r = engine->RegisterObjectBehaviour("CSobAddEvent", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CSobAddEvent", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CSobAddEvent", asBEHAVE_FACTORY, "CSobAddEvent @f()", asFUNCTION((asCreateFactory<WWCOMMON::CSobAddEvent>)), asCALL_CDECL); nlassert(r>=0);

		// Register parents methods.
		registerISobEvent<WWCOMMON::CSobAddEvent>("CSobAddEvent");

		// Register object properties.
		r = engine->RegisterObjectProperty("CSobAddEvent", "CVector Position", offsetof(WWCOMMON::CSobAddEvent, Position)); nlassert(r>=0);
		r = engine->RegisterObjectProperty("CSobAddEvent", "CVector Speed", offsetof(WWCOMMON::CSobAddEvent, Speed)); nlassert(r>=0);
		r = engine->RegisterObjectProperty("CSobAddEvent", "float Yaw", offsetof(WWCOMMON::CSobAddEvent, Yaw)); nlassert(r>=0);
		r = engine->RegisterObjectProperty("CSobAddEvent", "float YawVelocity", offsetof(WWCOMMON::CSobAddEvent, YawVelocity)); nlassert(r>=0);
		r = engine->RegisterObjectProperty("CSobAddEvent", "string EMD", offsetof(WWCOMMON::CSobAddEvent, EMD)); nlassert(r>=0);
		/* TODO: std::vector<uint32> ActiveStateList, is this necessary? */

		// Register inheritance.
		r = engine->RegisterObjectBehaviour("CSobAddEvent", asBEHAVE_IMPLICIT_REF_CAST, "ISobEvent@ f()", asFUNCTION((refCast<WWCOMMON::CSobAddEvent,WWCOMMON::ISobEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CSobAddEvent", asBEHAVE_IMPLICIT_REF_CAST, "IGameEvent@ f()", asFUNCTION((refCast<WWCOMMON::CSobAddEvent,WWCOMMON::IGameEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("ISobEvent", asBEHAVE_REF_CAST, "CSobAddEvent@ f()", asFUNCTION((refCast<WWCOMMON::ISobEvent,WWCOMMON::CSobAddEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("IGameEvent", asBEHAVE_REF_CAST, "CSobAddEvent@ f()", asFUNCTION((refCast<WWCOMMON::IGameEvent,WWCOMMON::CSobAddEvent>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);
	}


};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTWWCOMMONEVENTBINDERY_H__
