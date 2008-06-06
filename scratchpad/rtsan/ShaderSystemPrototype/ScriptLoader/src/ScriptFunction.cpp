// Local functions
#include "ScriptLoader/ScriptFunction.h"
#include "ScriptLoader/ScriptManager.h"
#include "ScriptLoader/ScriptFunctionInstance.h"

// Global includes
#include <angelscript.h>

XERCES_CPP_NAMESPACE_USE

ScriptFunction::ScriptFunction(DOMNode* node, Script* parent) {
	parseFunction(node);

	asIScriptEngine* engine = ScriptManager::instance().getEngine();

	m_id = engine->GetFunctionIDByName(parent->getName().c_str(), m_name.c_str());
}

ScriptFunction::~ScriptFunction() {
	for(argMap::iterator iter = m_args.begin(); iter != m_args.end(); iter++) {
		delete iter->second;
	}
	m_args.clear();
}

int ScriptFunction::getId() const {
	return m_id;
}

const std::string& ScriptFunction::getName() const {
	return m_name;
}

const ScriptArg* ScriptFunction::getArgument(const char* name) const {
	ScriptFunction::argMap::const_iterator iter = m_args.find(name);
	if(iter == m_args.end())
		return NULL;
	return iter->second;
}

void ScriptFunction::parseFunction(DOMNode* node) {
	DOMNamedNodeMap* attributes = node->getAttributes();
	DOMAttr* attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("name"));
	m_name = XMLString::transcode(attr->getValue());
	attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("return"));
	m_return = ScriptArg::Helper::instance().find(XMLString::transcode(attr->getValue()));

	for(DOMNode* child = node->getFirstChild(); child != 0; child=child->getNextSibling()) {
		if(child->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;
		char* name = XMLString::transcode(child->getNodeName());
		if(strcmp(name, "argument") == 0) {
			addArg(child);
		}
	}
}

/*
asIScriptContext* ScriptFunction::getContext() {
	asIScriptEngine *engine = ScriptManager::instance().getEngine();
	asIScriptContext *context = engine->CreateContext();
	context->Prepare(m_id);
	return context;
}
*/

void ScriptFunction::addArg(DOMNode* node) {
	ScriptArg* arg = new ScriptArg(node);
	m_args[arg->getName().c_str()] = arg;
}

ScriptFunctionInstance* ScriptFunction::getInstance() const {
	return new ScriptFunctionInstance(this);
}

ScriptArg::eType ScriptFunction::getRetValType() const {
	return m_return;
}

ScriptFunction::argMap::const_iterator ScriptFunction::begin() const {
	return m_args.begin();
}

ScriptFunction::argMap::const_iterator ScriptFunction::end() const {
	return m_args.end();
}