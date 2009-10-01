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

//#include "bindclassutil.h"
#include "registrar.h"

//
// Namespaces
//

//       class          , name     , handle, create
// TODO: add this back if/when I fix the autobinder.
ASTRAITS(NLMISC::CMatrix, "CMatrix", true, true, true);
ASTRAITS(NLMISC::CVector, "CVector", true, true, true);

namespace WWSCRIPT {

class ScriptNelBindery : public ScriptBinding {
public:
	ScriptNelBindery() { }

	static void NelInfo(std::string &log) {
		nlinfo(log.c_str());
	}

	bool bindObjects() {
		int r;
		nlinfo("Binding NelInfo");
		asIScriptEngine *engine = ScriptManager::getInstance().getEngine();
		ScriptManager::getInstance().getEngine()->RegisterGlobalFunction(
			"void NelInfo(string&in)", 
			asFUNCTIONPR(ScriptNelBindery::NelInfo,(std::string&),void), 
			asCALL_CDECL);

		/* autobinder.	*/
		REGISTER_TYPE(NLMISC::CMatrix, engine);	
		r = engine->REGISTER_OBJECT_METHOD(NLMISC::CMatrix, "identity", &NLMISC::CMatrix::identity);
		nlassert(r >= 0);
		nlinfo("Binding CMatrix");
		//r = engine->RegisterObjectType("CMatrix", sizeof(NLMISC::CMatrix), asOBJ_REF); assert(r>=0);
		//r = engine->RegisterGlobalProperty("const CMatrix Identity", &NLMISC::CMatrix::Identity); assert(r>=0);


		nlinfo("Binding CVector");
		/* Autobinder */
		REGISTER_TYPE(NLMISC::CVector, engine);
		r = engine->REGISTER_OBJECT_METHOD(NLMISC::CVector, "norm", &NLMISC::CVector::norm); nlassert(r >= 0);
		//r = engine->REGISTER_OBJECT_METHOD(NLMISC::CVector, "toString", &NLMISC::CVector::toString); nlassert(r >= 0);
		r = engine->REGISTER_OBJECT_METHOD(NLMISC::CVector, "isNull", &NLMISC::CVector::isNull); nlassert(r >= 0);
		nlinfo("registering property x");
		r = engine->RegisterObjectProperty("CVector", "float x", offsetof(NLMISC::CVector,x)); nlassert(r >= 0);

		nlinfo("Done binding.");
		return true;
	}
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTNELBINDERY_H__
