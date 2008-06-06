/**
 * \file main.cpp
 * \date January 2005
 * \author Matt Raykowski
 *
 * This sample demonstrates creating custom vertex programs for any models based
 * CMeshInstance.
 */

/* Copyright, 2005 Werewolf
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

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>
#include <nel/net/service.h>
#include <nel/misc/log.h>
#include <nel/misc/displayer.h>
#include <nel/misc/window_displayer.h>
#include <nel/misc/path.h>

//
// Werewolf Includes
//
#include "CMeshVertexProgramSample.h"

//
// Class
//

//
// Standard Includes
//
#include "stdpch.h"

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
// NeL Includes
//
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_skeleton.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_texture.h>
#include <nel/misc/rgba.h>
#include <3d/shape.h>
#include <3d/mesh.h>

//
// Werewolf Includes
//	

//
// Namespaces
//

//
// Global Variables
//

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
	NLMISC::CPath::addSearchPath("data/",true,false);

	uint pScreenWidth = 800;
	uint pScreenHeight = 600;
	NL3D::UDriver *m_Driver;

	NLMISC::CRGBA pClearColor;
	pClearColor.set(0, 0, 0, 255);

	// Create a driver
	uint icon = 0;
#ifdef NL_OS_WINDOWS
	//icon = (uint)LoadIcon(ghInstance,MAKEINTRESOURCE(IDI_ICON1));
#endif
	bool useD3D = false;
#ifdef NL_INDEX_BUFFER_H //new 3d
	m_Driver = NL3D::UDriver::createDriver(icon,useD3D);
#else
	m_Driver = NL3D::UDriver::createDriver(icon);
#endif
	nlassert(m_Driver);
	m_Driver->setSwapVBLInterval(0);

	// Create the window with config file values
	if (!m_Driver->setDisplay(NL3D::UDriver::CMode(pScreenWidth, pScreenHeight, 32, true, 0))) {
		nlwarning ("Can't set display mode %d %d %d %d %d", pScreenWidth, pScreenHeight, 32, 0, 0);
		return 0;
	}

	// Set the cache size for the font manager (in bytes)
	//m_Driver->setFontManagerMaxMemory (2000000);
	//m_Driver->setAmbientColor(m_AmbientColor);
	//m_Driver->enableFog(false);
	//m_Driver->setupFog(CConfigTask::instance().configFile().getVar("FogDistMin").asFloat(),CConfigTask::instance().configFile().getVar("FogDistMax").asFloat(),m_ClearColor);

	// Create a scene and set it up
	NL3D::UScene *m_Scene = m_Driver->createScene(false);
	if (m_Scene == 0) {
		nlwarning ("Can't create a NeL UScene");
		return 0;
	}
	m_Scene->getCam().setPerspective(NLMISC::degToRad(90.0f), 1.33f, 0.01f, 300.0f);
	m_Scene->getCam().setTransformMode(NL3D::UTransformable::DirectMatrix);	
	m_Scene->enableLightingSystem(true);
	m_Scene->setShadowMapBlurSize(1);
	m_Scene->setPolygonBalancingMode(NL3D::UScene::PolygonBalancingOn);
	m_Scene->setGroupLoadMaxPolygon("Fx", 5000);

#ifdef NL_OS_WINDOWS
	HWND hWnd = (HWND )m_Driver->getDisplay();
	SetWindowText(hWnd,"Mesh Vertex Program Sample");
#endif

	NL3D::UInstance m_Instance=m_Scene->createInstance("data/mesh/gnu.shape");
	NL3D::USkeleton m_Skeleton;

	if(m_Instance.empty()) {
		nlerror("Failed loading the shape.");
		return 0;
	}
	m_Instance.setTransformMode(NL3D::UTransformable::DirectMatrix);

	m_Skeleton.cast(m_Scene->createSkeleton("data/anims/gnu.skel"));
	if(m_Skeleton.empty()) {
		nlerror("Failed to load skeleton.");
		return 0;
	} else {
		m_Skeleton.setTransformMode(NL3D::UTransformable::DirectMatrix);
		// bind the skeleton to the mesh
		m_Skeleton.bindSkin(m_Instance);
		m_Skeleton.enableCastShadowMap(true);
	}

	NLMISC::CMatrix pInstancePos, pCameraPos;
	pInstancePos.setPos(NLMISC::CVector(2,2,0));
	pCameraPos.setPos(NLMISC::CVector(-5,-5,10));
	m_Instance.setMatrix(pInstancePos);
	m_Scene->getCam().lookAt(pCameraPos.getPos(),pInstancePos.getPos());

	// figure out time
	double pTime, pFirstTime, pOldTime;
	pTime=pFirstTime=pOldTime = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());

	// set up the custom vertex program
	CMeshVertexProgramSample *sampleVP=new CMeshVertexProgramSample();
	//NL3D::IShape *shape=(IShape*)m_Instance.getObjectPtr();
	NL3D::CMesh *pMesh=NLMISC::safe_cast<NL3D::CMesh *>((NL3D::IShape *)m_Instance.getObjectPtr());
	//NL3D::IMeshVertexProgram=pMesh->getMeshGeom().getMeshVertexProgram();
	NL3D::IMeshVertexProgram *tMvp;
	/*tMvp=*/ *(pMesh->getMeshGeom().getMeshVertexProgram())=*sampleVP;
	//tMvp=sampleVP;

	// the main loop
	bool running=true;
	while(running) {
		// 3D update cycle
		if(!m_Driver->isActive())
			running=false;

		// figure out time
		pOldTime = pTime;
		double newTime = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());
		pTime = newTime - pFirstTime;

		m_Scene->animate(pTime);
		m_Driver->EventServer.pump();

		// other code update cycle
		if(m_Driver->AsyncListener.isKeyPushed(NLMISC::KeyESCAPE)) {
			running=false;
		}

		// 3D render cycle
		m_Scene->render();

		// other code render cycle

		// 3D swap.
		m_Driver->swapBuffers();
		m_Driver->clearBuffers(pClearColor);
	}
	return 0;
}
