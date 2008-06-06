/**
 * \file CMessageTranslator.h
 * \date April 2005
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

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include "../Shared/CMsgEntity.h"
#include "../Shared/CMessageTranslator.h"

//
// Namespaces
//

void CMessageTranslator::init() {
	registerList("source");
	registerList("target");
	registerList("name");
	registerList("objType");
	registerList("objectid");
	registerList("position");
	registerList("facing");
	registerList("moveSpeed");
	registerList("msgComment");

	// invalid. typically logging, warning, debugging, etc.
	registerMap(CMsgEntity::ENT_MSG_INVALID, "source");
	registerMap(CMsgEntity::ENT_MSG_INVALID, "target");
	registerMap(CMsgEntity::ENT_MSG_INVALID, "msgComment");

	// spawning
	registerMap(CMsgEntity::ENT_MSG_SPAWN, "source");
	registerMap(CMsgEntity::ENT_MSG_SPAWN, "target");
	registerMap(CMsgEntity::ENT_MSG_SPAWN, "objType");
	registerMap(CMsgEntity::ENT_MSG_SPAWN, "objectid");
}

bool CMessageTranslator::registerMap(uint32 entType, std::string attrib) {
	nlinfo("Mapping attribute: %s to entity type: %d.",attrib.c_str(), entType);

	if(!isInList(attrib)) {
		nlwarning("Attempt to register attribute: %s for msg type: %d failed: attribute not registered.",attrib.c_str(),entType);
		return false;
	}

	TAttributeMap::iterator itr;
	if(!isInMap(entType))
		itr=m_AttributeMap.insert(m_AttributeMap.end(), TAttributeMap::value_type(entType, TAttributeList::list()));
	else
		itr=m_AttributeMap.find(entType);

	itr->second.push_back(attrib);
	return true;
}

bool CMessageTranslator::registerList(std::string attrib) {
	nlinfo("Registering attribute type: %s", attrib.c_str());
	if(isInList(attrib))
		return false;

	m_AttributeList.push_back(attrib);
	return true;
}

bool CMessageTranslator::isInList(std::string attrib) {
	TAttributeList::iterator itr=m_AttributeList.begin();
	while(itr != m_AttributeList.end()) {
		if( (*itr) == attrib )
			return true;
		itr++;
	}
	return false;
}

bool CMessageTranslator::isInMap(uint32 entType) {
	TAttributeMap::const_iterator i = m_AttributeMap.find(entType);
	if(i == m_AttributeMap.end())
		return false;
	return true;
}

CMessageTranslator::TAttributeListItr CMessageTranslator::getTypeListBegin(uint32 entType) {
	if(!isInMap(entType))
		return m_AttributeList.end();
	TAttributeMap::iterator itr=m_AttributeMap.find(entType);

	if(itr == m_AttributeMap.end())
		return m_AttributeList.end();

	TAttributeListItr listr=itr->second.begin();
	return listr;
}

CMessageTranslator::TAttributeListItr CMessageTranslator::getTypeListEnd(uint32 entType) {
	if(!isInMap(entType))
		return m_AttributeList.end();
	TAttributeMap::iterator itr=m_AttributeMap.find(entType);

	if(itr == m_AttributeMap.end())
		return m_AttributeList.end();

	TAttributeListItr listr=itr->second.end();
	return listr;
}
