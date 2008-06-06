#ifndef __SCRIPTFUNCTION_H__
#define __SCRIPTFUNCTION_H__

#include <xercesc/dom/DOM.hpp>
#include <string>
#include <hash_map>

#include "ScriptArg.h"
#include "general.h"
#include <angelscript.h>

class Script;
class ScriptFunctionInstance;

class ScriptFunction {
public:
	ScriptFunction(xercesc::DOMNode* node, Script* parent);
	~ScriptFunction();

	typedef std::hash_map<const char*, ScriptArg*, std::hash<const char*>, streqpred> argMap;
	// typedef std::pair<const char*, ScriptArg*> argPair;

	const std::string& getName() const;
	int getId() const;
	const ScriptArg* getArgument(const char* name) const;
	argMap::const_iterator begin() const;
	argMap::const_iterator end() const;

	ScriptArg::eType getRetValType() const;
	ScriptFunctionInstance* getInstance() const;
//	asIScriptContext* getContext();

private:
	void parseFunction(xercesc::DOMNode* node);
	void addArg(xercesc::DOMNode* node);
	std::string m_name;
	int m_id;
	argMap m_args;
	ScriptArg::eType m_return;
};


#endif // __SCRIPTFUNCTION_H__