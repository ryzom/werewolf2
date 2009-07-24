/**
 * \file ScriptManager.h
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
#ifndef __SCRIPTMANAGER_H__
#define __SCRIPTMANAGER_H__

//
// Standard Includes
//
#include <hash_map>
#include <string>

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
#include "wwcommon/ISingleton.h"
#include "scriptstring.h"
#include "Script.h"
#include "general.h"
#include "ScriptEngineDefs.h"

//
// Namespaces
//

namespace WWSCRIPT {

class WWSCRIPT_API asCOutputStream : public asIOutputStream
{
public:
	void Write(const char *text) { nlinfo("%s", text); }
};

class WWSCRIPT_API ScriptManager : public WWCOMMON::ISingleton<ScriptManager> {
public:
	asIScriptEngine* getEngine() const;
	Script* loadScript(std::string xmlfile, const char* section);
	const Script* getScript(const char* name) const;

	bool initializeScripts();

	typedef std::hash_map<const char*, Script*, std::hash<const char*>, streqpred> scriptMap;
	// typedef std::pair<const char*, Script*> scriptPair;

	scriptMap::const_iterator begin() const;
	scriptMap::const_iterator end() const;

	ScriptManager();
	~ScriptManager();

private:
	asIScriptEngine *m_engine;
	scriptMap m_scripts;

	NLMISC::CApplicationContext m_ApplicationContext;
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTMANAGER_H__
