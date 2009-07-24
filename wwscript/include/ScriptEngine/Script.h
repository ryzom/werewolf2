/**
 * \file Script.h
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
#ifndef __SCRIPT_H__
#define __SCRIPT_H__

//
// Standard Includes
//
#include <string>

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "ScriptFunction.h"
#include "general.h"
#include "ScriptEngineDefs.h"
#include "ScriptLoader.h"

//
// Namespaces
//

namespace WWSCRIPT {

class WWSCRIPT_API Script {
public:
	Script(TScriptLoader script);

	~Script();

	typedef CHashMap<const char*, ScriptFunction*, streqpred> functionMap;
	// typedef std::pair<const char*, ScriptFunction*> functionPair;

	const std::string& getName() const;
	const ScriptFunction* getFunction(const char* name) const;
	functionMap::const_iterator begin() const;
	functionMap::const_iterator end() const;

private:
	std::string m_name;
	std::string m_scriptfile;
	std::string m_xmlfile;
	std::string m_section;
	functionMap m_functions;
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPT_H__
