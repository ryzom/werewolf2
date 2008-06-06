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

#define USE_JPEG

#include "globals.h"
#include "resource.h"

//
// System Includes
//
#ifdef NL_OS_WINDOWS
#	include <windows.h>
#	undef min
#	undef max
	HINSTANCE ghInstance = 0;
#endif
#include <string>

//
// CEGUI Includes
//
#include "renderers/NeLRenderer/nelrenderer.h"
#include "CEGUI.h"
#include "NeLDriver.h"

#include <nel/misc/hierarchical_timer.h>
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
CEGUI::System		*gGuiSystem;
CEGUI::NeLRenderer	*gGuiRenderer;
bool gStopDemo;

void createDemoWindows();
void initDemoEventWiring();
bool handleQuit(const CEGUI::EventArgs& e);
bool handleSlider(const CEGUI::EventArgs& e);
bool handleRadio(const CEGUI::EventArgs& e);
bool handleCheck(const CEGUI::EventArgs& e);

#ifdef NL_OS_WINDOWS
int WINAPI WinMain( HINSTANCE hInstance, 
									 HINSTANCE hPrevInstance, 
									 LPSTR lpCmdLine, 
									 int nCmdShow ) {
	ghInstance = hInstance;
	NLMISC::CApplicationContext myApplicationContext;
#else
int main(int argc, char **argv)
{
#endif
	try {
		gScreenWidth=800;
		gScreenHeight=600;
		NLMISC::CPath::addSearchPath("datafiles",true,false);

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

		nlinfo("Start initializing the 3D system");
		gDriver=new NeLDriver(driver);
		gDriver->init();

		// start up the Gui system.
		gGuiRenderer = new CEGUI::NeLRenderer(driver);
		gGuiRenderer->addSearchPath("datafiles",true,false);
		gGuiSystem = new CEGUI::System(gGuiRenderer);
		gGuiRenderer->activateInput();
		gGuiRenderer->captureCursor(true);

		// load some GUI stuff for demo.
		nlinfo("Start up and configure the GUI system.");
		try	{
			using namespace CEGUI;
			
			Logger::getSingleton().setLoggingLevel(Insane);

			// load scheme and set up defaults
			SchemeManager::getSingleton().loadScheme("TaharezLook.scheme");
			System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

			// create the font and set it up as the default.
			System::getSingleton().setDefaultFont("Tahoma-12");

			Window* sheet = WindowManager::getSingleton().createWindow("DefaultWindow", "root_wnd");
			System::getSingleton().setGUISheet(sheet);

			// do demo stuff
			ImagesetManager::getSingleton().createImageset("werewolfgui.imageset");
	
			createDemoWindows();
			initDemoEventWiring();
		} catch(CEGUI::Exception) {	// catch to prevent exit (errors will be logged).
			;
		}

		NLMISC::CRGBA pClearColor(0,0,0,255);
		const CEGUI::Font *fnt = gGuiSystem->getDefaultFont();
		gStopDemo=false;
		// BEGIN MAIN LOOP
		NLMISC::CHTimer::startBench();
		NLMISC::CHTimer frameTimer("MAIN_DemoLoop");
		NLMISC::CHTimer renderGUITimer("MAIN_RenderGUI");
		while(!gStopDemo) {
			frameTimer.before();
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
			renderGUITimer.before();
			gGuiSystem->renderGUI();
			renderGUITimer.after();

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
			frameTimer.after();
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

void createDemoWindows(void)
{
	using namespace CEGUI;

	WindowManager& winMgr = WindowManager::getSingleton();
	Window* root = winMgr.getWindow("root_wnd");

	FrameWindow* fwnd1 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Demo7/Window1");
	root->addChildWindow(fwnd1);
	fwnd1->setMinimumSize(Size(0.2f, 0.2f));
	fwnd1->setMaximumSize(Size(0.8f, 0.8f));
	fwnd1->setPosition(Point(0.2f, 0.2f));
	fwnd1->setSize(Size(0.5f, 0.5f));
	fwnd1->setText("Demo 7 - Window 1");
	fwnd1->setCloseButtonEnabled(false);

	FrameWindow* fwnd2 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Demo7/Window2");
	root->addChildWindow(fwnd2);
	fwnd2->setMinimumSize(Size(0.2f, 0.2f));
	fwnd2->setMaximumSize(Size(0.8f, 0.8f));
	fwnd2->setPosition(Point(0.4f, 0.1f));
	fwnd2->setSize(Size(0.5f, 0.5f));
	fwnd2->setText("Demo 7 - Window 2");
	fwnd2->setCloseButtonEnabled(false);

	FrameWindow* fwnd3 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Demo7/Window3");
	root->addChildWindow(fwnd3);
	fwnd3->setMinimumSize(Size(0.2f, 0.2f));
	fwnd3->setMaximumSize(Size(0.8f, 0.8f));
	fwnd3->setPosition(Point(0.25f, 0.4f));
	fwnd3->setSize(Size(0.5f, 0.5f));
	fwnd3->setText("Demo 7 - Window 3");
	fwnd3->setCloseButtonEnabled(false);

	PushButton* btn = (PushButton*)winMgr.createWindow("TaharezLook/Button", "Demo7/Window1/Quit");
	fwnd1->addChildWindow(btn);
	btn->setMaximumSize(Size(1.0f, 1.0f));
	btn->setPosition(Point(0.02f, 0.1f));
	btn->setSize(Size(0.25f, 0.1f));
	btn->setText("Exit Demo");
	btn->setNormalTextColour(colour(1, 0, 0));

	RadioButton* rbtn = (RadioButton*)winMgr.createWindow("TaharezLook/RadioButton", "Demo7/Window1/Radio1");
	fwnd1->addChildWindow(rbtn);
	rbtn->setMaximumSize(Size(1.0f, 1.0f));
	rbtn->setPosition(Point(0.02f, 0.23f));
	rbtn->setSize(Size(0.25f, 0.075f));
	rbtn->setText("Option 1");
	rbtn->setID(0);

	rbtn = (RadioButton*)winMgr.createWindow("TaharezLook/RadioButton", "Demo7/Window1/Radio2");
	fwnd1->addChildWindow(rbtn);
	rbtn->setMaximumSize(Size(1.0f, 1.0f));
	rbtn->setPosition(Point(0.02f, 0.306f));
	rbtn->setSize(Size(0.25f, 0.075f));
	rbtn->setText("Option 2");
	rbtn->setID(1);

	rbtn = (RadioButton*)winMgr.createWindow("TaharezLook/RadioButton", "Demo7/Window1/Radio3");
	fwnd1->addChildWindow(rbtn);
	rbtn->setMaximumSize(Size(1.0f, 1.0f));
	rbtn->setPosition(Point(0.02f, 0.382f));
	rbtn->setSize(Size(0.25f, 0.075f));
	rbtn->setText("Option 3");
	rbtn->setID(2);

	Checkbox* cbox = (Checkbox*)winMgr.createWindow("TaharezLook/Checkbox", "Demo7/Window1/Checkbox");
	fwnd1->addChildWindow(cbox);
	cbox->setMaximumSize(Size(1.0f, 1.0f));
	cbox->setPosition(Point(0.02f, 0.48f));
	cbox->setSize(Size(0.45f, 0.075f));
	cbox->setText("Enable some option");
	cbox->setSelected(true);

	StaticText* stxt = (StaticText*)winMgr.createWindow("TaharezLook/StaticText", "Demo7/Window1/Text1");
	fwnd1->addChildWindow(stxt);
	stxt->setMaximumSize(Size(1.0f, 1.0f));
	stxt->setPosition(Point(0.5f, 0.1f));
	stxt->setSize(Size(0.45f, 0.5f));
	stxt->setText("This is a static text widget.  More examples of this, and the static image, can be seen in the frame-rate / debug overlay.");
	stxt->setFrameEnabled(false);
	stxt->setBackgroundEnabled(false);
	stxt->setHorizontalFormatting(StaticText::WordWrapCentred);

	Editbox* ebox = (Editbox*)winMgr.createWindow("TaharezLook/Editbox", "Demo7/Window1/Editbox");
	fwnd1->addChildWindow(ebox);
	ebox->setMaximumSize(Size(1.0f, 1.0f));
	ebox->setPosition(Point(0.2f, 0.6f));
	ebox->setSize(Size(0.6f, 0.1f));
	ebox->setText("Single-line editbox");

	Scrollbar* sbar = (Scrollbar*)winMgr.createWindow("TaharezLook/LargeVerticalScrollbar", "Demo7/Window1/Scrollbar1");
	fwnd1->addChildWindow(sbar);
	sbar->setMinimumSize(Size(0.01f, 0.1f));
	sbar->setMaximumSize(Size(0.03f, 1.0f));
	sbar->setPosition(Point(0.06f, 0.6f));
	sbar->setSize(Size(0.06f, 0.38f));

	Slider* sldr = (Slider*)winMgr.createWindow("TaharezLook/Slider", "Demo7/Window1/Slider1");
	fwnd1->addChildWindow(sldr);
	sldr->setMinimumSize(Size(0.01f, 0.1f));
	sldr->setMaximumSize(Size(0.03f, 1.0f));
	sldr->setPosition(Point(0.9f, 0.625f));
	sldr->setSize(Size(0.03f, 0.28f));
	sldr->setCurrentValue(1.0f);

	ProgressBar* pbar = (ProgressBar*)winMgr.createWindow("TaharezLook/ProgressBar", "Demo7/Window2/Progbar1");
	fwnd2->addChildWindow(pbar);
	pbar->setMaximumSize(Size(1.0f, 1.0f));
	pbar->setPosition(Point(0.04f, 0.1f));
	pbar->setSize(Size(0.94f, 0.05f));
	pbar->setProgress(0.33f);

	pbar = (ProgressBar*)winMgr.createWindow("TaharezLook/AlternateProgressBar", "Demo7/Window2/Progbar2");
	fwnd2->addChildWindow(pbar);
	pbar->setMaximumSize(Size(1.0f, 1.0f));
	pbar->setPosition(Point(0.04f, 0.16f));
	pbar->setSize(Size(0.94f, 0.05f));
	pbar->setProgress(0.7f);

	Listbox* lbox = (Listbox*)winMgr.createWindow("TaharezLook/Listbox", "Demo7/Window2/Listbox");
	fwnd2->addChildWindow(lbox);
	lbox->setMaximumSize(Size(1.0f, 1.0f));
	lbox->setPosition(Point(0.04f, 0.25f));
	lbox->setSize(Size(0.42f, 0.3f));

	Combobox* cbobox = (Combobox*)winMgr.createWindow("TaharezLook/Combobox", "Demo7/Window2/Combobox");
	fwnd2->addChildWindow(cbobox);
	cbobox->setMaximumSize(Size(1.0f, 1.0f));
	cbobox->setPosition(Point(0.5f, 0.25f));
	cbobox->setSize(Size(0.42f, 0.45f));

	MultiColumnList* mclbox = (MultiColumnList*)winMgr.createWindow("TaharezLook/MultiColumnList", "Demo7/Window2/MultiColumnList");
	fwnd2->addChildWindow(mclbox);
	mclbox->setMaximumSize(Size(1.0f, 1.0f));
	mclbox->setPosition(Point(0.05f, 0.6f));
	mclbox->setSize(Size(0.9f, 0.38f));
	mclbox->setSelectionMode(MultiColumnList::RowSingle);

	StaticImage* simg = (StaticImage*)winMgr.createWindow("TaharezLook/StaticImage", "Demo7/Window2/Image1");
	fwnd2->addChildWindow(simg);
	simg->setMaximumSize(Size(1.0f, 1.0f));
	simg->setPosition(Point(0.5f, 0.32f));
	simg->setSize(Size(0.5f, 0.30f));
	simg->setFrameEnabled(false);
	simg->setBackgroundEnabled(false);
	simg->setImage("WerewolfGuiImagery", "CeguiLogo");

	MultiLineEditbox* mleb = (MultiLineEditbox*)winMgr.createWindow("TaharezLook/MultiLineEditbox", "Demo7/Window3/MLEditbox");
	fwnd3->addChildWindow(mleb);
	mleb->setMaximumSize(Size(1.0f, 1.0f));
	mleb->setPosition(Point(0.02f, 0.1f));
	mleb->setSize(Size(0.96f, 0.85f));
	mleb->setText("Multi-line edit box.  Edit me!");

	// List box setup
	mclbox->addColumn("Server Name", 0, 0.33f);
	mclbox->addColumn("Address", 1, 0.5f);
	mclbox->addColumn("Ping", 2, 0.2f);

	lbox->addItem(new MyListItem("Listbox Item 1"));
	lbox->addItem(new MyListItem("Listbox Item 2"));
	lbox->addItem(new MyListItem("Listbox Item 3"));
	lbox->addItem(new MyListItem("Listbox Item 4"));
	lbox->addItem(new MyListItem("Listbox Item 5"));
	lbox->addItem(new MyListItem("Listbox Item 6"));
	lbox->addItem(new MyListItem("Listbox Item 7"));

	cbobox->addItem(new MyListItem("Combobox Item 1"));
	cbobox->addItem(new MyListItem("Combobox Item 2"));
	cbobox->addItem(new MyListItem("Combobox Item 3"));
	cbobox->addItem(new MyListItem("Combobox Item 4"));
	cbobox->addItem(new MyListItem("Combobox Item 5"));
	cbobox->addItem(new MyListItem("Combobox Item 6"));
	cbobox->addItem(new MyListItem("Combobox Item 7"));
	cbobox->addItem(new MyListItem("Combobox Item 8"));
	cbobox->addItem(new MyListItem("Combobox Item 9"));
	cbobox->addItem(new MyListItem("Combobox Item 10"));

	mclbox->addRow();
	mclbox->addRow();
	mclbox->addRow();
	mclbox->addRow();
	mclbox->addRow();

	mclbox->setItem(new MyListItem("Laggers World"), 0, 0);
	mclbox->setItem(new MyListItem("yourgame.some-server.com"), 1, 0);
	mclbox->setItem(new MyListItem("1000ms"), 2, 0);

	mclbox->setItem(new MyListItem("Super-Server"), 0, 1);
	mclbox->setItem(new MyListItem("whizzy.fakenames.net"), 1, 1);
	mclbox->setItem(new MyListItem("8ms"), 2, 1);

	mclbox->setItem(new MyListItem("Cray-Z-Eds"), 0, 2);
	mclbox->setItem(new MyListItem("crayzeds.notarealserver.co.uk"), 1, 2);
	mclbox->setItem(new MyListItem("43ms"), 2, 2);

	mclbox->setItem(new MyListItem("Fake IPs"), 0, 3);
	mclbox->setItem(new MyListItem("123.320.42.242"), 1, 3);
	mclbox->setItem(new MyListItem("63ms"), 2, 3);

	mclbox->setItem(new MyListItem("Yet Another Game Server"), 0, 4);
	mclbox->setItem(new MyListItem("abc.abcdefghijklmn.org"), 1, 4);
	mclbox->setItem(new MyListItem("284ms"), 2, 4);

	// work-around a small bug (oops!)
	mclbox->setSelectionMode(MultiColumnList::RowMultiple);
	mclbox->setSelectionMode(MultiColumnList::RowSingle);
}


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
		((StaticImage*)WindowManager::getSingleton().getWindow("Demo7/Window2/Image1"))->setImage("WerewolfGuiImagery", "CeguiLogo");
	}
	else if (id == 1)
	{
		((StaticImage*)WindowManager::getSingleton().getWindow("Demo7/Window2/Image1"))->setImage("WerewolfGuiImagery", "NeLLogo");
	}
	else
	{
		((StaticImage*)WindowManager::getSingleton().getWindow("Demo7/Window2/Image1"))->setImage("WerewolfGuiImagery", "WerewolfLogo");
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


