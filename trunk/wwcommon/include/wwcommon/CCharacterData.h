/**
 * \file CCharacterData.h
 * \date June 2009
 * \author Matt Raykowski
 */

/* Copyright, 2009 Werewolf
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

#ifndef __CCHARACTERDATA_H__
#define __CCHARACTERDATA_H__

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

//
// Namespaces
//

namespace WWCOMMON {

	/// Defines characters. This will be in the DB in the game/demo.
	struct CCharacterData {
		/// Default ctor.
		CCharacterData() {};
		std::string Name;	/*!< The name of the character. */
		uint32 CharacterID;	/*!< The ID of this character. */
		uint32 UserID;		/*!< The ID of the user who owns this character. */
		std::string EmdType;	/*!< The type of EMD the client should use for this character. */
		bool Online;

		void serial(NLMISC::IStream &f) {
			f.serial(Name);
			f.serial(CharacterID);
			f.serial(UserID);
			f.serial(EmdType);
			f.serial(Online);
		}
	};

}; // END OF NAMESPACE WWCOMMON

#endif // __CCHARACTERDATA_H__
