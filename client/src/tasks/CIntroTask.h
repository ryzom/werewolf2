/**
 * \file CIntroTask.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Some code adapted from MTP-Target.
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

#ifndef __CINTROTASK_H__
#define __CINTROTASK_H__

//
// System Includes
//
#include "CEGUI.h"

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
using namespace NLMISC;

namespace WWCLIENT {

//
// Class
//

/**
 * \brief A list item class.
 */
class IntroListItem : public CEGUI::ListboxTextItem {
public:
	IntroListItem(const CEGUI::String &text) : CEGUI::ListboxTextItem(text) {
		setSelectionBrushImage((CEGUI::utf8*)"WerewolfLook", (CEGUI::utf8*)"MultiListSelectionBrush");
	}
};

class CIntroTask : public WWCOMMON::ISingleton<CIntroTask>, public WWCOMMON::ITask {
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release(); /**< Inherited from ITask, does nothing. */

	void error(std::string &reason);

	/**
	 * \brief Resets the introductory state task.
	 *
	 * This clears the Gui object list and sets the internal state to eInit.
	 */
	void reset();

	/**
	 * \brief Initializes/loads the windows for this task.
	 */
	void initWindows();

	void handlePlayOffline();
	//void handlePlayOnBtn();
	//void handleConnect();
	//void handleConnectShard();

	virtual std::string name() { return "CIntroTask"; }
private:
	uint32 ServerId;
};

}; // END NAMESPACE WWCLIENT

#endif // __CINTROTASK_H__
