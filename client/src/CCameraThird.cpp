/**
 * \file CCameraThird.cpp
 * \date October 2004
 * \author Matt Raykowski
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
#include <nel/misc/smart_ptr.h>

//
// Werewolf Includes
//
#include "CCameraThird.h"
//#include "CInterpolator.h"
#include "wwcommon/CGeneralInterpolator.h"

#include "interfaces/IBaseCamera.h"
#include "CActorProxy.h"
#include "CSimulationImpl.h"

//#include "tasks/CEntityTask.h"
#include "tasks/C3DTask.h"
#include "tasks/CTimeTask.h"
#include "tasks/CInputTask.h"


//
// Namespaces
//

namespace WWCLIENT {

//
// Class
//

CCameraThird::CCameraThird() {
	;
}

void CCameraThird::init() {
	m_Camera=C3DTask::instance().scene().getCam();
	m_Camera.setTransformMode(NL3D::UTransformable::DirectMatrix);
	m_Camera.setPerspective ((float)NLMISC::Pi/2.f, 1.33f, 0.1f, 1000);
	m_ViewLagBehind = 20.0f;
	m_ViewAheadDistance = 4.0f;
	m_ViewHeightOffset = 3.0f;

	m_ViewPitch = 0.4f;
	m_ViewYaw = 0.0f;
	m_YawVelocity = 0.0f;
	m_PitchVelocity = 0.0;
	m_MaxRotSpeed = 12.0f;
	m_RotBrakingFactor = 0.9f;
	m_RotAccel = 1.0f;
	m_CameraLag = 0.1f;

	m_LastTime = CTimeTask::instance().time();
}

void CCameraThird::update() {
	//nlinfo("Updating camera: from: %f/%f/%f to: %f/%f/%f",eyePos.x,eyePos.y,eyePos.z,entself->getPosition().x,entself->getPosition().y,entself->getPosition().z);

	// this should actually be elsewhere
	int dy = (int)CInputTask::instance().kbDown(NLMISC::KeyRIGHT)-(int)CInputTask::instance().kbDown(NLMISC::KeyLEFT);
	int dp = (int)CInputTask::instance().kbDown(NLMISC::KeyUP)-(int)CInputTask::instance().kbDown(NLMISC::KeyDOWN);
	rotate(dp, dy);

	calculateTarget();
	move();
	//m_Camera.lookAt(eyePos,NLMISC::CVector(0.0f,0.0f,0.0f));
}

void CCameraThird::render() {
	;
}

void CCameraThird::release() {
	;
}

void CCameraThird::stop() {
	;
}

std::string CCameraThird::name() {
	return "CCameraThird";
}

float CCameraThird::getViewingDistance() {
	return m_ViewLagBehind;
}

void CCameraThird::setViewingDistance(float dist) {
	m_ViewLagBehind = dist;
}

float CCameraThird::getViewAheadDistance() {
	return m_ViewAheadDistance;
}

void CCameraThird::setViewAheadDistance(float dist) {
	m_ViewAheadDistance = dist;
}

void CCameraThird::calculateTarget() {
	CActorProxy *actor=getClientSimulation()->getSelfSob();
	if(actor==NULL)
		return; // no entity has been introduced into the world yet.

	WWCOMMON::CPerformer::CPerformerInterpKey key = actor->getInterpolator()->getSmoothKey();
	//WWCOMMON::CPerformer::CPerformerInterpKey key = *(actor->getCurrentKey());

	NLMISC::CMatrix mat = key.getMatrix();
	mat.rotateZ(m_ViewYaw);
	NLMISC::CVector j = mat.getJ();
	float pitch = m_ViewPitch;

	m_LookAtPoint = mat.getPos();
	m_LookAtPoint.x += m_ViewAheadDistance*j.x;
	m_LookAtPoint.y += m_ViewAheadDistance*j.y;
	m_LookAtPoint.z += m_ViewHeightOffset;

	mat = key.getMatrix();
	mat.rotateZ(m_ViewYaw);
	j = mat.getJ();
	m_EyePos = mat.getPos();
	m_EyePos.x += m_ViewAheadDistance*j.x;
	m_EyePos.y += m_ViewAheadDistance*j.y;
	m_EyePos.z += m_ViewHeightOffset;
	m_EyePos.x -= j.x*cos(pitch)*m_ViewLagBehind;
	m_EyePos.y -= j.y*cos(pitch)*m_ViewLagBehind;
	m_EyePos.z += sin(pitch)*m_ViewLagBehind;
}

void CCameraThird::move() {
	m_Camera.lookAt(m_EyePos,m_LookAtPoint);
}

void CCameraThird::rotate(int dp, int dy) {
	float elapsed = (float)(CTimeTask::instance().time() - m_LastTime);
	m_LastTime = CTimeTask::instance().time();

	m_YawVelocity += m_RotAccel*elapsed*dy;
	if (m_YawVelocity>m_MaxRotSpeed) m_YawVelocity=m_MaxRotSpeed;
	if (m_YawVelocity<-m_MaxRotSpeed) m_YawVelocity=-m_MaxRotSpeed;
	if (dy==0) m_YawVelocity=m_YawVelocity*m_RotBrakingFactor;
	m_ViewYaw += m_YawVelocity*elapsed;

	m_PitchVelocity += m_RotAccel*elapsed*dp;
	if (m_PitchVelocity>m_MaxRotSpeed) m_PitchVelocity=m_MaxRotSpeed;
	if (m_PitchVelocity<-m_MaxRotSpeed) m_PitchVelocity=-m_MaxRotSpeed;
	if (dp==0) m_PitchVelocity=m_PitchVelocity*m_RotBrakingFactor;
	m_ViewPitch += m_PitchVelocity*elapsed;
}

}; // END NAMESPACE WWCLIENT
