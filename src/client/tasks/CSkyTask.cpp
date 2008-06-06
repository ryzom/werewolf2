/**
 * \file CSkyTask.cpp
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

//
// System Includes
//
#include "stdpch.h"

//
// NeL Includes
//
#include <nel/3d/u_scene.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_texture.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_cloud_scape.h>
#include <nel/3d/u_text_context.h>

//
// Werewolf Includes
//
#include "CSkyTask.h"

#include "tasks/C3DTask.h"
#include "tasks/CConfigTask.h"
#include "tasks/CResourceTask.h"
#include "tasks/CTimeTask.h"

//
// Namespaces
//
using namespace NL3D;
using namespace NLMISC;

namespace WWCLIENT {

CSkyTask::CSkyTask():ITask() {
	shapeName("sky.shape");

	m_SkyScene   = 0;
	m_SkyMesh    = 0;
	m_CloudScape = 0;
}


void CSkyTask::init() {
	nlinfo("starting sky task.");
	m_SkyScene = C3DTask::instance().driver().createScene(false);

	m_SkyScene->getCam().setPerspective(degToRad(CConfigTask::instance().configFile().getVar("Fov").asFloat()), 1.33f, 0.15f, 3000.0f);
	m_SkyScene->getCam().setTransformMode(NL3D::UTransformable::DirectMatrix);

	nlinfo("Loading sky mesh.");
	std::string res = CResourceTask::instance().get(shapeName());
	m_SkyMesh = m_SkyScene->createInstance(res);
	if(!m_SkyMesh.empty()) {
		m_SkyMesh.setTransformMode(NL3D::UTransformable::DirectMatrix);
		m_SkyMesh.setMatrix(NLMISC::CMatrix::Identity);
		m_SkyMesh.getMaterial(0).setZWrite(false);
	}

	nlinfo("initing clouds.");
	if(CConfigTask::instance().configFile().getVar("DisplayClouds").asInt() == 1) {
		nlinfo("Create cloud scape.");
		m_CloudScape = m_SkyScene->createCloudScape();
		nlinfo("finished creating cloudscape.");
		NL3D::SCloudScapeSetup css;
		css.NbCloud = 50;
		css.CloudSpeed = 8.0f;
		css.WindSpeed = 1.5f;

		css.Ambient = NLMISC::CRGBA (152, 158, 206, 255);
		css.Diffuse = NLMISC::CRGBA (236, 231, 193, 255);

		//css.Ambient = NLMISC::CRGBA (120,150,155,255);
		//css.Diffuse = NLMISC::CRGBA (220,250,255,255);
		nlinfo("cloud scape settings.");
		m_CloudScape->setNbCloudToUpdateIn80ms(1);
		m_CloudScape->setQuality(160.0);
		//nelCloudScape->setDebugQuad (true);
		nlinfo("Init cloud scape.");
		m_CloudScape->init(&css);	
	}
	nlinfo("ending sky task.");
}

void CSkyTask::update() {
	NLMISC::CMatrix skyCameraMatrix;
	skyCameraMatrix.identity();
	skyCameraMatrix = C3DTask::instance().scene().getCam().getMatrix();
	skyCameraMatrix.setPos(NLMISC::CVector::Null);
	
	m_SkyScene->getCam().setMatrix(skyCameraMatrix);
	
	m_SkyScene->animate(CTimeTask::instance().time());
	
	if (m_CloudScape) {
		// WARNING this function work with screen
		m_CloudScape->anim(CTimeTask::instance().deltaTime()); 
	}
}

void CSkyTask::render() {
	// Must clear ZBuffer For incoming rendering.
	C3DTask::instance().driver().clearZBuffer();
	m_SkyScene->render();
	
	if (m_CloudScape)
		m_CloudScape->render();
}

void CSkyTask::release() {
	if(!m_SkyMesh.empty()) {
		m_SkyScene->deleteInstance(m_SkyMesh);
	}

	if(m_CloudScape) {
		m_SkyScene->deleteCloudScape(m_CloudScape);
		m_CloudScape = 0;
	}

	if(m_SkyScene) {
		C3DTask::instance().driver().deleteScene(m_SkyScene);
		m_SkyScene = 0;
	}
}


void CSkyTask::shapeName(std::string shapeName) {
	if(shapeName.empty()) return;
	m_ShapeName = shapeName;
}

std::string CSkyTask::shapeName() {
	return m_ShapeName;
}

}; // END NAMESPACE WWCLIENT
