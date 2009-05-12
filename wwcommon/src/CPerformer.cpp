/**
 * \file CPerformer.cpp
 * \date April 2005
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
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/vector.h>
#include <nel/misc/vectord.h>
#include <nel/misc/matrix.h>
#include <nel/pacs/u_move_primitive.h>

//
// Werewolf Includes
//

// sobs
#include "CPerformer.h"
#include "CSobFactory.h"
#include "ISimulationObj.h"

// states
#include "CStateManager.h"
#include "ISimulationState.h"
#include "CMovementState.h"
#include "CStrafeState.h"
#include "COrientState.h"

#include "IBaseSimulation.h"
#include "CGameEventServer.h"

//
// Namespaces
//

namespace WWCOMMON {

OF_REGISTER(CSobFactory, ISimulationObj, CPerformer, std::string, "sobPerformer");

CPerformer::CPerformer() : ISimulationObj(), m_MotionBlendDt(0.0f) {
	// default motion / position information.
	m_MaxSpeed = 12.f;
	m_Acceleration = 15.f;
	m_BrakingFactor = 0.95f;
	m_MaxRotSpeed = 6.0f;
	m_RotAcceleration = 0.0f;
	m_RotBrakingFactor = 0.8f;

	// set up collision system
	m_MovePrimitive=::getSimulation()->getMoveContainer().addCollisionablePrimitive(0,1);
	// default to a cylinder
	m_MovePrimitive->setPrimitiveType(NLPACS::UMovePrimitive::_2DOrientedCylinder);
	// set the collision reaction type
	m_MovePrimitive->setReactionType(NLPACS::UMovePrimitive::Slide);
	// disable events for collisions.
	m_MovePrimitive->setTriggerType(NLPACS::UMovePrimitive::NotATrigger);
	// set the collision mask.
//	m_MovePrimitive->setCollisionMask(CPacsTask::OtherCollisionBit+CPacsTask::SnowballCollisionBit+CPacsTask::StaticCollisionBit);
	// set the self collision bit
//	m_MovePrimitive->setOcclusionMask(CPacsTask::SelfCollisionBit);
	// make sure the object is an obstacle.
	m_MovePrimitive->setObstacle(true);
	// hard-set the size of the primitive. later maybe use the bbox.
	m_MovePrimitive->setRadius(1.0f);
	m_MovePrimitive->setHeight(1.8f);
	// and.. insert into the world image.
	m_MovePrimitive->insertInWorldImage(0);
	m_MovePrimitive->setGlobalPosition(NLMISC::CVectorD(1.0, 1.0, 1.0), 0);
	m_MovePrimitive->UserData=m_SobId;

	m_Interpolator=new CPerformer::CPerformerInterp(2.0f, 5);
	m_CorrectionInterp = new CPerformer::CorrectInterp(2.0f, 2);
	
	addStateLayer(CStateFactory::ST_MOVEMENT);
	addStateLayer(CStateFactory::ST_ORIENTATION);
	addStateLayer(CStateFactory::ST_STRAFE);
}

CPerformer::~CPerformer() {
	if(m_Interpolator)
		delete m_Interpolator;
	if(m_CorrectionInterp)
		delete m_CorrectionInterp;
	if(m_MovePrimitive)
		::getSimulation()->getMoveContainer().removePrimitive(m_MovePrimitive);
}

NLMISC::CVector CPerformer::getPosition() { 
	NLMISC::CVector vect=m_CurrentKey.getPosition();
	return vect;
}

NLMISC::CMatrix CPerformer::getMatrix() { 
	return m_CurrentKey.getMatrix();
}

void CPerformer::setPosition(NLMISC::CVector pos) { 
	m_CurrentKey.setPosition(pos);
	CPerformer::CPerformerInterpKey key = m_CurrentKey;
	key.setTime(::getSimulation()->adjustedTime());
	m_Interpolator->addKey(key);
}

void CPerformer::setGlobalPosition(NLMISC::CVector pos) {
	m_MovePrimitive->setGlobalPosition(pos, 0, NLPACS::UGlobalPosition::Unspecified);
}

float CPerformer::getMaxSpeed() {
	return m_MaxSpeed;
}

float CPerformer::getAcceleration() {
	return m_Acceleration;
}

float CPerformer::getBrakingFactor() {
	return m_BrakingFactor;
}

float CPerformer::getMaxRotSpeed() {
	return m_MaxRotSpeed;
}

float CPerformer::getRotAcceleration() {
	return m_RotAcceleration;
}

void CPerformer::setRotAcceleration(float a) {
	m_RotAcceleration = a;
}

float CPerformer::getRotBrakingFactor() {
	return m_RotBrakingFactor;
};

void CPerformer::update() {
	m_Interpolator->update();
	m_CorrectionInterp->update();

	// handle states and update views
	ISimulationObj::update();
}

ISimulationObj *CPerformer::Create() { 
	return new CPerformer();
}

double CPerformer::getMotionBlendDt() {
	return m_MotionBlendDt;
}

void CPerformer::setMotionBlendDt(double dt) {
	m_MotionBlendDt = dt;
}

}; // END NAMESPACE WWCOMMON
