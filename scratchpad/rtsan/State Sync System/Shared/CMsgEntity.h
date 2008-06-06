/**
 * \file CMsgEntity.h
 * \date March 2005
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

#ifndef __CMSGENTITY_H__
#define __CMSGENTITY_H__

//
// Standard Includes
//
#include <string>
#include <list>
#include <map>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/stream.h> 

//
// Werewolf Includes
//	
#include "CMessageTranslator.h"
#include "CEntity.h"

//
// Namespaces
//

/**
 * \class CMsgEntity CMsgEntity.h "Shared/CMsgEntity.h"
 * \brief Contains data for messaging about entity activity and state changes.
 *
 * This class contains data that will be sent between clients and the server to update
 * each other on the changing states and various activities of the entity. It also provides
 * a mechanism using serialization to automatically create a nework message. A special note
 * is that before serializing this to be sent across the network you must set the Type.
 */
class CMsgEntity {
public:
	/// The various types of entity messages.
	enum TEntityMessageType {
		ENT_MSG_INVALID =	0, /*!< Invalid/error message. Just logs the message. */
		ENT_MSG_SPAWN	=	1, /*!< A request for spawning. */
		ENT_MSG_MOTION	=	2, /*!< A change in motion state. */
	};

	/// Default ctor.
	CMsgEntity() { }
	/// Data filling constructor.
	CMsgEntity(std::string name, uint32 uid, uint32 eid) : Name(name), OwnerID(uid), TargetID(eid) { }

	/**
	 * \brief Serialize an entity information message.
	 *
	 * This takes in incoming or outgoing stream and converts the various data types
	 * into the stream. It uses the CMessageTranslator to determine which variables
	 * to insert/extract from the stream and in what order.
	 *
	 * \param f The incoming or outgoing stream.
	 */
	void serial(NLMISC::IStream &f) {
		f.serial(Type); // type is ALWAYS first.

		// get the list of attributes we'll read.
		CMessageTranslator::TAttributeListItr list=CMessageTranslator::instance().getTypeListBegin(Type);
		while(list != CMessageTranslator::instance().getTypeListEnd(Type)) {
			std::string attrName=(*list);
			
			if(attrName == "source") {
				f.serial(OwnerID);
			} else if(attrName == "target") {
				f.serial(TargetID);
			} else if(attrName == "name") {
				f.serial(Name);
			} else if(attrName == "objType") {
				f.serial(ObjType);
			} else if(attrName == "objectid") {
				f.serial(ObjectID);
			} else if(attrName == "position") {
				f.serial(Position);
			} else if(attrName == "motionState" ) {
				f.serialEnum(MotionState);
			} else {
				nlwarning("Unsupported attribute type in message map: %s", attrName.c_str());
			}
			list++;
		}
	}

	// these are *required*
	uint32					Type;		/*!< Type of message this is. All messages must have this. */	
	uint32					OwnerID;	/*!< The ID of the owner of the entity this message is for. Translator name: "source" */
	uint32					TargetID;	/*!< The Entity ID of the target entity this message is for. Translator name: "target" */

	// these are for various messages.
	std::string				Name;		/*!< The friendly name (as seen by players) of this entity. Translator name: "name" */
	std::string				ObjType;	/*!< The type of entity this is. Typically for loading media. Translator name: "objType" */
	uint32					ObjectID;	/*!< This is for a specific entity. Usually character logins. Translator name: "objectid" */
	NLMISC::CVector			Position;	/*!< The position of the entity. Used only on spawns. Translator name: "position" */
	CEntity::eMotionState	MotionState;/*!< The current motion state of the entity. Translator name: "motionState" */
	float					Facing;		/*!< The angle the entity is facing. Translator name: "facing" */
	NLMISC::CVector			MoveDir;	/*!< The direction the entity is moving towards. Translator name: "movDir" */
	
};

#endif // __CMSGENTITY_H__
