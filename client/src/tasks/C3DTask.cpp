/**
 * \file C3DTask.cpp
 * \date June 2004
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

//
// System Includes
//
#include "stdpch.h"

#ifdef NL_OS_WINDOWS
#	include <windows.h>
#	include "../resource.h"
#	undef min
#	undef max
	extern HINSTANCE ghInstance;
#endif

//
// NeL Includes
//
#include <nel/misc/path.h>

#include <nel/3d/mesh.h>
#include <nel/3d/shape.h>
#include <nel/3d/material.h>
#include <nel/3d/register_3d.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/scene_user.h>

//
// Werewolf Includes
//
#include "wwcommon/CTaskManager.h"
#include "CConfigTask.h"
#include "CTimeTask.h"
#include "C3DTask.h"
#include "CSimulationImpl.h"

//
// Namespaces
//
using namespace std;
using namespace NL3D;
using namespace NLMISC;

#ifdef NL_OS_WINDOWS
#include "resource.h"
extern HINSTANCE ghInstance;
#endif // NL_OS_WINDOWS

namespace WWCLIENT {

void C3DTask::init() {
	m_ScreenWidth = CConfigTask::instance().configFile().getVar("ScreenWidth").asInt();
	m_ScreenHeight = CConfigTask::instance().configFile().getVar("ScreenHeight").asInt();

	CConfigFile::CVar v;
	v = CConfigTask::instance().configFile().getVar("AmbientColor");
	nlassert(v.size()==4);

	m_AmbientColor.set(v.asInt(0),v.asInt(1),v.asInt(2),v.asInt(3));
	v = CConfigTask::instance().configFile().getVar("ClearColor");
	nlassert(v.size()==4);
	m_ClearColor.set(v.asInt(0),v.asInt(1),v.asInt(2),v.asInt(3));


	// Create a driver
	uint icon = 0;
#ifdef NL_OS_WINDOWS
	icon = (uint)LoadIcon(ghInstance,MAKEINTRESOURCE(IDI_ICON1));
#endif
	bool useD3D = CConfigTask::instance().configFile().getVar("OpenGL").asInt()==0;
#ifdef NL_INDEX_BUFFER_H //new 3d
	m_Driver = UDriver::createDriver(icon,useD3D);
#else
	m_Driver = UDriver::createDriver(icon);
#endif
	nlassert(m_Driver);
	m_Driver->setSwapVBLInterval(0);

	// Create the window with config file values
	if (!m_Driver->setDisplay (UDriver::CMode(m_ScreenWidth, m_ScreenHeight,
			CConfigTask::instance().configFile().getVar("ScreenDepth").asInt(),
			CConfigTask::instance().configFile().getVar("Fullscreen").asInt()==0,
			CConfigTask::instance().configFile().getVar("ScreenFrequency").asInt()))) {
		nlwarning ("Can't set display mode %d %d %d %d %d", m_ScreenWidth, m_ScreenHeight,
			CConfigTask::instance().configFile().getVar("ScreenDepth").asInt(),
			CConfigTask::instance().configFile().getVar("Fullscreen").asInt(),
			CConfigTask::instance().configFile().getVar("ScreenFrequency").asInt());
		return;
	}

	// Set the cache size for the font manager (in bytes)
	m_Driver->setFontManagerMaxMemory (2000000);

	// Create a Text context for later text rendering
	m_TextContext = m_Driver->createTextContext (CPath::lookup("n019003l.pfb"));

	if (m_TextContext == 0) {
		nlwarning ("Can't create text context");
		return;
	}

	m_TextContext->setKeep800x600Ratio(false);

	m_Driver->setAmbientColor(m_AmbientColor);

	m_Driver->enableFog(false);
	m_Driver->setupFog(CConfigTask::instance().configFile().getVar("FogDistMin").asFloat(),CConfigTask::instance().configFile().getVar("FogDistMax").asFloat(),m_ClearColor);

	// Create a scene
	m_Scene = m_Driver->createScene(false);

	if (m_Scene == 0) {
		nlwarning ("Can't create a NeL UScene");
		return;
	}

	m_Scene->getCam().setPerspective(degToRad(CConfigTask::instance().configFile().getVar("Fov").asFloat()), 1.33f, 1.0f*GScale, 30000.0f*GScale);
	m_Scene->getCam().setTransformMode(UTransformable::DirectMatrix);
	
	m_Scene->enableLightingSystem(true);
	m_Scene->setShadowMapBlurSize(1);

	m_Scene->setPolygonBalancingMode(UScene::PolygonBalancingOn);
	m_Scene->setGroupLoadMaxPolygon("Fx", CConfigTask::instance().configFile().getVar("FxNbMaxPoly").asInt());

	// Set the default shape cache size.
	// TODO henri:everyone figure out where to get the value to initialize the shape cache.
	NL3D::CSceneUser* sceneUser = dynamic_cast<NL3D::CSceneUser*>(m_Scene);
	sceneUser->getScene().getShapeBank()->setShapeCacheSize("default", 500);

	//LevelParticle = NULL;

#ifdef NL_OS_WINDOWS
	HWND hWnd = (HWND )m_Driver->getDisplay();
	SetWindowText(hWnd,"Werewolf");
#endif
}

void C3DTask::update() {
	if(!m_Driver->isActive())
		WWCOMMON::CTaskManager::instance().exit();

	m_Scene->animate(CTimeTask::instance().time());
	m_Driver->EventServer.pump();

	/**
	 * @todo Figure out what F1 is for and turn this back on with InfoLog.
	 */
	//if(C3DTask::instance().kbPressed(KeyF1)) {
	//	CHTimer::display();
	//	CHTimer::displayHierarchicalByExecutionPathSorted(InfoLog, CHTimer::TotalTime, true, 64);
	//}
}

void C3DTask::render() {
//	nlinfo("C3DTask::render():begin render");
	C3DTask::instance().driver().enableFog(true);
	if(getClientSimulation()->getSelfSob() != NULL) {
		getClientSimulation()->getSelfSob()->render();
	}
	m_Scene->render();
	C3DTask::instance().driver().enableFog(false);
	//CEntityManager::instance().renderNames();
//	nlinfo("C3DTask::render():end render");
}

void C3DTask::release() {
	;
}

std::string C3DTask::name() {
	return "C3DTask";
}

NL3D::UDriver &C3DTask::driver() const {
	nlassert(m_Driver);
	return *m_Driver;
}

NL3D::UScene &C3DTask::scene() const {
	nlassert(m_Scene);
	return *m_Scene;
}

NL3D::UTextContext &C3DTask::textContext() const {
	nlassert(m_TextContext);
	return *m_TextContext;
}

void C3DTask::clear() {
	m_Driver->clearBuffers (m_ClearColor);
}

void C3DTask::clearColor(CRGBA color) {
	m_ClearColor = color;
}

void C3DTask::captureCursor(bool b) {
	; // this is done in the NeLRenderer right now.
}

uint16 C3DTask::getScreenWidth() {
	return m_ScreenWidth;
}

uint16 C3DTask::getScreenHeight() {
	return m_ScreenHeight;
}

}; // END NAMESPACE WWCLIENT
