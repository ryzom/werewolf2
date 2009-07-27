/**
 * \file ScriptArg.cpp
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
#include <iostream>

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwscript/ScriptEngine/ScriptArg.h"
#include "wwscript/ScriptEngine/ScriptLoader.h"

//
// Namespaces
//

namespace WWSCRIPT {

ScriptArg::ScriptArg(TScriptArgument arg) {
	m_binding = NULL;
	m_name = arg.ArgumentName;
	m_id = arg.ArgumentId;
	m_type = Helper::instance().find(arg.ArgumentType);

//	for(DOMNode* child = node->getFirstChild(); child != 0; child=child->getNextSibling()) {
//		if(child->getNodeType() != DOMNode::ELEMENT_NODE)
//			continue;
//		char* name = XMLString::transcode(child->getNodeName());
//		if(!isBound() && strcmp(name, "binding") == 0) {
//			parseBinding(child);
//		}
//	}
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

ScriptArg::eType ScriptArg::Helper::find(std::string name) const {
	TypeMap::const_iterator iter = m_types.find(name.c_str());
	if(iter == m_types.end())
		return ScriptArg::OBJECT;
	else {
		return iter->second;
	}
}

//void ScriptArg::parseBinding(DOMNode* node) {
//	DOMNamedNodeMap* attributes = node->getAttributes();
//	DOMAttr* attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("map"));
//	const char* map = XMLString::transcode(attr->getValue());
//	attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("property"));
//	const char* property = XMLString::transcode(attr->getValue());
//	m_binding = new Binding(map, property);
//}

}; // END NAMESPACE WWSCRIPT
