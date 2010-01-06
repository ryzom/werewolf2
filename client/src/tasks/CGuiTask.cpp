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
#include <nel/misc/dynloadlib.h>
#include <nel/cegui/inellibrary.h>
#include <nel/cegui/nelrenderer.h>

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

//CEGUI::Renderer *gGuiRenderer;
//CEGUI::System *gGuiSystem;

namespace WWCLIENT {

void CGuiTask::init() {
	nlinfo("Loading NeL CEGUI Renderer.");

	// Load the CEGUI renderer and get a handle to the library.
	if(!m_DriverLib.loadLibrary("nelceguirenderer", true, true , true)) {
		nlerror("Failed to load NeL CEGUI Renderer library.");
	}
	NELRENDERER_CREATE_PROC createNelRenderer = reinterpret_cast<NELRENDERER_CREATE_PROC>(m_DriverLib.getSymbolAddress(NELRENDERER_CREATE_PROC_NAME));

	// create the CEGUI renderer.
	try {
		m_GuiRenderer = createNelRenderer(&C3DTask::instance().driver(), true);
	} catch(CEGUI::GenericException &e) {
		nlerror("Caught CEGUI Exception during renderer creation: %s", e.getMessage().c_str());
	}

	try {
		m_GuiSystem = new CEGUI::System(m_GuiRenderer);
	} catch(CEGUI::GenericException &e) {
		nlerror("Caught CEGUI Exception during System creation: %s", e.getMessage().c_str());
	}

	CEGUI::NeLRenderer *rndr = (CEGUI::NeLRenderer *)m_GuiRenderer;
	rndr->activateInput();
	rndr->captureCursor(true);

	try	{
		using namespace CEGUI;

		Logger::getSingleton().setLoggingLevel(Insane);

		// load scheme and set up defaults
		SchemeManager::getSingleton().loadScheme("TaharezLook.scheme");
		System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

		// create the font and set it up as the default.
		FontManager::getSingleton().createFont("Commonwealth-10.font");


		//Window* sheet = WindowManager::getSingleton().createWindow("DefaultWindow", "root_wnd");
		Window* sheet = WindowManager::getSingleton().loadWindowLayout("werewolf.layout");
		System::getSingleton().setGUISheet(sheet);
		WindowManager::getSingleton().getWindow("IntroTask/MainMenu")->hide();
		WindowManager::getSingleton().getWindow("GameTask/DebugWindow")->hide();
		WindowManager::getSingleton().getWindow("LandscapeProgress")->hide();
		WindowManager::getSingleton().getWindow("GameTask/Chatbox")->hide();
		WindowManager::getSingleton().getWindow("PreGameTask/SelectChar")->hide();
		WindowManager::getSingleton().getWindow("werewolf/PreGameTask/CreateChar")->hide();
		WindowManager::getSingleton().getWindow("werewolf/OptionsDlg")->hide();
		WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ConnectLS")->hide();
		WindowManager::getSingleton().getWindow("werewolf/NetworkTask/ShardList")->hide();

	} catch(CEGUI::GenericException &e) {
		nlinfo("Caught CEGUI Exception during System creation: %s", e.getMessage().c_str());
	} catch(CEGUI::Exception &e) {	// catch to prevent exit (errors will be logged).
		nlinfo("Failed to initialize CEGUI system: %s", e.getMessage().c_str());
	}

	const CEGUI::Font *fnt = m_GuiSystem->getDefaultFont();
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

