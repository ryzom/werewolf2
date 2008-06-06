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

#ifndef __CMESSAGETRANSLATOR_H__
#define __CMESSAGETRANSLATOR_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>

//
// Werewolf Includes
//	
#include "../Shared/ISingleton.h"

//
// Namespaces
//

/**
 * \class CMessageTranslator CMessageTranslator.h "Shared/CMessageTranslator.h"
 * \brief Provides a translation mechanism for entity messages.
 *
 * The purpose of this class is to provide the CMsgEntity class a means
 * to convert to and from an NLNET::CMessage. It maintains a list of attributes
 * that an entity can have (m_AttributeList) and maintains a map of what attributes
 * are relevent to various entity message types (m_EntityMap).
 */
class CMessageTranslator : public ISingleton<CMessageTranslator> {
public:
	/// Type representing a list of available attribute types.
	typedef std::list<std::string> TAttributeList;
	/// The iterator for attribute lists.
	typedef std::list<std::string>::iterator TAttributeListItr;
	/// Type representing a map of attribute types. Key is ENT_MSG_*
	typedef std::map<uint32, TAttributeList> TAttributeMap;

	/**
	 * \brief Initialize the message translator.
	 *
	 * Pre-populates the lists and maps with types and attributes.
	 */
	void init();

	/**
	 * \brief Registers an attribute as relevent to a message type.
	 *
	 * \param entType The type for which attrib will be registered
	 * \param attrib The attribute to be registered to entType
	 *
	 * \return True if successfully registered.
	 */
	bool registerMap(uint32 entType, std::string attrib);

	/**
	 * \brief Registers an available attribute on entity messages.
	 *
	 * \param attrib The attribute to be added to the valid attribute list.
	 *
	 * \return True if successfully registered.
	 */
	bool registerList(std::string attrib);

	/**
	 * \brief Checks if an attribute is already registered.
	 *
	 * \param attrib The attrib that will be checked for in the list.
	 *
	 * \return True if attribute is registered.
	 */
	bool isInList(std::string attrib);

	/**
	 * \brief Checks if an entity type has been put into the map.
	 *
	 * \param entType The type that will be checked for in the map.
	 *
	 * \return True if the type is in the map.
	 */
	bool isInMap(uint32 entType);

	/**
	 * \brief Returns the list associated with an entity message type
	 *
	 * \param entType The type in the map to be returned.
	 *
	 * \return A TAttributeListItr containing the attributes for the entType. It's .end() if invalid.
	 */
	TAttributeListItr getTypeListBegin(uint32 entType);

	TAttributeListItr getTypeListEnd(uint32 entType);

protected:

	/// A list of attribute types.
	TAttributeList	m_AttributeList;
	/// A map of attribute types to message types.
	TAttributeMap	m_AttributeMap;
	
};

#endif // __CMESSAGETRANSLATOR_H__
