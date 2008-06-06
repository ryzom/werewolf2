#include <nel/misc/types_nl.h>

#ifdef NL_OS_WINDOWS
#	include <windows.h>
#	undef min
#	undef max
#endif

#include <cstdio>

#include <nel/misc/time_nl.h>
#include <nel/misc/path.h>
#include <nel/misc/config_file.h>
#include <nel/misc/vectord.h>
#include <3d/event_mouse_listener.h>
#include <3d/mesh_instance.h>
#include <nel/3d/u_point_light.h>

#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_instance.h>
#include <nel/misc/value_smoother.h>

#include <GpuManager/GpuProgramFactory.h>
#include <PropertyManager/PropertyManager.h>
#include "../Test01/GpuOrenNayarFP.h"

using namespace std;
using namespace NLMISC;
using namespace NL3D;

CConfigFile			 ConfigFile;

// The 3d driver
UDriver					*Driver = NULL;

// This is the main scene
UScene					*Scene = NULL;

// The previous and current frame dates
TTime					LastTime, NewTime;

// This variable is used to display text on the screen
UTextContext			*TextContext = NULL;

UInstance				Instance;
UPointLight				pl;

UCamera					Camera;

// true if you want to exit the main loop
static bool				NeedExit = false;

CEvent3dMouseListener	MouseListener;

NLMISC::CValueSmootherTemplate<double> m_DeltaTimeSmooth;

void init();
void mainLoop();

int main(int argc, char **argv) {
	init();
	mainLoop();
	// Release the 3d driver
	MouseListener.removeFromServer(Driver->EventServer);
	Scene->deleteInstance(Instance);

	delete Driver;

	// Exit
	return EXIT_SUCCESS;
}

void init() {
	GpuProgramFactory::instance().registerProgram("OrenNayarFP", GpuOrenNayarFP::create);

	ConfigFile.load ("test.cfg");
	CPath::addSearchPath(ConfigFile.getVar("DataPath").asString (), true, false);
	Driver = UDriver::createDriver();

	// Create the window with config file values
	Driver->setDisplay (UDriver::CMode(ConfigFile.getVar("ScreenWidth").asInt(), 
									   ConfigFile.getVar("ScreenHeight").asInt(),
									   ConfigFile.getVar("ScreenDepth").asInt(),
									   ConfigFile.getVar("ScreenFull").asInt()==0));
	Driver->setSwapVBLInterval(0);

	// Set the cache size for the font manager (in bytes)
	Driver->setFontManagerMaxMemory (2000000);

	MouseListener.addToServer(Driver->EventServer);

	// Create a Text context for later text rendering
	TextContext = Driver->createTextContext (CPath::lookup(ConfigFile.getVar("FontName").asString ()));
	
	// Create a scene
	Scene = Driver->createScene(false);

	nlinfo ("Shader system prototype first draft test 02");
	nlinfo ("");
	nlinfo ("Press ESC to exit...");
	// Get the current time

	Instance = Scene->createInstance (ConfigFile.getVar("Model").asString());
	CConfigFile::CVar cv;

	pl = Scene->createPointLight();
	cv = ConfigFile.getVar("LightPosition");
	pl.setPos(cv.asFloat(0), cv.asFloat(1), cv.asFloat(2));
	cv = ConfigFile.getVar("LightColor");
	pl.setDiffuse(CRGBA(cv.asInt(0), cv.asInt(1), cv.asInt(2)));

	cv=ConfigFile.getVar("SunAmbientColor");
	NLMISC::CRGBA landscapeSunAmbient(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunDiffuseColor");
	NLMISC::CRGBA landscapeSunDiffuse(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunSpecularColor");
	NLMISC::CRGBA landscapeSunSpecular(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunDirection");
	NLMISC::CVector sunDirection(cv.asFloat(0), cv.asFloat(1), cv.asFloat(2));

	Scene->setSunDirection(sunDirection);
	Scene->setSunAmbient(landscapeSunAmbient);
	Scene->setSunDiffuse(landscapeSunDiffuse);
	Scene->setSunSpecular(landscapeSunSpecular);
	Scene->enableLightingSystem(true);

	Camera = Scene->getCam();
	Camera.setTransformMode (UTransformable::DirectMatrix);
	Camera.setPerspective ((float)Pi/2.f, 1.33f, 0.1f, 1000);

	CAABBox bbox;
	Instance.getShapeAABBox(bbox);
	Camera.lookAt (CVector(0.7f, -0.7f, 0.7f) * bbox.getRadius(), CVector(0, 0, 0));

	MouseListener.setMatrix (Camera.getMatrix());
	MouseListener.setFrustrum (Camera.getFrustum());
	MouseListener.setViewport (Driver->getViewport());
	MouseListener.setHotSpot (CVector(0,0,0));
	MouseListener.setMouseMode (CEvent3dMouseListener::edit3d);
	MouseListener.enableModelMatrixEdition(false);
	MouseListener.enableTranslateXYInWorld(false);

	NewTime = CTime::getLocalTime();
}

void mainLoop() {
	while ((!NeedExit) && Driver->isActive()) {
		// Clear all buffers
		Driver->clearBuffers (CRGBA (0, 0, 0));

		// Update the time counters
		LastTime = NewTime;
		NewTime = CTime::getLocalTime();

		// Set new animation date
		Scene->animate (float(NewTime)/1000);

		// Render
		Scene->render ();

		// Display the frame rate
		double dt = (double)(NewTime - LastTime);
		m_DeltaTimeSmooth.addValue(dt/1000);
		float fps = m_DeltaTimeSmooth.getSmoothValue() ? 1.0 / m_DeltaTimeSmooth.getSmoothValue() : 0.0;
		TextContext->setHotSpot (UTextContext::BottomRight);
		TextContext->setColor (CRGBA(255, 255, 255, 255));
		TextContext->setFontSize (14);
		TextContext->printfAt (ConfigFile.getVar("FpsX").asFloat(),
			ConfigFile.getVar("FpsY").asFloat(), "%.2ffps %ums", fps, (uint32)dt);
		
//		TextContext->setHotSpot (UTextContext::MiddleTop);
//		TextContext->printfAt (0.5f, 0.99f, "THE TEST APPLICATION NO. 01!!!");
		Driver->swapBuffers ();

		// Pump user input messages
		Driver->EventServer.pump();

		// Manage the keyboard

		if (Driver->AsyncListener.isKeyDown (KeyESCAPE))
		{
			NeedExit = true;
		}
		Camera.setMatrix (MouseListener.getViewMatrix());
	}
	nlinfo("Exiting...");
}