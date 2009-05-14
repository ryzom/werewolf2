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
#include "CEGUI.h"

//
// NeL Includes
//
#include <nel/cegui/inellibrary.h>

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
	// Load the CEGUI renderer and get a handle to the library.
	NLMISC::CLibrary driverLib;
	if(!driverLib.loadLibrary("nelceguirenderer", true, true , true)) {
		nlerror("Failed to load NeL CEGUI Renderer library.");
	}
	NELRENDERER_CREATE_PROC createNelRenderer = reinterpret_cast<NELRENDERER_CREATE_PROC>(driverLib.getSymbolAddress(NELRENDERER_CREATE_PROC_NAME));


	// create the CEGUI renderer.
	m_GuiRenderer = createNelRenderer(&C3DTask::instance().driver(), true);
	m_GuiSystem = new CEGUI::System(m_GuiRenderer);
	m_GuiRenderer->activateInput();
	m_GuiRenderer->captureCursor(true);

	try	{
		using namespace CEGUI;
			
		Logger::getSingleton().setLoggingLevel(Insane);

		// load scheme and set up defaults
		SchemeManager::getSingleton().loadScheme("WerewolfLook.scheme");
		System::getSingleton().setDefaultMouseCursor("WerewolfLook", "MouseArrow");

		// create the font and set it up as the default.
		//System::getSingleton().setDefaultFont("Tahoma-12");
		if(!FontManager::getSingleton().isFontPresent("Commonwealth-10"))
			FontManager::getSingleton().createFont("Commonwealth-10.font");

		Window* sheet = WindowManager::getSingleton().createWindow("DefaultWindow", "Root");
		//Window* sheet = WindowManager::getSingleton().loadWindowLayout("Werewolf.layout");
		System::getSingleton().setGUISheet(sheet);

		Window* guiLayout = WindowManager::getSingleton().loadWindowLayout("Werewolf.layout");
		sheet->addChildWindow(guiLayout);

		//WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->hide();
		//WindowManager::getSingleton().getWindow("GameTask/DebugWindow")->hide();
		//WindowManager::getSingleton().getWindow("LandscapeProgress")->hide();
		//WindowManager::getSingleton().getWindow("NetworkTask/ConnectLS")->hide();
		//WindowManager::getSingleton().getWindow("NetworkTask/ShardList")->hide();
		//WindowManager::getSingleton().getWindow("GameTask/Chatbox")->hide();
		//WindowManager::getSingleton().getWindow("PreGameTask/SelectChar")->hide();
		// do demo stuff
		//ImagesetManager::getSingleton().createImageset("werewolfgui.imageset");
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

