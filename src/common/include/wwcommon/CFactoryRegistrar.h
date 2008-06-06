/**
 * \file CFactoryRegistrar.h
 * \date February 2006
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

#ifndef __CFACTORYREGISTRAR_H__
#define __CFACTORYREGISTRAR_H__

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//

//
// Class
//

/// This macro is used in the header to setup the registrar object.
#define OF_SETUP_REGISTRAR(OF, BO, DO, KY) \
	const static WWCOMMON::CFactoryRegistrar<OF, BO, DO, KY> registrar; \
	friend class WWCOMMON::CFactoryRegistrar<OF, BO, DO, KY>;

/// This macro is used in the source to initialize the registrar object, registering the DO object.
#define OF_REGISTER(OF, BO, DO, KY, KEY) \
	const static WWCOMMON::CFactoryRegistrar<OF, BO, DO, KY> registrar(KEY);

namespace WWCOMMON {
	/**
	 * \class CFactoryRegistrar.h CFactoryRegistrar.h "wwcommon/CFactoryRegistrar.h"
	 * \brief The main Werewolf task.
	 */
	template<class FactoryClass, class BaseClass, class ManufactureClass, typename ClassKey=std::string>
	class CFactoryRegistrar {
	public:
		/// Creates a new instance of the ManufacturedClass.
		static BaseClass *CreateInstance() {
			return new ManufactureClass();
		}

		/// Registers the create instance method with the factory.
		CFactoryRegistrar(const ClassKey &key) {
			FactoryClass::instance().Register(key, CreateInstance);
		}
	};
}; // END OF NAMESPACE WWCOMMON

#endif // __CFACTORYREGISTRAR_H__
