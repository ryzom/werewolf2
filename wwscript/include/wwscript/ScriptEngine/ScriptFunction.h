/**
 * \file ScriptFunction.h
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
#ifndef __SCRIPTFUNCTION_H__
#define __SCRIPTFUNCTION_H__

//
// Standard Includes
//
#include <string>

//
// System Includes
//
#include <angelscript.h>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "ScriptArg.h"
#include "wwcommon/general.h"
#include "ScriptLoader.h"

//
// Namespaces
//

namespace WWSCRIPT {

class Script;
class ScriptFunctionInstance;

class ScriptFunction {
public:
	ScriptFunction(TScriptFunction func, Script* parent);
	~ScriptFunction();

	typedef CHashMap<const char*, ScriptArg*, strltpred_hash_compare> ArgMap;
	// typedef std::pair<const char*, ScriptArg*> argPair;

	const std::string& getName() const;
	int getId() const;
	const ScriptArg* getArgument(const char* name) const;
	ArgMap::const_iterator begin() const;
	ArgMap::const_iterator end() const;

	ScriptArg::eType getRetValType() const;
	ScriptFunctionInstance* getInstance() const;

private:
	std::string m_name;
	int m_id;
	ArgMap m_args;
	ScriptArg::eType m_return;
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTFUNCTION_H__
