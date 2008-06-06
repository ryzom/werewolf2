#ifndef __SCRIPTMANAGER_H__
#define __SCRIPTMANAGER_H__

// Local includes
#include "ScriptLoaderGeneral/ISingleton.h"
#include "scriptstring.h"
#include "Script.h"
#include "ScriptLoaderGeneral/general.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"

// Global includes
#include <stdio.h>
#include <hash_map>
#include <string>
#include <angelscript.h>

class SCRIPT_LOADER_API asCOutputStream : public asIOutputStream
{
public:
	void Write(const char *text) { printf(text); }
};

class SCRIPT_LOADER_API ScriptManager : public ISingleton<ScriptManager> {
public:
	asIScriptEngine* getEngine() const;
	Script* loadScript(std::string xmlfile, const char* section);
	const Script* getScript(const char* name) const;

	typedef std::hash_map<const char*, Script*, std::hash<const char*>, streqpred> scriptMap;
	// typedef std::pair<const char*, Script*> scriptPair;

	scriptMap::const_iterator begin() const;
	scriptMap::const_iterator end() const;

	ScriptManager();
	~ScriptManager();

private:
	asIScriptEngine *m_engine;
	scriptMap m_scripts;

};

#endif // __SCRIPTMANAGER_H__