/**
 * \file CEntityFactory.h
 * \date June 2004
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

#ifndef __CENTITYFACTORY_H__
#define __CENTITYFACTORY_H__

//
// System Includes
//
#include <map>
#include <string>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "CEntity.h"
#include "ISingleton.h"

//
// Namespaces
//

//
// Class References
//

//
// Class
//

/**
 * \class CEntityFactory CEntityFactory.h "entities/CEntityFactory.h"
 * \brief This provides a concrete instantiation mechanism for entities.
 *
 * This class provides a central mechanism for instantiating concrete
 * entity objects based upon a string name. Simply call the create()
 * method with the name of the class you want to instantiate. Introducing
 * new object types is easy. The object must have IEntity as one of it's
 * base classes, create a static method called create() that instantiates and
 * returns a pointer to the new instance, and finally you register it with 
 * the following code:
 * \code
namespace {
	bool registered=CEntityFactory::instance().registerEntity("self",CEntitySelf::create);
}
 * \endcode
 */
class CEntityFactory : public ISingleton<CEntityFactory> {
public:
	typedef CEntity * (*CreateEntityCallback) ();
	bool registerEntity(std::string typeId, CreateEntityCallback createFn);
	bool unregisterEntity(std::string typeId);
	CEntity *create(std::string typeId);
private:
	typedef std::map<std::string, CreateEntityCallback> CallbackMap;
	CallbackMap m_Callbacks;

};


#endif // __CENTITYFACTORY_H__