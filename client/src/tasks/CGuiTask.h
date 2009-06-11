/**
 * \file CGuiTask.h
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

#ifndef __CGUITASK_H__
#define __CGUITASK_H__

//
// System Includes
//
#include "CEGUI.h"

//
// NeL Includes
//
#include <nel/misc/dynloadlib.h>

//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Typedefs
//

//
// Class
//
namespace CEGUI
{
	class Renderer;
}; // END NAMESPACE CEGUI

namespace WWCLIENT {

class CGuiTask : public WWCOMMON::ISingleton<CGuiTask>, public WWCOMMON::ITask {
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual std::string name();

	CEGUI::System *getRenderSystem() { return m_GuiSystem; }
	CEGUI::Renderer *getRenderer() { return m_GuiRenderer; }
	
protected:
	CEGUI::System		*m_GuiSystem;
	CEGUI::Renderer	*m_GuiRenderer;
	 NLMISC::CLibrary m_DriverLib;
};

}; // END NAMESPACE WWCLIENT

#endif // __CGUITASK_H__
