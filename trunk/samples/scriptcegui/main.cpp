/**
 * \file main.cpp
 * \date November 2004
 * \author Matt Raykowski
 * \author Henri Kuuste
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

#ifdef NL_OS_WINDOWS
#define USE_JPEG
#endif

#include "globals.h"
#include "resource.h"

//
// System Includes
//
#ifdef NL_OS_WINDOWS
#	define NOMINMAX
#	include <windows.h>
	HINSTANCE ghInstance = 0;
#endif
#include <string>

//
// CEGUI Includes
//
#include <nel/cegui/inellibrary.h>
#include <nel/cegui/nelrenderer.h>
//#include <nel/cegui/nellogger.h>
#include <CEGUI.h>
#include <CEGUILogger.h>

#include "NeLDriver.h"
#include "NeLInputDriver.h"

#include <nel/misc/hierarchical_timer.h>
#include <nel/misc/dynloadlib.h>

#include "wwcommon/CGameEventRegistrar.h"
#include "wwcommon/CFactoryRegistrar.h"

#include "wwscript/ScriptEngine/ScriptManager.h"
#include "wwscript/ScriptEngine/ScriptFunctionInstance.h"
#include "wwscript/ScriptBindings/ScriptBinding.h"

#include "ScriptCEGUIBindery.h"
#include "AngelScriptModule.h"

//
// Namespaces
//
using namespace std;

//
// NEL GLOBALS
//
NeLDriver *gDriver;
uint16 gScreenWidth;
uint16 gScreenHeight;

//
// CEGUI GLOBALS
//
//CEGUI::NeLLogger *gGuiLogger;
CEGUI::System *gGuiSystem;
CEGUI::Renderer *gGuiRenderer;
bool gStopDemo;

void createDemoWindows();
void initDemoEventWiring();
bool handleQuit(const CEGUI::EventArgs& e);
bool handleSlider(const CEGUI::EventArgs& e);
bool handleRadio(const CEGUI::EventArgs& e);
bool handleCheck(const CEGUI::EventArgs& e);

#ifndef CEGUI_DATA_DIR
#define CEGUI_DATA_DIR "datafiles"
#endif // CEGUI_DATA_DIR


#ifdef NL_OS_WINDOWS
int WINAPI WinMain( HINSTANCE hInstance, 
									 HINSTANCE hPrevInstance, 
									 LPSTR lpCmdLine, 
									 int nCmdShow ) {
	ghInstance = hInstance;
#else
int main(int argc, char **argv)
{
#endif
	NLMISC::CApplicationContext myApplicationContext;
	try {
		gScreenWidth=800;
		gScreenHeight=600;
		NLMISC::CPath::addSearchPath(CEGUI_DATA_DIR,true,false);

		// Register game and simulation object events.
		WWCOMMON::registerEvents();
		WWCOMMON::registerCommonFactoryObjects();
		
		// Now initialize the scripting system.
		WWSCRIPT::ScriptManager::getInstance().initialize();

		// Add my "client side" bindings.
		WWSCRIPT::ScriptManager::getInstance().addBindingObj(new BindCEGUIObjects());

		WWSCRIPT::ScriptManager::getInstance().initializeScripts();

		// Load the CEGUI renderer and get a handle to the library.
		NLMISC::CLibrary driverLib;
		if(!driverLib.loadLibrary("nelceguirenderer", true, true , true)) {
			nlerror("Failed to load NeL CEGUI Renderer library.");
		}
		NELRENDERER_CREATE_PROC createNelRenderer = reinterpret_cast<NELRENDERER_CREATE_PROC>(driverLib.getSymbolAddress(NELRENDERER_CREATE_PROC_NAME));
		
		NL3D::UDriver *driver;

		// Create a driver
		uint icon = 0;
#ifdef NL_OS_WINDOWS
		icon = (uint)LoadIcon(ghInstance,MAKEINTRESOURCE(IDI_ICON1));
#endif
		bool useD3D = false;
#ifdef NL_INDEX_BUFFER_H //new 3d
		NL3D::UDriver *driver = NL3D::UDriver::createDriver(icon,useD3D);
#else
		driver = NL3D::UDriver::createDriver(icon);
#endif
		nlassert(driver);
		NL3D::UDriver::CMode mode;

		nlinfo("Start initializing the 3D system");
		gDriver=new NeLDriver(driver);
		gDriver->init();
		
		// create the CEGUI renderer.
		AngelScriptModule *asScriptModule;
		try {
			gGuiRenderer = createNelRenderer(driver, true);

			// Create the AngelScript module.
			asScriptModule = new AngelScriptModule("CEGUI");

			// Initialize the system, tell it to use AngelScript and give it a config file.
			gGuiSystem = new CEGUI::System(	gGuiRenderer, 0, 0,
											dynamic_cast<CEGUI::ScriptModule *>(asScriptModule),
											"cegui.config");

		} catch(CEGUI::GenericException &e) {
			nlerror("Caught CEGUI Exception during Renderer or System creation: %s", e.getMessage().c_str());
		}

		// Create the NeL renderer.
		CEGUI::NeLRenderer *rndr = (CEGUI::NeLRenderer *)gGuiRenderer;
		rndr->deactivateInput();
		rndr->captureCursor(true);

		// Create an input driver to inject events into CEGUI.
		NeLInputDriver *inputDrv = new NeLInputDriver();
		inputDrv->addToServer(driver->EventServer);
		inputDrv->setScreenMode(driver->getWindowWidth(),driver->getWindowHeight(),(float)mode.Depth);
		inputDrv->setDriver(driver);
		inputDrv->activate();
		
		// load some GUI stuff for demo.
		nlinfo("Start up and configure the GUI system.");
		try	{
			using namespace CEGUI;
			
			// Load an image to use as a background
			ImagesetManager::getSingleton().createImagesetFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

			// Run the script that sets up the windows.
			const WWSCRIPT::Script *scr = WWSCRIPT::ScriptManager::getInstance().getScript("CEGUI");
			scr->getFunction("CEGUIMainMenuInit")->getInstance()->execute();

			// here we will use a StaticImage as the root, then we can use it to place a background image
			//Window* background = WindowManager::getSingleton().createWindow("TaharezLook/StaticImage", "background_wnd");
			//Window* background = WindowManager::getSingleton().getWindow("background_wnd");
			// set position and size
			//background->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0)));
			//background->setSize(UVector2(cegui_reldim(1), cegui_reldim( 1)));

			initDemoEventWiring();
		} catch(CEGUI::Exception &e) {	// catch to prevent exit (errors will be logged).
			nlerror("exception %s", e.getMessage().c_str()); 
		}

		NLMISC::CRGBA pClearColor(0,0,0,255);
		const CEGUI::Font *fnt = gGuiSystem->getDefaultFont();
		gStopDemo=false;
		// BEGIN MAIN LOOP
		//NLMISC::CHTimer::startBench();
		//NLMISC::CHTimer frameTimer("MAIN_DemoLoop");
		//NLMISC::CHTimer renderGUITimer("MAIN_RenderGUI");
		nlinfo("starting main loop.");
		while(!gStopDemo) {
			//frameTimer.before();
			// stop the demo if the driver stops.
			if(!gDriver->getDriver().isActive()) {
				gStopDemo=true;
				continue;
			}
			gDriver->getDriver().clearBuffers(pClearColor);
			// RUN UPDATES
			//   3D
			gDriver->update();

			// handle some input
			if(gDriver->getDriver().AsyncListener.isKeyPushed(NLMISC::KeyESCAPE)) {
				gStopDemo=true;
			}

			// RUN RENDERS
			// draw display
			//renderGUITimer.before();
			gGuiSystem->renderGUI();
			//renderGUITimer.after();

			//   3D
			gDriver->render();
			//   End 3D
			//   3D Swap ** ALWAYS THE LAST PART
			gDriver->getDriver().swapBuffers();
			if(gDriver->getDriver().AsyncListener.isKeyPushed(NLMISC::KeyF2)) {
				NLMISC::CBitmap btm;
				gDriver->getDriver().getBuffer(btm);
				std::string filename = NLMISC::CFile::findNewFile("screenshot.jpg");
				NLMISC::COFile fs(filename);
				btm.writeJPG(fs);
				nlinfo("Screenshot '%s' saved", filename.c_str());
			}
			//   3D Swap ** NOTHING AFTER THIS
			//frameTimer.after();
			if(gDriver->getDriver().AsyncListener.isKeyPushed(NLMISC::KeyF3)) {
				NLMISC::CHTimer::display();
			}
		}
		// END MAIN LOOP
	} catch(NLMISC::Exception &e) {
		fprintf (stderr,"main trapped an exception: '%s'", e.what ());
	}
	return 0;
}


/*************************************************************************
	Sample sub-class for ListboxTextItem that auto-sets the selection brush
	image.
*************************************************************************/
class MyListItem : public CEGUI::ListboxTextItem
{
public:
	MyListItem(const CEGUI::String& text) : ListboxTextItem(text)
	{
		setSelectionBrushImage((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MultiListSelectionBrush");
	}
};



/*************************************************************************
	Perform required event hook-ups for this demo.
*************************************************************************/
void initDemoEventWiring(void)
{
	using namespace CEGUI;

	WindowManager::getSingleton().getWindow("Demo7/Window1/Quit")->
		subscribeEvent(PushButton::EventClicked, handleQuit);

	WindowManager::getSingleton().getWindow("Demo7/Window1/Slider1")->
		subscribeEvent(Slider::EventValueChanged, handleSlider);

	WindowManager::getSingleton().getWindow("Demo7/Window1/Checkbox")->
		subscribeEvent(Checkbox::EventCheckStateChanged, handleCheck);

	WindowManager::getSingleton().getWindow("Demo7/Window1/Radio1")->
		subscribeEvent(RadioButton::EventSelectStateChanged, handleRadio);

	WindowManager::getSingleton().getWindow("Demo7/Window1/Radio2")->
		subscribeEvent(RadioButton::EventSelectStateChanged, handleRadio);

	WindowManager::getSingleton().getWindow("Demo7/Window1/Radio3")->
		subscribeEvent(RadioButton::EventSelectStateChanged, handleRadio);

}

bool handleQuit(const CEGUI::EventArgs& e)
{
	gStopDemo=true;

	return true;
}

bool handleSlider(const CEGUI::EventArgs& e)
{
	using namespace CEGUI;

	float val = ((Slider*)((const WindowEventArgs&)e).window)->getCurrentValue();

	((ProgressBar*)WindowManager::getSingleton().getWindow("Demo7/Window2/Progbar1"))->setProgress(val);
	((ProgressBar*)WindowManager::getSingleton().getWindow("Demo7/Window2/Progbar2"))->setProgress(1.0f - val);

	WindowManager::getSingleton().getWindow("root_wnd")->setAlpha(val);

	return true;
}

bool handleRadio(const CEGUI::EventArgs& e)
{
	using namespace CEGUI;

	CEGUI::uint id = ((RadioButton*)((const WindowEventArgs&)e).window)->getSelectedButtonInGroup()->getID();

	if (id == 0)
	{
		WindowManager::getSingleton().getWindow("Demo7/Window2/Image1")->setProperty("Image", "set:WerewolfGuiImagery image:CeguiLogo");
	}
	else if (id == 1)
	{
		WindowManager::getSingleton().getWindow("Demo7/Window2/Image1")->setProperty("Image", "set:WerewolfGuiImagery image:NeLLogo");
	}
	else
	{
		WindowManager::getSingleton().getWindow("Demo7/Window2/Image1")->setProperty("Image", "set:WerewolfGuiImagery image:WerewolfLogo");
	}


	return true;
}

bool handleCheck(const CEGUI::EventArgs& e)
{
	using namespace CEGUI;

	if (((Checkbox*)((const WindowEventArgs&)e).window)->isSelected())
	{
		WindowManager::getSingleton().getWindow("Demo7/Window3")->show();
	}
	else
	{
		WindowManager::getSingleton().getWindow("Demo7/Window3")->hide();
	}

	return true;
}


