/**
 * \file ScriptFunctionInstance.h
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
#ifndef __SCRIPTFUNCTIONINSTANCE_H__
#define __SCRIPTFUNCTIONINSTANCE_H__

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

//
// Werewolf Includes
//
#include "wwcommon/general.h"
#include "ScriptFunction.h"
#include "ScriptVariable.h"
#include "ScriptArg.h"

//
// Namespaces
//

namespace WWSCRIPT {

class ScriptFunctionInstance {
public:
	ScriptFunctionInstance(const ScriptFunction* const function);
	~ScriptFunctionInstance();

	void execute();
	void reset();

	typedef CHashMap<const char*, ScriptVariable*, strltpred_hash_compare> argInstMap;

	ScriptVariable* getArg(const char* name);
	argInstMap::iterator begin();
	argInstMap::iterator end();
	ScriptVariable* getRetVal();
	void setBoundArgs();

	asIScriptContext* getContext();

private:
	void prepare();
	void clear();
	void addArgument(ScriptArg* arg);
	bool m_executed;
	asIScriptContext* m_context;
	const ScriptFunction* const m_function;
	argInstMap m_args;
	ScriptVariable m_return;
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTFUNCTIONINSTANCE_H__
