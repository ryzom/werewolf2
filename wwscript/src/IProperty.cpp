/**
 * \file IProperty.cpp
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

//
// System Includes
//
#include <stdio.h>
#include <angelscript.h>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwscript/ScriptEngine/ScriptManager.h"
#include "wwscript/GlobalProperty/IProperty.h"
#include "wwscript/GlobalProperty/ConstantFloatProperty.h"
#include "wwscript/GlobalProperty/ConstantIntProperty.h"
//#include "wwscript/GlobalProperty/CgGLEnumMatrixProperty.h"
#include "wwscript/GlobalProperty/FloatProperty.h"
#include "wwscript/GlobalProperty/FloatArrayProperty.h"

#include "wwscript/ScriptEngine/ScriptVariable.h"

//
// Namespaces
//

namespace WWSCRIPT {

void ConstantFloatProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME DECENTLY 
	var->setValue(*m_value);
}

void ConstantIntProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME DECENTLY 
	var->setValue(*m_value);
}

void FloatProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME DECENTLY
	var->setValue(*((*m_func)()));
}


void FloatArrayProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME
	nlwarning("FloatArrayProperty Script Parameters Not implemented!");
}

}; // END OF NAMESPACE WWSCRIPT
