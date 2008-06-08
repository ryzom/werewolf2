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
// System Includes
//
#include "stdpch.h"
#include <list>
#include "renderers/NeLRenderer/nelrenderer.h"
#include "CEGUI.h"

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "tasks/CGuiTask.h"
#include "tasks/C3DTask.h"

//
// Namespaces
//
using namespace std;
using namespace NLMISC;
using namespace NL3D;

namespace WWCLIENT {

void CGuiTask::init() {
	// create the CEGUI renderer.
	m_GuiRenderer = new CEGUI::NeLRenderer(&C3DTask::instance().driver());
	m_GuiRenderer->addSearchPath("data/gui/",true,false);
	m_GuiSystem = new CEGUI::System(m_GuiRenderer);
	m_GuiRenderer->activateInput();
	m_GuiRenderer->captureCursor(true);

	try	{
		using namespace CEGUI;
			
		Logger::getSingleton().setLoggingLevel(Insane);

		// load scheme and set up defaults
		SchemeManager::getSingleton().loadScheme("TaharezLook.scheme");
		System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

		// create the font and set it up as the default.
		System::getSingleton().setDefaultFont("Tahoma-12");

		//Window* sheet = WindowManager::getSingleton().createWindow("DefaultWindow", "root_wnd");
		Window* sheet = WindowManager::getSingleton().loadWindowLayout("werewolf.xml");
		System::getSingleton().setGUISheet(sheet);
		WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->hide();
		WindowManager::getSingleton().getWindow("GameTask/DebugWindow")->hide();
		WindowManager::getSingleton().getWindow("LandscapeProgress")->hide();
		WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS")->hide();
		WindowManager::getSingleton().getWindow("NetworkTask/ShardList")->hide();
		WindowManager::getSingleton().getWindow("GameTask/Chatbox")->hide();
		WindowManager::getSingleton().getWindow("PreGameTask/SelectChar")->hide();
		// do demo stuff
		ImagesetManager::getSingleton().createImageset("werewolfgui.imageset");
	} catch(CEGUI::Exception) {	// catch to prevent exit (errors will be logged).
		;
	}
	
}

void CGuiTask::render() {
	m_GuiSystem->renderGUI();
}

void CGuiTask::release() {
	;
}

void CGuiTask::update() {
	;
}

std::string CGuiTask::name() {
	return "CGuiTask";
}

}; // END NAMESPACE WWCLIENT

