#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <xercesc/dom/DOM.hpp>
#include <string>
#include <hash_map>

#include "ScriptFunction.h"
#include "general.h"

class Script {
public:
	Script(std::string file, const char* section);
	~Script();

	typedef std::hash_map<const char*, ScriptFunction*, std::hash<const char*>, streqpred> functionMap;
	// typedef std::pair<const char*, ScriptFunction*> functionPair;

	const std::string& getName() const;
	const ScriptFunction* getFunction(const char* name) const;
	functionMap::const_iterator begin() const;
	functionMap::const_iterator end() const;

private:
	/**
	 * Parse the script node.
	 */
	void parseScript(xercesc::DOMNode* node);
	/**
	 * Add a function from an xml node.
	 */
	void addFunction(xercesc::DOMNode* node);

	std::string m_name;
	std::string m_scriptfile;
	std::string m_xmlfile;
	const char* m_section;
	functionMap m_functions;
};

#endif // __SCRIPT_H__