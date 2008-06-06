#include "ScriptArg.h"
#include <iostream>
#include "PropertyManager.h"

XERCES_CPP_NAMESPACE_USE

ScriptArg::ScriptArg(DOMNode *node) {
	m_binding = NULL;
	parseArg(node);
}

ScriptArg::~ScriptArg() {
	if(m_binding)
		delete m_binding;
}

int ScriptArg::getId() const {
	return m_id;
}

const std::string& ScriptArg::getName() const {
	return m_name;
}

ScriptArg::eType ScriptArg::getType() const {
	return m_type;
}

void ScriptArg::parseArg(DOMNode* node) {
	DOMNamedNodeMap* attributes = node->getAttributes();
	DOMAttr* attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("name"));
	m_name = XMLString::transcode(attr->getValue());
	attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("id"));
	m_id = atoi(XMLString::transcode(attr->getValue()));
	attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("type"));
	m_type = Helper::instance().find(XMLString::transcode(attr->getValue()));

	for(DOMNode* child = node->getFirstChild(); child != 0; child=child->getNextSibling()) {
		if(child->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;
		char* name = XMLString::transcode(child->getNodeName());
		if(!isBound() && strcmp(name, "binding") == 0) {
			parseBinding(child);
		}
	}
}

bool ScriptArg::isBound() const {
	return m_binding != NULL;
}

const ScriptArg::Binding* ScriptArg::getBinding() const {
	return m_binding;
}

ScriptArg::Helper::Helper() {
	m_types["void"] = ScriptArg::VOID_TYPE;
	m_types["int"] = ScriptArg::INT;
	m_types["uint"] = ScriptArg::UINT;
	m_types["long"] = ScriptArg::LONG;
	m_types["float"] = ScriptArg::FLOAT;
	m_types["double"] = ScriptArg::DOUBLE;
	m_types["bool"] = ScriptArg::BOOL;
	m_types["object"] = ScriptArg::OBJECT;
	m_types["string"] = ScriptArg::STRING;
}

ScriptArg::Helper::~Helper() {
	// do nothing
}

ScriptArg::eType ScriptArg::Helper::find(const char* name) const {
	typeMap::const_iterator iter = m_types.find(name);
	if(iter == m_types.end())
		return ScriptArg::OBJECT;
	else {
		return iter->second;
	}
}

void ScriptArg::parseBinding(DOMNode* node) {
	DOMNamedNodeMap* attributes = node->getAttributes();
	DOMAttr* attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("map"));
	const char* map = XMLString::transcode(attr->getValue());
	attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("property"));
	const char* property = XMLString::transcode(attr->getValue());
	m_binding = new Binding(map, property);
}