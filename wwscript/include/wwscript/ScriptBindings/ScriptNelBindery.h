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
#ifndef __SCRIPTNELBINDERY_H__
#define __SCRIPTNELBINDERY_H__

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

//
// Namespaces
//

namespace WWSCRIPT {

class ScriptNelBindery : public ScriptBinding {
public:
	ScriptNelBindery() { }

	static void NelInfo(std::string &log) {
		nlinfo(log.c_str());
	}

	bool bindObjects() {
		nlinfo("Binding NelInfo");
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		ScriptManager::getInstance().getEngine()->RegisterGlobalFunction(
			"void NelInfo(string&in)", 
			asFUNCTIONPR(ScriptNelBindery::NelInfo,(std::string&),void), 
			asCALL_CDECL);

		bindCVector();
		bindCMatrix();

		nlinfo("Done binding.");
		return true;
	}

	void bindCVector() {
		int r;
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		nlinfo("Binding CVector");
		// Register Object Type
		r = engine->RegisterObjectType("CVector", sizeof(NLMISC::CVector), asOBJ_REF); nlassert(r>=0);
		
		// Register Behaviors
		r = engine->RegisterObjectBehaviour("CVector", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CVector", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CVector", asBEHAVE_FACTORY, "CVector @f()", asFUNCTION((asCreateFactory0<NLMISC::CVector>)), asCALL_CDECL); nlassert(r>=0);
		
		// Register Methods.
		r = engine->RegisterObjectMethod("CVector", "float norm()", asMETHODPR(NLMISC::CVector, norm, (void) const, float), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod("CVector", "string& toString()", asMETHOD(NLMISC::CVector, toString), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod("CVector", "bool isNull()", asMETHODPR(NLMISC::CVector, isNull, (void) const, bool), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectMethod("CVector", "void set(float &in,float &in,float &in)", asMETHODPR(NLMISC::CVector, set, (float,float,float), void), asCALL_THISCALL); nlassert(r>=0);

		// Register object properties.
		r = engine->RegisterObjectProperty("CVector", "float x", offsetof(NLMISC::CVector,x)); nlassert(r >= 0);
		r = engine->RegisterObjectProperty("CVector", "float y", offsetof(NLMISC::CVector,y)); nlassert(r >= 0);
		r = engine->RegisterObjectProperty("CVector", "float z", offsetof(NLMISC::CVector,z)); nlassert(r >= 0);

		// Register inheritance.
		// TODO register CVector inheritance.
	}

	void bindCMatrix() {
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding CMatrix");
		r = engine->RegisterObjectType("CMatrix", sizeof(NLMISC::CMatrix), asOBJ_REF); nlassert(r>=0);	

		// Register Behaviors
		r = engine->RegisterObjectBehaviour("CMatrix", asBEHAVE_ADDREF, "void f()", asMETHOD(asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CMatrix", asBEHAVE_RELEASE, "void f()", asMETHOD(asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CMatrix", asBEHAVE_FACTORY, "CMatrix @f()", asFUNCTION((asCreateFactory0<NLMISC::CMatrix>)), asCALL_CDECL); nlassert(r>=0);

		r = engine->RegisterObjectMethod("CMatrix", "void identity()", asMETHODPR(NLMISC::CMatrix,identity,(void),void), asCALL_THISCALL); nlassert(r >= 0);
		r = engine->RegisterObjectMethod("CMatrix", "void setScale(float)", asMETHODPR(NLMISC::CMatrix,setScale, (float), void ), asCALL_THISCALL); nlassert(r>=0);
		//r = engine->RegisterObjectMethod("CMatrix", "void setScale(CVector &in)", asMETHODPR(NLMISC::CMatrix,setScale, (NLMISC::CVector &), void ), asCALL_THISCALL); assert(r>=0);
		r = engine->RegisterObjectMethod("CMatrix", "void getPos(CVector &in)", asMETHODPR(NLMISC::CMatrix,getPos, (NLMISC::CVector &) const, void ), asCALL_THISCALL); nlassert(r>=0);
		//r = engine->RegisterObjectBehaviour("CMatrix", asBEHAVE_MULTIPLY, "CMatrix f(CMatrix &in)", asMETHODPR(NLMISC::CMatrix,operator *, (NLMISC::CMatrix &), NLMISC::CMatrix), asCALL_THISCALL); assert(r>=0);
		//r = engine->RegisterObjectBehaviour("CMatrix", asBEHAVE_MUL_ASSIGN, "CMatrix f(CMatrix &in)", asMETHODPR(NLMISC::CMatrix,operator *=, (NLMISC::CMatrix &in), NLMISC::CMatrix), asCALL_THISCALL); assert(r>=0);
		r = engine->RegisterObjectMethod("CMatrix", "void setCoefficient(float, int, int)", asMETHODPR(NLMISC::CMatrix,setCoefficient, (float, sint, sint), void ), asCALL_THISCALL); nlassert(r>=0);
	}
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTNELBINDERY_H__
