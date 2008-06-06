/**
 * \file CPreGameTask.h
 * \date January 2005
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

#ifndef __CPREGAMETASK_H__
#define __CPREGAMETASK_H__

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
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Namespaces
//

namespace WWCLIENT {

//
// Class
//

/// Defines characters. Class automatically sets selection brush and store a character.
struct TCharacterData {
	std::string Name;	/*!< The name of the character. */
	uint32 CharacterID;	/*!< The ID of this character. */
	uint32 UserID;		/*!< The ID of the user who owns this character. */
	std::string EmdType;/*!< The type of EMD the client should use for this character. */
	bool Online;

	void serial(NLMISC::IStream &f) {
		f.serial(Name);
		f.serial(CharacterID);
		f.serial(UserID);
		f.serial(EmdType);
		f.serial(Online);
	}
};

/// Class to automatically set selection brush and store an object ID.
class CharItem : public CEGUI::ListboxTextItem {
public:
	CharItem(const CEGUI::String& text, TCharacterData *character) : ListboxTextItem(text, 0, character) {
		setSelectionBrushImage((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MultiListSelectionBrush");
	}
};

/**
 * \class CPreGameTask CGameTask.h "tasks\CGameTask.h"
 * \brief The core pre game logic task. 
 *
 * Things such as character selection and character creation
 * tasks. This is only used for online games and is initialized
 * by the network portion of CIntroTask
 */
class CPreGameTask : public WWCOMMON::ISingleton<CPreGameTask>, public WWCOMMON::ITask {
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual void stop();
	
	void startGame();
	void insertCharacter(struct TCharacterData character);
	virtual std::string name() { return "CPreGameTask"; }

private:

};

}; // END NAMESPACE WWCLIENT

#endif // __CPREGAMETASK_H__
